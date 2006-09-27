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
 * Implements standard inner and outer loop for performance testing
 */

/**
 * \defgroup loop_test "loop_" Standard loops for testing and reporting
 * \brief
 * These loops work use the information in T_test_app:: and in file: def_test.h
 * to execute the outer and inner loops and generate the output.
 */

/*@{*/

#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

/*
 * def_test defines the test specific paramters
 */
#include "def_test.h"
#include "bucket_stats.h"
#include "test_utils.h"
#include "loop_test.h"
#include "table.h"


/** \brief stores one line of statistics and results for all tests */
typedef struct {
	size_t count; /*!< the count value for this line */
	T_stats_record rec[NUM_TEST]; /*!< the statistics for this line */
} T_loop_result_line;

/** \brief
 * stores the table of results for counts and tests.  The results are
 * organized into lines (T_loop_result_line::) for each valid count
 * in the range defined by size, start, and incr.  Additionally, the run
 * field controls whether a given test is run at all.
 */
typedef struct {
	size_t size;	/*!< number of results lines */
	size_t start;	/*!< starting count number */
	size_t incr;	/*!< count number increment */
	T_stats_record *loop_overhead; /*!< the data for empty loop time */
	T_loop_result_line *lines; /*!< the results line for each count */
	char run[NUM_TEST];	/*!< run this test */
} T_loop_results;

/**
 * \brief
 * return the value of NUM_TEST, so we don't have to make def_tests global
 * \return NUM_TEST
 */
size_t loop_get_num_test()
{
	return NUM_TEST;
}

/**
 * \brief
 * return the test names , so we don't have to make def_tests global
 * \param test the test number
 * \return the test name or NULL if \b test is invalid
 */
const char *loop_get_testname(size_t test)
{
	return ( test < NUM_TEST) ? def_tests[test].name : NULL;
}

/**
 * \brief
 * allocates and initialize a T_loop_results:: structure.
 * The T_loop_results::lines are initialized to for each valid
 * count in the supplied T_test_app::, and the T_loop_result::run flags
 * are set based on the T_test_app::testmask
 * \param app the application state
 */
T_loop_results * loop_results_create(T_test_app *app)
{
	size_t i, j, count;
	long mask;
	T_loop_results *lr = malloc(sizeof(T_loop_results));
	lr->size = app->num_counts;
	lr->start= app->start;
	lr->incr = app->incr;
	lr->lines = malloc(lr->size * sizeof(T_loop_result_line));

	lr->loop_overhead = malloc( sizeof(T_stats_record) );
	stats_record_init(lr->loop_overhead);
	

	/* Set the test skip values from the testmask */
	mask = 1;
	for ( j = 0; j < NUM_TEST; j++ , mask = mask << 1) {
		lr->run[j] = ( mask & app->testmask ) ? 0 : 1;
	}

	/* Initialize the statistics records */
	for ( i=0, count=lr->start; i < lr->size ; i++, count+= lr->incr ) {
		lr->lines[i].count = count;
		for ( j = 0; j < NUM_TEST; j++ ) {
			if ( lr->run[j] )
				stats_record_init(&lr->lines[i].rec[j]);
		}
	}

	return lr;
}

/**
 * \brief deallocate a loop_results structure
 * \param lr the loop_results structure to destroy
 */
void loop_results_destroy(T_loop_results *lr)
{
	free(lr->lines);
	free(lr->loop_overhead);
	free(lr);
}

/**
 * \brief
 * record a data point in the appropriate T_loop_result::lines entry
 * based on the count and test indices.  Notices that the line index
 * must be solved backwards from the count and the T_loop_result::start etc.
 * as their is not a line for every count, but only for every \b valid count.
 * \param lr	the loop results struct
 * \param count	the count number for this data point
 * \param test	the test number for this data point
 * \param data	the value to record
 */
void loop_results_record(T_loop_results *lr, size_t count,
			size_t test, T_tick data)
{
	size_t ord = (count - lr->start)/lr->incr;
#ifndef BE_VERY_CAREFUL
	stats_record_datapoint(&lr->lines[ord].rec[test],data);
#else
	if ( ord < lr->size )
		stats_record_datapoint(&lr->lines[ord].rec[test],data);
	else
		fprintf(stderr,"loop:WARNING count %ld out of range\n",count);
		fflush(stderr);
#endif
}
/** Set an mfence and start the test.  On Geode GX/LX rdtsc also fences, but
 *  this may be fixed on other platforms */
#define LOOP_START_TEST(tick) \
do {	\
		rdtscll(tick);	\
} while (0)

/** End the test.  The no ops are because Rdtsc on Geode serializes, and
 * shouldn't, but we want to make sure that we don't see the impact of this */
#define LOOP_END_TEST(tick) 	\
do {	\
		asm (	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
			"nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"	\
		);	\
		rdtscll(tick);	\
} while (0)

/**
 * \brief
 * run the inner loop of the testing for a given function and count
 * if ! app->rand, the count will be constant, but should still be
 * derived from the count in \b DEF_NEXT. \b DEF_RUN should \b only
 * call the function under test with any parameter set up or pointer math
 * handled in \b DEF_NEXT. Notice that DEF_NEXT begins a block, thus extra
 * variables needed for this can be define in DEF_NEXT and referenced in
 * DEF_RUN without impacting the timestamp.
 *
 * TODO: compute an empty rdtscll() pair time to subtract from the test time
 *
 * \param ord	the ordinal number of the test, lr->lines[ord] give the "count"
 * \param test	the test index into the def_tests array
 * \param lr	the loop results struct
 * \param app	the application state
 */
void loop_inner(size_t ord, size_t test, T_loop_results *lr, T_test_app *app) {
	T_tick t1, t2;
	size_t try;
	/* Variables set by DEF_NEXT */
	size_t count; 
	T_test_sample *src;
	void *psrc;

#ifndef DEF_NO_DECLARE_DST
	T_test_sample *dst;
	void *pdst;
#endif

	/*
	 * test_count is the count used for constant count testing
	 * if ( !app->rand ) test_count sets the count in the buffer samples
	 *
	 * Regardless of app->rand, the count SHOULD come from the
	 * count embedded in the (a?) sample
	 */
	size_t test_count = lr->lines[ord].count;
	size_t tries = app->num_try;
	T_def_Fn   f = def_tests[test].f;
	size_t char_count = 0;

	if (app->verbose > 4)  {
		printf("vvvvv:%s\n",def_tests[test].name);
	}

	for ( try=0;  try < tries; try++ ) {
		DEF_NEXT(app,test_count,count,src,psrc,dst,pdst);
		if ( (app->verbose > 4)  && app->rand ) {
			printf("R%4ld:",count);
			char_count += 6;
		}
		if ( (app->verbose > 4)  && app->rand_align ) {
			printf("A%2ld:",src->align);
			char_count += 4;
			if ( (long)(psrc - src->align) & (TEST_CL_SIZE-1) )
				printf("BAD 0x%lx ",psrc);
		}
#ifndef DEF_NO_DECLARE_DST
		if ( (app->verbose > 4)  && app->rand )
			printf("D%ld ",dst->count);
#endif
		/* add a datapoint for the test overhead */
		LOOP_START_TEST(t1);
		LOOP_END_TEST(t2);
		stats_record_datapoint(lr->loop_overhead,(t2-t1));
		if (app->verbose > 4) {
			printf("L%3lld:",(t2-t1));
			char_count += 5;
		}

		LOOP_START_TEST(t1);
		DEF_RUN(f,count,psrc,pdst);
		LOOP_END_TEST(t2);

		t2 = t2 - t1;
		if (app->verbose > 4 ) {
			printf("S%08x:T%4ld:",psrc,(long) t2);
			char_count += 16;
			if ( char_count > (160-26) ) {
				printf("\n");
				char_count =0;
			}
		}
		loop_results_record(lr,count,test,t2);
	}
	if ( app->verbose > 4 )
		printf("\n");
	return ;
}

/**
 * \brief
 * compute the statistical results for this loop. This should be called
 * prior to loop_results_report()
 * \param lr	the loop results structure
 */
void loop_results_compute(T_loop_results *lr)
{
	size_t t,i, count;
	T_loop_result_line *l;
	double overhead;
	T_stats_record *r;

	/* Compute the loop overhead from the statistics gathered */
	stats_record_compute(lr->loop_overhead,0,"loop_overhead",NULL);
	overhead = lr->loop_overhead->result.wavg;

	for ( i=0;  i< lr->size ; i++ ) {
		l = &lr->lines[i];
		for (t=0;t<NUM_TEST; t++) {
			if ( !lr->run[t] )
				continue;
			r = &l->rec[t];
			/* We first compute the raw data,
			 * then offset with the overhead and compute
			 * the count weighted avg */
			stats_record_compute(r,0, def_tests[t].name,NULL);
			r->result.wavg -= overhead;
			count = (l->count) ? l->count : 1;
			r->result.dwavg = r->result.wavg/count ;
		}
	}
}

typedef double (*T_loop_results_get_func)(T_loop_results *, T_test_app *,
				size_t , size_t );
/**
 * get the raw data for this count and test
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the raw value
 */
double	loop_results_get_raw(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	return lr->lines[count].rec[test].result.wavg;
}

/**
 * get the comparative data for this count and test
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the comparative value
 */
double	loop_results_get_comp(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	double base = lr->lines[count].rec[app->baseline].result.wavg;
	return base/lr->lines[count].rec[test].result.wavg;
}

/**
 * get the comparative data for this count and test in ticks
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the comparative value
 */
double	loop_results_get_comp_diff(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	double base = lr->lines[count].rec[app->baseline].result.wavg;
	return lr->lines[count].rec[test].result.wavg - base;
}

/**
 * get the comparative data for this count and test in percent
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the comparative value
 */
double	loop_results_get_comp_pct(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	double base = lr->lines[count].rec[app->baseline].result.wavg;
	return ( base - lr->lines[count].rec[test].result.wavg )/base;
}

/**
 * get the diff data for this count and test
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the difference value
 */
double	loop_results_get_diff(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	size_t pcount = (count > 0 ) ? count -1 : 0 ;
	double prev = lr->lines[pcount].rec[test].result.wavg;
	return lr->lines[count].rec[test].result.wavg -prev;
}


/**
 * get the normalized data for this count and test
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the normalized value
 */
double	loop_results_get_normalized(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	return lr->lines[count].rec[test].result.dwavg;
}

/**
 * get the bandwith speed data for this count and test
 * @param lr	the loop results structure
 * @param app	the application state
 * @param count the virtual count (line number)
 @ @param test	the test number
 * @return the speed value
 */
double	loop_results_get_speed(	T_loop_results *lr, T_test_app *app,
				size_t count, size_t test)
{
	return ( app->clock * 1E6 * app->bandwidth_scale /
		lr->lines[count].rec[test].result.dwavg );
}

/*
		loop_results_get_speed;
*/
/**
 * Fills out one output result at the given location and type
 * @param lr	the loop results structure
 * @param app	the application state
 * @param tbl	the table to use
 * @param row_base the row orgin for the output result
 * @param col_base the column orgin for the output result
 * @param type	the type of output result to produce
 */
size_t loop_results_fill_table(T_loop_results *lr, T_test_app *app,
				T_table *tbl,
				size_t row_base, size_t col_base, char type)
{
	size_t i,t;
	size_t row_hdr  = row_base + 0;
	size_t row_data = row_base + 1;
	size_t col = col_base;
	char *label = "BOGUS";
	T_loop_results_get_func get_fun = NULL;

	switch ( type ) {
	case 'R':
		label = "ticks";
		get_fun = loop_results_get_raw;
		break;
	case 'C':
		label = "compare";
		get_fun = loop_results_get_comp;
		break;
	case 'A':
		label = "compare (ticks)";
		get_fun = loop_results_get_comp_diff;
		break;
	case 'P':
		label = "compare (%)";
		get_fun = loop_results_get_comp_pct;
		break;
	case 'D':
		label = "diff (ticks)";
		get_fun = loop_results_get_diff;
		break;
	case 'N':
		label = "ticks/byte";
		get_fun = loop_results_get_normalized;
		break;
	case 'S':
		label = "bytes/s";
		get_fun = loop_results_get_speed;
		break;
	}
	
	/* Chicken out... though this shouldn't happen */
	if ( NULL == get_fun )
		return col;
	
	table_set_cell_zstr(tbl,row_base, col, label);
	col++;
	
	/* Fill in the byte count column */
	table_set_cell_zstr(tbl,row_hdr, col, "bytes" );
	for ( i=0;  i< lr->size ; i++ ) {
		table_set_cell_u32(tbl,row_data+i,col,
				lr->lines[i].count);
	}

	col++;
	for (t=0;t<NUM_TEST; t++) {
		size_t row=row_data;
		if ( !lr->run[t] ) {
			continue;
		}
		table_set_cell_zstr(tbl,row_hdr,col,def_tests[t].name);

		for ( i=0;  i< lr->size ; i++ ) {
			double val = get_fun(lr,app,i,t);
			table_set_cell_dbl(tbl,row++,col,val);
		}

		col++;
	}
	return col;
}

/** The sum of the count values for the test series
 * @note this is double as size_t could overflow (unlikely)
 * @param lr	the loop results structure
 * @return the total of the tested counts
 */
double loop_results_count_total(T_loop_results *lr)
{
	size_t i;
	double tot=0.;
	for ( i=0;  i< lr->size ; i++ ) {
		tot += (double)lr->lines[i].count;
	}
	return tot;
}

/** The sum of the raw values for a given test
 * @param lr	the loop results structure
 * @param test	the test number
 * @return the total of the raw values
 */
double loop_results_raw_total(T_loop_results *lr, size_t test)
{
	size_t i;
	double tot=0.;
	for ( i=0;  i< lr->size ; i++ ) {
		tot += lr->lines[i].rec[test].result.wavg;
	}

	return tot;
}

/** Add summary data to the report table
 * @param lr the loop results structure
 * @param app the application state
 * @param tbl	the table to use
 * @param row_data the row orgin for the summary output
 * @param col_data the column orgin for the summary output
 * @return the row following the last entry
 */
size_t
loop_results_fill_data(T_loop_results *lr, T_test_app *app,
			T_table *tbl, size_t row_data, size_t col_data)
{
	size_t t,best_row, max_row;
	double best=HUGE_VAL;
	double max=0;
	size_t tbest=0;
	size_t tmax=0;
	size_t row = row_data;
	size_t coln = col_data;
	size_t colv = col_data + 1;
	double count_total=loop_results_count_total(lr);
	double base;
	T_zstring *name = zstring_create(NULL);

	/*
	 * we are saving the totals "just in case" we think of 
         * other statistics to compute other than best and worst
	 */
	double totals[NUM_TEST];

	table_set_cell_zstr(tbl,row++,coln,"=====");
	table_set_cell_zstr(tbl,row++,coln,"Totals:");

	table_set_cell_zstr(tbl,row,coln,"total_bytes");
	table_set_cell_dbl(tbl,row++,colv,count_total);

	/* label & reserve space for the best/worst at the top of the list */
	best_row = row;
	table_set_cell_zstr(tbl,row++,coln,"best_test");
	table_set_cell_zstr(tbl,row++,coln,"best_total");

	max_row = row;
	table_set_cell_zstr(tbl,row++,coln,"worst_test");
	table_set_cell_zstr(tbl,row++,coln,"worst_total");

	/* report totals data for each test */
	for (t=0;t<NUM_TEST; t++) {
		if ( !lr->run[t] ) {
			continue;
		}
		totals[t]=loop_results_raw_total(lr,t);
		if ( totals[t] < best ) {
			best = totals[t];
			tbest = t;
		}
		if ( totals[t] > max ) {
			max = totals[t];
			tmax = t;
		}
		zstring_set(name,"total_");
		zstring_cat(name,def_tests[t].name);
		table_set_cell_zstr(tbl,row,coln,zstring_get(name));
		table_set_cell_dbl(tbl,row,colv,totals[t]);
		row++;
	}

	if ( app->fmt.compare ) {
		/* report ratio data for each test */
		base = totals[app->baseline];
		for (t=0;t<NUM_TEST; t++) {
			if ( !lr->run[t] ) {
				continue;
			}
			zstring_set(name,"ratio_");
			zstring_cat(name,def_tests[t].name);
			table_set_cell_zstr(tbl,row,coln,zstring_get(name));
			table_set_cell_dbl(tbl,row,colv,base/totals[t]);
			row++;
		}
	}

	/* and the summary data */
	table_set_cell_zstr(tbl,best_row,colv,def_tests[tbest].name);
	table_set_cell_dbl(tbl,best_row+1,colv,best);

	table_set_cell_zstr(tbl,max_row,colv,def_tests[tmax].name);
	table_set_cell_dbl(tbl,max_row+1,colv,max);

	return row;
}

/**
 * \brief
 * print the statistical results for this loop.  This should not be
 * called until after loop_results_compute()
 * \param lr the loop results structure
 * \param app the application state
 */
void loop_results_report(T_loop_results *lr, T_test_app *app)
{
	size_t e,t;
	T_table *tbl = table_create(1,1); /* we use this to count the data rows */
	size_t tests_run=0;
	size_t row_base = 0;
	size_t rows = row_base + lr->size + 2;
	size_t cols = app->fmt.data_cols;
	size_t col_base = app->fmt.data_cols;
	size_t col_data = 0;
	size_t row_data = 0;

	/* this works because T_table tolerates out out bound addresses */
	size_t data_rows =	test_app_fill_data(app,tbl,0,0) +
				loop_results_fill_data(lr,app,tbl,0,0);
	

	/* Set up the results table */
	for (t=0;t<NUM_TEST; t++) {
		if ( lr->run[t] ) {
			tests_run++;
		}
	}

	cols += ( tests_run + 2 ) * app->fmt.emit_num;

	if (  app->fmt.data_cols > 1 && data_rows > rows ) {
		rows = data_rows;
	}

	table_destroy(tbl); /* junk the bogus one */
	tbl = table_create(rows,cols);

	/* Fill in the application state data */
	row_data = test_app_fill_data(app,tbl,row_data,col_data);
	

	/* fill in the output results */
	for ( e = 0; e < app->fmt.emit_num; e++ ) {
		col_base = loop_results_fill_table(lr,app,tbl,row_base,col_base,
			app->fmt.emit[e]);
	}


	/* Add summary data */
	if ( app->fmt.data_cols > 1 ) {
		row_data = loop_results_fill_data(lr,app,tbl,row_data,col_data);
	}

	table_print(tbl,stdout,app->fmt.tbl_fmt);
	table_destroy(tbl);

}


/**
 * \brief
 * Set up and run the inner loop for the test, and then report the results
 * \param app the application state
 */
void loop_test(T_test_app *app)
{
	size_t i,test;
	T_loop_results *lr;
	
	/*
	 *
	 */ 
	stats_init_package();
	lr = loop_results_create(app);
	
	/*
	 * Loop through the tests and counts, the buffer tocs drive
	 * the counts.
	 */
	for (test=0;test<NUM_TEST; test++) {
		if ( lr->run[test] ) {
			for ( i=0;  i< lr->size ; i++ ) {
				loop_inner(i,test,lr,app);
			}
		}
	}

	loop_results_compute(lr);
	loop_results_report(lr,app);
	loop_results_destroy(lr);
}

