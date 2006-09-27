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
#ifndef _TABLE_H_
#define _TABLE_H_

/**
 * \file
 * \brief
 * Types and default settings for  \ref simple_table and \ref table_cell
 */
#include <stdio.h>
#include "zstring.h"


/**
 * The list of valid types for T_cell::data, stored in T_cell::type
 */
typedef enum E_cell_type {
	CELL_UNINIT = 0,	/*!<T_cell::data is uninitialized	    */
        CELL_DOUBLE,		/*!<T_cell::data is a double		    */
        CELL_LONG,		/*!<T_cell::data is a long		    */
        CELL_ULONG,		/*!<T_cell::data is an unsigned long	    */
        CELL_LONGLONG,		/*!<T_cell::data is a long long		    */
        CELL_ULONGLONG,		/*!<T_cell::data is an unsigned long long   */
        CELL_VOID,		/*!<T_cell::data is a void *		    */
	CELL_ZSTRING		/*!<T_cell::data is a string (T_zstring:: ) */
} T_cell_type;

/**
 * The formats to use when printing cell data, one for each type in
 * T_cell_type::
 */
typedef struct S_cell_fmt {
	const char *DOUBLE;	/*!<format for T_cell_type::CELL_DOUBLE	   */
	const char *LONG;	/*!<format for T_cell_type::CELL_LONG	   */
	const char *ULONG;	/*!<format for T_cell_type::CELL_ULONG	   */
	const char *LONGLONG;	/*!<format for T_cell_type::CELL_LONGLONG  */
	const char *ULONGLONG;	/*!<format for T_cell_type::CELL_ULONGLONG */
	const char *VOID;	/*!<format for T_cell_type::CELL_VOID	   */
	const char *ZSTRING;	/*!<format for T_cell_type::CELL_ZSTRING   */
} T_cell_fmt;

/**
 * The class data for T_cell::
 */
typedef struct S_cell_class {
	T_cell_fmt *fmt;	/*!<the default format for cell_print() */
} T_cell_class;

/**
 * The flexible storage union for T_cell::
 */
typedef union U_cell_data {
	double		   DOUBLE;	/*!<for T_cell_type::CELL_DOUBLE    */
	long		   LONG;	/*!<for T_cell_type::CELL_LONG      */
	unsigned long	   ULONG;	/*!<for T_cell_type::CELL_ULONG     */
	long long	   LONGLONG;	/*!<for T_cell_type::CELL_LONGLONG  */
	unsigned long long ULONGLONG;	/*!<for T_cell_type::CELL_ULONGLONG */
	const void *	   VOID;	/*!<for T_cell_type::CELL_VOID      */
	T_zstring *	   ZSTRING;	/*!<for T_cell_type::CELL_ZSTRING   */
} T_cell_data;

/**
 * The data defining a T_cell:: instance
 *
 * The use of capitilized names makes it convenient to access
 * in macros using CELL_##field to indicate the type for the field
 */
typedef struct S_cell {
	T_cell_data data;	/*!<the data storage union	*/
	T_cell_type type;	/*!<the type of data stored	*/
} T_cell;

T_cell *cell_create_array(size_t num);
void cell_destroy_array(T_cell *cell_array, size_t num);
T_cell *cell_set(T_cell *c, T_cell_type type, const void *data);
int cell_fprintf(T_cell *c, FILE *out, T_cell_fmt *fmt );
int cell_sprintf(T_cell *c, T_zstring *z, T_cell_fmt *fmt );


extern T_cell_fmt	g_cell_fmt_default;
extern T_cell_class	g_cell_class;

/**
 * Store the strings to control formatting of T_table::
 */
typedef struct S_table_fmt {
	const char *	sep;		/*!<string between cells	*/
	const char *	start;		/*!<string at table start	*/
	const char *	end;		/*!<string at table end		*/
	const char *	start_row;	/*!<string before each row	*/
	const char *	end_row;	/*!<string after each row	*/
	const char *	start_cell;	/*!<string before each cell	*/
	const char *	end_cell;	/*!<string after each cell	*/
	int		no_return;	/*!<suppress \n after rows	*/
	int		auto_pad;	/*!<create fixed width output	
					 *  using space "padding" */
} T_table_fmt;

/**
 * class data for T_table::
 */
typedef struct S_table_class {
	T_table_fmt *fmt; 		/*!<the format for table_print */
} T_table_class;

/**
 * the data defining a T_table:: instance
 */
typedef struct S_table {
	size_t rows;	/*!<the number of rows in the table	*/
	size_t cols;	/*!<the number of columns in the table	*/
	size_t max;	/*!<the number of cells in the table	*/
	T_cell *cells;	/*!<the array of cells of the table	*/
} T_table;

extern T_table_fmt g_table_fmt_default;
extern T_table_fmt g_table_fmt_html;
extern T_table_fmt g_table_fmt_csv;
extern T_table_class g_table_class;

T_table *table_create(size_t rows, size_t cols);
void table_destroy(T_table *t);
T_cell *table_get_cell(T_table *t, size_t row, size_t col);
T_cell *table_set_cell(T_table *t, size_t row, size_t col,
			T_cell_type type, const void *data);
T_cell *table_set_cell_zstr(T_table *t, size_t row, size_t col,
				const char *zstr);
T_cell *table_set_cell_dbl(T_table *t, size_t row, size_t col, double dbl);
T_cell *table_set_cell_u64(T_table *t, size_t row, size_t col,
				unsigned long long u64);
T_cell *table_set_cell_u32(T_table *t, size_t row, size_t col,
				unsigned long u32);
T_cell *table_reformat(T_table *t, size_t row, size_t col, T_cell_fmt *fmt);
void table_print(T_table *t,FILE *out, T_table_fmt *fmt);

#endif
