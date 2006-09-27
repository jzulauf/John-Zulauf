/*
 * Copyright (c) 2006, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 *    * Redistributions of source code must retain the above copyright 
 *      notice, this list of conditions and the following disclaimer.
 *    * Neither the name of Advanced Micro Devices, Inc. nor the names
 *      of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief
 * Implements a simple table object
 */
#include "table.h"
/**
 * \defgroup table_cell "cell_" A smart union datatype
 * \brief
 * T_cell:: can store and print a variety of data, allowing for
 * flexible storage, such as is useful in an array.
 */

/*@{*/

/**
 * default print formats
 *
 * These formats defined the output for T_cell:: fields. 
 */
T_cell_fmt g_cell_fmt_default = {
	.DOUBLE		= "% lg",
	.LONG		= "%ld",
	.ULONG		= "%lu",
	.LONGLONG	= "%Ld",
	.ULONGLONG	= "%Lu",
	.VOID		= "0x%08lx",
	.ZSTRING	= "%s"
};

/**
 * The class object for T_cell::
 */
T_cell_class g_cell_class = {
	.fmt		= &g_cell_fmt_default
};

/**
 * Array creator for T_cell::
 *
 * Since the "uninitialized" type is 0, we simply calloc to create
 * 
 * @param num	the number of T_cell:: to create
 * @return the array of T_cell:: created
 */
T_cell *cell_create_array(size_t num)
{
	T_cell *cell_array = calloc(num,sizeof(T_cell));
	return cell_array;
}
/**
 * Destructor for arrays of cells
 *
 * WARNING: num must match the number passed to the creator
 */
void cell_destroy_array(T_cell *cell_array, size_t num)
{
	size_t i;
	for (i=0; i < num ; i++ ) {
		T_cell *c = cell_array + i;
		if ( CELL_ZSTRING == c->type ) {
			zstring_destroy(c->data.ZSTRING);
		}
	}
	free(cell_array);
}

/**
 * A define to simplify setting the T_cell:: fields except ZSTRING
 */
#define CELL_SET_CASE(cell,dtype,ctype,data)	\
	case CELL_##dtype:				\
		cell->data.dtype = *(ctype *)data;	\
		cell->type = CELL_##dtype;		\
		break;


/**
 * Sets the value
 *
 * @param c	the cell to set (if NULL, function is NOOP)
 * @param type	the type of data
 * @param data	the address of the data to store (except for type VOID where
 *		the parameter is the data to store).  ZSTRING are copied
 *		into a T_zstring::.
 * @return the cell itself (can be NULL)
 *
 * Note: changing a cell from type ZSTRING destroys the T_zstring:: referenced
 * do \b not keep a pointer to the T_zstring:: in your own code.
 */
T_cell *cell_set(T_cell *c, T_cell_type type,
				const void *data)
{
	if ( NULL == c ) {
		return c;
	}

	/* Clean up the old zstring data if we are changing away from it */
	if ( type != c->type && CELL_ZSTRING == c->type ) {
		zstring_destroy(c->data.ZSTRING);
	}

	/* Make the correct assigment */
	switch (type) {
	case CELL_ZSTRING:
		if ( CELL_ZSTRING == c->type ) {
			zstring_set(c->data.ZSTRING,(const char*)data);
		} else {
			c->data.ZSTRING = zstring_create((const char *)data);
			c->type = CELL_ZSTRING;
		}
		break;
	CELL_SET_CASE(c,DOUBLE,double,data)
	CELL_SET_CASE(c,LONG,long,data)
	CELL_SET_CASE(c,ULONG,unsigned long,data)
	CELL_SET_CASE(c,LONGLONG,long long,data)
	CELL_SET_CASE(c,ULONGLONG,unsigned long long,data)
	case CELL_VOID:
	default:
		c->data.VOID = data;
		c->type = CELL_VOID;
	}

	return c;
}


/**
 * fprintf the cell to the given FILE
 *
 * @param c	the cell to fprintf
 * @param out	the file to write
 * @param fmt	the cell format to use, or NULL for to use T_cell_class::fmt
 * @return	the return value fprintf
 */
int cell_fprintf(T_cell *c, FILE *out, T_cell_fmt *fmt)
{
	int ret=0;
	T_zstring *zf = zstring_create(NULL);
	ret = cell_sprintf(c,zf,fmt);
	if ( ret ) {
		fprintf(out,"%s",zstring_get(zf));
	}
	zstring_destroy(zf);
	return ret;
}

/** Try to snprintf with the given length */
#define CELL_ZPRINTF_TRY(pz,pfmt,data,ret,try_len) {		\
	zstring_alloc(pz,try_len);				\
	ret = snprintf((char *)zstring_get(pz),try_len+1,pfmt,data);	\
}

/** snprintf into a zstring. Adaptively resize the zstring until
 *  the snprintf fits */
#define CELL_ZPRINTF(pz,pfmt,data,ret) {			\
	if ( pfmt ) {						\
		CELL_ZPRINTF_TRY(pz,pfmt,data,ret,16);		\
		if ( ret > 16 ) {				\
			CELL_ZPRINTF_TRY(pz,pfmt,data,ret,ret);	\
		}						\
	} else {						\
		ret = 0;					\
	}							\
}

/**
 * A generic sprintf macro for T_cells::  except ZSTRING
 */
#define CELL_DO_ZPRINT_CASE(pz,fmt,cell,dtype,ret)	\
	case CELL_##dtype:				\
		CELL_ZPRINTF(pz,fmt->dtype,cell->data.dtype,ret);\
		break;

/**
 * A generic sprintf macro for T_cells:: of type ZSTRING
 */
#define CELL_DO_ZPRINT_CASE_ZSTRING(pz,fmt,cell,ret,tstr)	\
	case CELL_ZSTRING:					\
		tstr = zstring_get(cell->data.ZSTRING);		\
		CELL_ZPRINTF(pz,fmt->ZSTRING,tstr,ret);	\
		break;

/**
 * sprintf the cell to the passed zstring using the given fmt structure
 * Note: will alloc zstring as needed.
 * @param c	the cell to sprintf
 * @param z	the T_zstring:: to hold the result (automatically resized)
 * @param fmt	the cell format to use, or NULL for to use T_cell_class::fmt
 * @return	the return value sprintf (number of characters formatted)
 */
int cell_sprintf(T_cell *c, T_zstring *z, T_cell_fmt *fmt)
{
	int ret=0;
	if ( NULL != c ) {
		const char *temp_str;
		fmt = ( NULL != fmt ) ? fmt : g_cell_class.fmt;
		switch(c->type) {
		CELL_DO_ZPRINT_CASE_ZSTRING(z,fmt,c,ret,temp_str)
		CELL_DO_ZPRINT_CASE(z,fmt,c,DOUBLE,ret)
		CELL_DO_ZPRINT_CASE(z,fmt,c,LONG,ret)
		CELL_DO_ZPRINT_CASE(z,fmt,c,ULONG,ret)
		CELL_DO_ZPRINT_CASE(z,fmt,c,LONGLONG,ret)
		CELL_DO_ZPRINT_CASE(z,fmt,c,ULONGLONG,ret)
		CELL_DO_ZPRINT_CASE(z,fmt,c,VOID,ret)
		default:
			break;
		}
	}
	return ret;
}

/*@}*/
	

/**
 * \defgroup simple_table "table_" Simple table utility API
 * \brief
 * Implements a simple table of \ref table_cell
 *
 * TODO
 *    - Add convenience "set" for the rest of the cell types
 */

/*@{*/

#include <limits.h>

/**
 * The default formatting for a T_table::
 */
T_table_fmt g_table_fmt_pad = {
	sep:" ",
	start:NULL,
	end:NULL,
	start_row:NULL,
	end_row:NULL,
	start_cell:NULL,
	end_cell:NULL,
	no_return:0,
	auto_pad:1
};

/**
 * The csv formatting for a T_table::
 */
T_table_fmt g_table_fmt_csv = {
	sep:",",
	start:NULL,
	end:NULL,
	start_row:NULL,
	end_row:NULL,
	start_cell:NULL,
	end_cell:NULL,
	no_return:0,
	auto_pad:0
};


/**
 * Formatting for a T_table:: which will produce an HTML table
 */
T_table_fmt g_table_fmt_html = {
	sep:NULL,
	start:"<table>",
	end:"</table>",
	start_row:"<tr>",
	end_row:"</tr>",
	start_cell:"<td>",
	end_cell:"</td>",
	no_return:0,
	auto_pad:0
};

/**
 * The class data for T_table::
 */
T_table_class g_table_class = {
	fmt:&g_table_fmt_pad
};

/**
 * The creator for T_table::
 *
 * @param rows the number of rows for the table
 * @param cols the number of columns for the table
 * @return the T_table:: created
 */
T_table *table_create(size_t rows, size_t cols) 
{
	T_table *t= malloc(sizeof(T_table));
	t->rows  = rows;
	t->cols  = cols;
	t->max   = rows * cols;
	t->cells = cell_create_array(t->max);

	return t;
}

/**
 * The destroyer for T_table::
 *
 * @param t	the table to destroy
*/
void table_destroy(T_table *t)
{
	cell_destroy_array(t->cells,t->max);
	free(t);
}

/**
 * Look up the cell based on the row, colum pair in a T_table::
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 */
T_cell *table_get_cell(T_table *t, size_t row, size_t col)
{
	size_t cell = (row * t->cols) + col;
	if ( ( row >= t->rows ) || ( col >= t->cols )  ) {
		return NULL;
	}
	return &(t->cells[cell]);
}

/**
 * Store a value in a cell
 *
 * This is a convenience function for strings as a commonly used type
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 * @param type	the type of data
 * @param data	the address of the data to store (except for type VOID where
 *		the parameter is the data to store).  ZSTRING are copied
 *		into a T_zstring::.
 *
 * @returns the cell set or NULL if row or column out-of-bounds
 */

T_cell *
table_set_cell(T_table *t, size_t row, size_t col,
			T_cell_type type, const void *data)
{
	return cell_set(table_get_cell(t,row,col),type,data);
}

/**
 * Store a NULL terminated string in a cell
 *
 * This is a convenience function for strings as a commonly used type
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 * @param zstr	the NULL terminated string to store (the string is copied)
 *
 * @returns the cell set or NULL if row or column out-of-bounds
 */

T_cell *
table_set_cell_zstr(T_table *t, size_t row, size_t col, const char *zstr)
{
	return cell_set(table_get_cell(t,row,col),
		CELL_ZSTRING,(const void *)zstr);
}

/**
 * Store a double in cell(row,col) of the T_table::
 *
 * This is a convenience function for double as a commonly used type
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 * @param dbl	the value to store
 *
 * @returns the cell set or NULL if row or column out-of-bounds
 */

T_cell *
table_set_cell_dbl(T_table *t, size_t row, size_t col, double dbl)
{
	return cell_set(table_get_cell(t,row,col),
			CELL_DOUBLE, (const void *)&dbl);
}

/**
 * Store a unsigned long long in cell(row,col) of the T_table::
 *
 * This is a convenience function for unsigned long long as a commonly used type
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 * @param u64	the value to store
 *
 * @returns the cell set or NULL if row or column out-of-bounds
 */
T_cell *
table_set_cell_u64(T_table *t, size_t row, size_t col, unsigned long long u64)
{
	return cell_set(table_get_cell(t,row,col),
			CELL_ULONGLONG, (const void *)&u64);
}

/**
 * Store a unsigned long in cell(row,col) of the T_table::
 *
 * This is a convenience function for unsigned long long as a commonly used type
 *
 * @param t the table
 * @param row	the row
 * @param col	the column
 * @param u32	the value to store
 *
 * @returns the cell set or NULL if row or column out-of-bounds
 */
T_cell *
table_set_cell_u32(T_table *t, size_t row, size_t col, unsigned long u32)
{
	return cell_set(table_get_cell(t,row,col),
			CELL_ULONG, (const void *)&u32);
}

/** Reformat a cell from it's native type to a string using sprintf and
 *  the passed format table.
 * @param t	the table
 * @param row	the row
 * @param col	the column
 * @param fmt	the fmt structure
 */
T_cell *table_reformat(T_table *t, size_t row, size_t col, T_cell_fmt *fmt)
{
	T_zstring *z = zstring_create(NULL);
	T_cell *cell = table_get_cell(t,row,col);
	cell_sprintf(cell,z,fmt);
	cell_set(cell,CELL_ZSTRING,zstring_get(z));
	zstring_destroy(z);
	return cell;
}

/** find the width of the column in the current fmt
 * @param t	the table to check
 * @param col	the column to scan
 * @param fmt	the fmt for the table (use g_table_class::fmt if NULL)
 * @return the maximum width for the column
 */
int table_get_column_width(T_table *t, size_t col, T_table_fmt *fmt)
{
	size_t row;
	int width = 0;
	T_zstring *z= zstring_create(NULL);
	fmt = ( NULL != fmt ) ? fmt : g_table_class.fmt;
	if ( col < t->cols ) {
		for ( row = 0; row < t->rows; row++ ) {
			T_cell *cell = table_get_cell(t,row,col);
			int cw = cell_sprintf(cell,z,NULL);
			width = (cw > width) ? cw : width;
		}
	}
	zstring_destroy(z);
	return width;
}

/**
 * print the table using the current format
 *
 * @param t	the table to print
 * @param out	the file to which to print
 * @param fmt	the fmt for the table (use g_table_class::fmt if NULL)
 */
void table_print(T_table *t,FILE *out, T_table_fmt *fmt)
{
	size_t r;
	size_t c;

	int pad_count;
	int pad;
	int count;
	int *width= NULL;
	T_zstring *zsep = zstring_create(NULL);

	fmt = ( NULL != fmt ) ? fmt : g_table_class.fmt;

	if ( fmt->auto_pad) {
		/* find the widths of all of the columns */
		width = calloc(t->cols,sizeof(int));
		for ( c = 0; c < t->cols; c++ ) {
			width[c] = table_get_column_width(t,c,fmt);
		}
	}

	if ( fmt->start ) {
		fprintf(out,fmt->start);
	}

	for ( r = 0; r < t->rows; r++ ) {
		size_t cmax; /*max valid column this row */
		zstring_set(zsep,NULL);

		if ( fmt->start_row ) {
			fprintf(out,fmt->start_row);
		}

		/* Find the max valid cell...  */
		for ( c = 0; c < t->cols; c++ ) {
			T_cell *cell = table_get_cell(t,r,c);
			if ( CELL_UNINIT != cell->type ) {
				cmax = c + 1;
			}
		}
			

		for ( c = 0; c < cmax; c++ ) {
			T_cell *cell = table_get_cell(t,r,c);

			if ( fmt->start_cell ) {
				fprintf(out,fmt->start_cell);
			}

			count = 0;
			if ( CELL_UNINIT != cell->type ) {
				fprintf(out,zstring_get(zsep));
				zstring_set(zsep,NULL);
				count = cell_fprintf(cell, out, NULL);
			}


			pad_count = (width) ? width[c] - count : 0;
			zstring_pad(zsep,' ',pad_count);
			zstring_cat(zsep,fmt->sep);
			

			if ( fmt->end_cell ) {
				fprintf(out,zstring_get(zsep));
				zstring_set(zsep,NULL);
				fprintf(out,fmt->end_cell);
			}
		}

		if ( fmt->end_row ) {
			fprintf(out,fmt->end_row);
		}

		if ( !fmt->no_return) {
			fprintf(out,"\n");
		}
	}

	if ( fmt->end ) {
		fprintf(out,fmt->end);
	}

	/* Clean up */
	zstring_destroy(zsep);
	if ( NULL != width ) {
		free(width);
	}
}
			


