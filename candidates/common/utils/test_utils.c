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
 * Implements a the test framework including application state and buffer
 * managment.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "opts_utils.h"
#include "test_utils.h"
#include "bucket_stats.h"
#include "fake_lib.h"
#include "loop_test.h"


/**
 * \defgroup test "test_" Test framework for performance testing
 * @{
 */

/**
 * \brief
 * Initializes the app state structure and command line (opts table)
 * \param progname name of this program (information only)
 * \param progversion name of this program (for help and usage)
 * \param type the cannonical form of the test 
 */
T_test_app *test_init(const char *progname, const char *progversion,
				T_test_type type)
{
	T_test_app *app = test_app_create(progname,progversion,type);
	opts_init(app->name,app->vers);
	test_app_default_opts(app);
	return app;
}


/**
 * \brief
 * Allocates the default set of buffers
 * \b NOTE: init buffers computes and allocates the buffers and the TOC
 * but \b doesn't fill them. It \b does look at the T_test_app settings
 * and the test_type to initialize the count and wrap call backs 
 * \param app the application state
 * \param scale size multiplier the samples: samp_size = count * scale + extra
 * \param extra additional bytes for each sample (see scale parameter)
 */
void test_create_buffers(T_test_app *app, size_t scale, size_t extra)
{
	size_t i;
	size_t buf_max = TEST_BUFFER_MAX;

	if ( TEST_IS_ONE_BUFFER(app->type) ) {
		buf_max = 1;
	}

	app->bufs = malloc(buf_max * sizeof(T_test_buffer *));
	for (i = 0; i < buf_max; i++ ) {
		app->bufs[i]=test_buffer_create(i,app,scale,extra);
	}
}


/**
 * \brief
 * Callback for opts_getopt() and the --oneshot flag
 * \param ptr_to_test_app action callback data pointing to the
 *	T_test_app structure
 * \return 0 (action successful)
 */
int test_action_oneshot(void *ptr_to_test_app)
{
	T_test_app *app = (T_test_app *)ptr_to_test_app;
	if ( !app )
		return 1;
	app->start = strtol(optarg, NULL,0);
	app->end   = app->start;
	return 0;
}

/*@}*/

/**
 * \defgroup test_app Test application state management
 * \ingroup test
 * @{
 */
#ifndef TEST_DEFAULT_START
/** \def TEST_DEFAULT_START \brief default start count */
#	define TEST_DEFAULT_START  0
#endif

#ifndef TEST_DEFAULT_INCR
/** \def TEST_DEFAULT_INCR \brief default count increment */
#	define TEST_DEFAULT_INCR   1
#endif

#ifndef TEST_DEFAULT_END
/** \def TEST_DEFAULT_END brief default end count */
#	define TEST_DEFAULT_END   33
#endif

#ifndef TEST_DEFAULT_NUM_TRY
/** \def TEST_DEFAULT_NUM_TRY
 * \brief default number of times to run each test for each count */
#	define TEST_DEFAULT_NUM_TRY 10000
#endif

/**
 * \brief
 * Allocate the test application state structure and set the default values 
 * \param name the program name to display
 * \param vers the program version to display
 * \param type the type of operation being tested \see test_init_buffers()
 * return the application state structure
 */
T_test_app *test_app_create(const  char *name, const char *vers,
					T_test_type type)
{
	T_test_app *app = calloc(1,sizeof(T_test_app));


	app->type = type;
	app->name = name ? name : "BOGUS NAME";
	app->vers = vers ? vers : "BOGUS VERSION";
	app->start = TEST_DEFAULT_START;
	app->end = TEST_DEFAULT_END;
	app->incr = TEST_DEFAULT_INCR;
	app->num_try = TEST_DEFAULT_NUM_TRY;
	app->bandwidth_scale = 1.;
	app->clock = 500;

	return app;

}

/**
 * option handling callback for the format flag
 */
int  test_app_opts_cb_format(void* data) {
	T_test_app_fmt *fmt = (T_test_app_fmt *)data;
	char c, *fstr = optarg;
	
	while ( c = *(fstr++) ) {
		fmt->opts[c]++;
		if (isupper(c) && (fmt->emit_num < TEST_APP_TABLE_MAX ) ) {
			if ( NULL != strchr("APC",c) ) {
				fmt->compare = 1;
			}
			fmt->emit[fmt->emit_num++] = c;
		}	
	}
	return 0;
}

/**
 * \brief
 * Add the common test commandline options to the opts table used
 * by opts_getopt()
 * \param app the application state structure
 */
void test_app_default_opts(T_test_app *app) {
	opts_default_opts();
	opts_def('o',"oneshot","test with start = end = '%s'","count",
			test_action_oneshot,(void*)app);	
	OPTS_DEF_STRTOL('s',"start","start test at '%s'","count",app->start);	
	OPTS_DEF_STRTOL('e',"end","end test at '%s' (inclusive)","count",
			app->end);	
	OPTS_DEF_STRTOL('i',"incr","step test count by '%s'","incr",app->incr);	
	OPTS_DEF_STRTOL('t',"tries","run test '%s' times","try",app->num_try);
	OPTS_DEF_STRTOL('C',"clock","CPU speed '%s'","speed_in_MHz",app->clock);
	OPTS_DEF_STRTOL('a',"alignment","offset start byte by '%s' from dword",
			"off",app->align);	
	OPTS_DEF_INCR('v',"verbose","increase vebosity, multiple '-v' for more",
			app->verbose);	

	/* The format flag is more complex than average */
	opts_def('F',"format",
			"use '%s' to control formatting, default N"
			"\n\t\tValid fmt characters:"
			"\n\t\t\tR: emit raw data table"
			"\n\t\t\tA: emit comparative data (raw difference)"
			"\n\t\t\tC: emit comparative data table (ratio)"
			"\n\t\t\tP: emit comparative data (percent difference)"
			"\n\t\t\tD: emit first difference data table"
			"\n\t\t\tN: emit normalized data table (default)"
			"\n\t\t\tS: emit speed (rate) data table"
			"\n\t\t\tX: don't emit a default table"
			"\n\t\t\tTables are output in the order specified"
			"\n\t\t\tc: use cvs format"
			"\n\t\t\th: use html format"
			"\n\t\t\td: suppress test information columns",

		"fmt", test_app_opts_cb_format, (void *)&app->fmt);

	OPTS_DEF_STRTOL('b',"baseline",
			"use '%s' (starting at 0) as baseline for -F C",
			"testnum",app->baseline);	
	
	OPTS_DEF_SET('R',"random","randomly vary count each function call",
			app->rand);
	OPTS_DEF_SET('A',"align-random","randomly vary alignment each function call",
			app->rand_align);
	OPTS_DEF_STRTOL('S',"seed","set intial seed value for rand_r to '%s'",
			"seed",app->seed);	
	OPTS_DEF_STRTOL('T',"testmask","disable tests matching bits in '%s'",
			"mask",app->testmask);	
}

/**
 * \brief
 * Outputs test option information
 * \param app the application state structure
 */
void test_app_print_options(T_test_app *app)
{
	if ( !app->verbose )
		return;

        printf("%s version %s\n",app->name,app->vers);
	printf("start\t%d\tend\t%d\n",app->start,app->end);
	printf("incr\t%d\tntry\t%d\n",app->incr,app->num_try);
        printf("align\t%d\tverbose\t%d\n", app->align, app->verbose);
        printf("baseline\t%d\n", app->baseline);
        printf("seed\t%d\trand\t%d\n", app->seed, app->rand);
        printf("rand_align\t%d\t", app->rand_align);
        printf("testmask\t0x%02lx\n", app->testmask);
	printf("f_opts\t%s\temit\t%s\n",app->fmt.opts_zstr,app->fmt.emit);
}

/** put a long value into the data column of the table */
#define TEST_APP_DATA_ULONG(a,t,r,c,f) {				\
	table_set_cell_zstr(t,r,c,#f);				\
	table_set_cell_u32(t,r,(c+1),(unsigned long)(a->f));	\
	r++;							\
}

/** put a string value into the data column of the table */
#define TEST_APP_DATA_ZSTR(a,t,r,c,f) {				\
	table_set_cell_zstr(t,r,c,#f);				\
	table_set_cell_zstr(t,r,(c+1),a->f);	\
	r++;							\
}

/**
 * Output the test spplication state information
 * @param app	the application state structure
 * @param t	the table to fill
 * @param row	the starting row for the data 
 * @param col	the starting column for the data
 * @return the row following the last data value
 */
size_t test_app_fill_data(T_test_app *app, T_table *t, size_t row, size_t col)
{
	if (app->fmt.data_cols < 2)
		return row;

	TEST_APP_DATA_ZSTR(app,t,row,col,name);
	TEST_APP_DATA_ZSTR(app,t,row,col,vers);
	TEST_APP_DATA_ULONG(app,t,row,col,start);
	TEST_APP_DATA_ULONG(app,t,row,col,end);
	TEST_APP_DATA_ULONG(app,t,row,col,incr);
	TEST_APP_DATA_ULONG(app,t,row,col,num_try);
	TEST_APP_DATA_ULONG(app,t,row,col,align);
	TEST_APP_DATA_ULONG(app,t,row,col,baseline);
	TEST_APP_DATA_ULONG(app,t,row,col,rand);
	TEST_APP_DATA_ULONG(app,t,row,col,rand_align);
	TEST_APP_DATA_ULONG(app,t,row,col,seed);
	TEST_APP_DATA_ULONG(app,t,row,col,testmask);
	TEST_APP_DATA_ULONG(app,t,row,col,clock);
	if ( app->verbose ) {
		TEST_APP_DATA_ULONG(app,t,row,col,verbose);
		TEST_APP_DATA_ULONG(app,t,row,col,bandwidth_scale);
		TEST_APP_DATA_ZSTR(app,t,row,col,fmt.opts_zstr);
		TEST_APP_DATA_ZSTR(app,t,row,col,fmt.emit);
		TEST_APP_DATA_ULONG(app,t,row,col,fmt.emit_num);
	}
	
	return row;
	
}
	

/** \def TEST_WARN(fmt,val) \brief print a warning to stderr */
#define TEST_WARN(fmt,val) fprintf(stderr,"WARN: " fmt,val)
/** \def TEST_FATAL(fmt,val) \brief print an error msg to stderr */
#define TEST_FATAL(fmt,val) {	\
	fprintf(stderr,"FATAL: " fmt,val); exit(42); \
}

/**
 * \brief
 * Make sure the user inputs are "sane"
 * \param app the application state structure
 */
void test_app_validate_options(T_test_app *app)
{
	char *write_opt = app->fmt.opts_zstr;
	char *read_opt = app->fmt.opts;
	size_t i;
	size_t num_test;
	/* validate the standard options */
	if ( app->start < 0 )
		app->start = 0;

	if ( app->end < 0 )
		app->end = 0;

#ifdef TEST_MAX_COUNT
	if ( app->end >= TEST_MAX_COUNT )
		app->end = TEST_MAX_COUNT;
#endif

	if ( app->start > app->end )
		app->start = app->end;

	if ( app->incr < 1 )
		app->incr = 1;

	if ( app->num_try < 1)
		app->num_try = 1;

	/* limit to the maximum alignment offset */
	/* with rand align, default align is "max" */
	if (	( app->align >=  TEST_ALIGN_SIZE ) ||
		( app->rand_align && !app->align ) ) {
		app->align  = TEST_ALIGN_SIZE-1;
	}

	/*
	 * To get even "decks" of aligment we need to adjust the try count
	 * to a multiple of the number of alignments (rounding up)
	 */
	if ( app->rand_align ) {
		size_t num_align = app->align + 1;
		size_t rem	 = app->num_try % num_align;
		app->num_try +=  rem ? num_align - rem : 0;
	}

	/*
	 * Validate the baseline
	 */
	num_test = loop_get_num_test();	

	if ( num_test < 1 )
		TEST_FATAL("Invalid number of tests %ld\n",num_test);

	if ( app->baseline < 0 ) {
		app->baseline = 0;
		TEST_WARN("Invalid baseline, reset to %ld\n",app->baseline);
	} else if ( app->baseline >= num_test ) {
		app->baseline = num_test - 1;
		TEST_WARN("Invalid baseline, reset to %ld\n",app->baseline);
	}
	if ( app->fmt.compare && (1 <<  app->baseline ) & app->testmask )
		TEST_FATAL("Baseline function masked %ld\n",app->baseline);

	/*
	 * Adjust the end pointer to start + incr * num_counts
	 * and find the num_counts (useful for sizing results
	 * and rand "decks") -- rounding down
	 */
	app->num_counts = 1 + (app->end - app->start) / app->incr;
	app->end = app->start + app->incr * ( app->num_counts - 1);

	/*
	 * Condense the opts checklist to a string
	 */
	for (i=0; i < UCHAR_MAX; i++, read_opt++) {
		if ( *read_opt ) {
			*(write_opt++) = (unsigned char) i;
		}
	}

	if ( !strlen(app->fmt.emit) ) {
		app->fmt.emit[0] = 'N';
		app->fmt.emit_num = 1;
	}

	app->fmt.data_cols =  app->fmt.opts['d'] ? 0 : 2;
	app->fmt.tbl_fmt = (	app->fmt.opts['c'] ? &g_table_fmt_csv	:
			   (	app->fmt.opts['h'] ? &g_table_fmt_html	:
				NULL ));
	
	app->useed = (unsigned int)app->seed;
	/*
	 * get the stats package to the same level of verbosity
	 */
	stats_set_verbose(app->verbose);
}

/**
 * \brief
 * Create a deck to adjust count in  buffer sequences. 
 * Card data stores a valid count value
 * \param app the application state
 * \return a pointer to the marked deck
 */
T_deck *test_app_count_deck_create(T_test_app *app)
{
	size_t i,j;
	/*
	 * Note that the app->useed is only used to create seeds
	 */
	unsigned int seed = rand_r(&app->useed);
	T_deck *deck = deck_create(app->num_counts,seed,NULL,0);
	for (i=0, j=app->start ; i < app->num_counts; i++ , j+= app->incr) {
		deck->cards[i].data = (void *)j;
	}
	return deck;
}

/*@}*/

/**
 * \defgroup test_comp Utility routines (cache, stride, alignment, and fill)
 * \ingroup test
 * @{
 */
/**
 * \brief
 * Compute a cacheline aligned stride given a size and an alignment
 * 
 * Note: the role of TEST_MAX_PREFETCH, is to ensure that extra prefetches
 * at the end of an optimized routine do \b not affect the results of the
 * next sample or test.
 *
 * \param size the size of the sample
 * \param align the the alignment off set the sample
 * \return cacheline aligned stride.
 */
size_t test_compute_cl_stride(size_t size, size_t align)
{
	size += align;
	size = size + TEST_CL_SIZE - (size % TEST_CL_SIZE);
	size += TEST_EXTRA_PREFETCH * TEST_CL_SIZE;
	return size;
}

/**
 * \brief
 * computes the buffer size from the strides and limits
 * \param min the minimum sample size
 * \param max the maximum sample size
 * \return the size of the buffer to create
 */
size_t test_compute_buffer_size(size_t min, size_t max)
{
	size_t bmin = min * TEST_BUFFER_REP_MAX;
	size_t bmax = max * TEST_BUFFER_REP_MIN;
	size_t size = TEST_CL_SIZE + (bmax > bmin) ? bmax : bmin;

	if ( size < TEST_MIN_BUFFER_SIZE )
		size = TEST_MIN_BUFFER_SIZE;
	else if ( size > TEST_MAX_BUFFER_SIZE )
		size = TEST_MAX_BUFFER_SIZE;

	return size;
}
	
/**
 * \brief
 * computes the next cacheline aligned address.  If address is cacheline
 * aligned, it is returned unaltered
 * \param addr address to align
 * \return cacheline aligned address
 */
char *test_compute_next_cl(char *addr)
{
	size_t off = ( (long)addr % TEST_CL_SIZE );
	if ( off )
		addr += TEST_CL_SIZE - off;
	return addr;
}


/**
 * \brief Fill a char buffer with a repeating pattern
 * \param buff char buffer of at least n bytes
 * \param n number bytes to fill
 * \param pat pattern to fill the buffer with (a NULL terminated string)
 */
void test_pat_fill(char *buff, size_t n, const char *pat)
{
	size_t i;
	size_t len = ( pat  ? fake_strlen(pat) : 0 );

	if ( !len ) {
		for ( i=0; i<n; i++ )
			buff[i] = 0;
	} else { 
		for ( i=0; i<n; i++ )
			buff[i] = pat[i%len];
	}
}

/*@}*/

/**
 * \defgroup test_buffer Test buffer creation and management functions
 * \ingroup test
 * @{
 */

/**
 * \brief Return the next alignment value for the buffer.  If alignment
 * randomization is enabled, this will vary, otherwise it will be constant.
 * \param b the 'this' buffer to affect
 * \return the appropriate alignment value
 */
size_t test_buffer_get_align(T_test_buffer *b)
{
	size_t align;
	if ( b->align_deck ) {
		T_deck_card *c = deck_deal(b->align_deck,0);
		align  = c->ord;
	} else
		align  = b->app->align;
	return align;
}

/**
 * \brief
 * Fills a toc for a given count and account for cacheline and align effects
 * \param b the 'this' buffer to affect
 * \param count the current constant for the toc
 */
void test_buffer_toc_const(T_test_buffer *b, size_t count)
{
	size_t i;
	T_test_sample *s;
	size_t stride = count * b->scale + b->pad;
	stride = test_compute_cl_stride(stride, b->max_align);

	b->toc_num = b->buf_size / stride;
	/*
	 * TODO: This should be an assert not a test
	 */
	if ( b->toc_num > b->toc_size ) 
		b->toc_num =  b->toc_size;

	for ( i=0; i < b->toc_num; i++ ) {
		s = &b->toc[i];

		s->align  = test_buffer_get_align(b);
		s->entry  = b->buf + i * stride + s->align;
		s->count  = count;
		s->stride = stride;
	}

	b->toc_type    = TOC_CONST;
	b->last_count  = count;
	b->toc_current = 0;

}

/**
 * \brief
 * Fills a toc for a given count and account for cacheline and align effects
 * \param b the 'this' buffer to affect
 * \param current_count the current constant count from the innerloop
 *	  (e.g loop_innerloop() )
 */
void test_buffer_toc_rand(T_test_buffer *b, size_t current_count)
{
	size_t i;
	T_test_sample *s;
	T_deck_card *c;

	char *p;
	size_t stride, count, total;
	size_t align  = b->app->align;

	/*
	 * Fit as many samples as possible, check for memory overrun below
	 */
	b->toc_num =  b->toc_size;

	p = b->buf;
	total = 0;
	for ( i=0; i < b->toc_num; i++ ) {
		/*
		 * We peek because we don't know if the next card won't
		 * "bust" the bufer, but we can't draw any cards we don't
		 * use.
		 */
		c = deck_peek(b->count_deck,0); /* 0 -> deck autoshuffles */
		count  = (size_t)c->data;

		stride = count * b->scale + b->pad;
		stride = test_compute_cl_stride(stride, b->max_align);

		if ( (total + stride) > b->buf_size ) {
			/* this one won't fit */
			b->toc_num = i;
			continue;
		}

		/*
		 * Deal to advance the deck, since we are using this card
		 */
		deck_deal(b->count_deck,0); /* 0 -> deck autoshuffles */

		s = &b->toc[i];
		s->align = test_buffer_get_align(b);
		s->entry = p + s->align;
		s->count = count;
		s->stride = stride;
		p += stride;
		total += stride;
	}

	b->toc_type    = TOC_VAR;
	/* this is the last time we set the toc, though it's not clear
	 * this is usefull to know in app->rand mode */
	b->last_count  = current_count;
	b->toc_current = 0;
}

/**
 * \brief
 * Fill a buffer with based on the toc entries and the fill callback. Since
 * both the _const and _rand toc initialization routines set all sample
 * fields this can be used in either case.
 * \param b the 'this' buffer to affect
 */
void test_buffer_fill_from_toc(T_test_buffer *b)
{
	size_t i;
	T_test_sample *s;
	if (!b->f_fill)
		return; /* some buffers don't need to be refilled */

	for ( i = 0; i < b->toc_num ; i++ ) {
		s = &b->toc[i];	
		b->f_fill(b,s,b->data_fill);
	}
}
/**
 * \brief
 * Standard callback to fill one sample with a zstring
 * \param b the 'this' buffer the sample is in.
 * \param s the sample to fill
 * \param data private data for this callback (unused)
 */
void test_buffer_cb_fill_zstr(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry, s->count, b->pat);
	s->entry[s->count]=0;
}
/**
 * \brief
 * Standard callback to fill one sample with a pattern (not NULL terminated)
 * \param b the 'this' buffer the sample is in.
 * \param s the sample to fill
 * \param data private data for this callback (unused)
 */
void test_buffer_cb_fill_mem(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry, s->count, b->pat);
}

/**
 * \brief
 * Create a single test buffer
 *
 * TODO: Needs a destructor
 * \param type buffer type 0 for src, 1 for dst, > 1 other (treated as dst)
 * \param app the application state
 * \param scale		the multiplier for count to compute buffer sample size
 * \param extra_pad	the fixed amount to add to each buffer size
 * \return test buffer structure
 */
T_test_buffer *test_buffer_create(size_t type, T_test_app *app, 
				 size_t scale, size_t extra_pad )
{
	size_t min_size = app->start;
	size_t max_size = app->end;
	T_test_buffer *buf = calloc(1,sizeof(T_test_buffer));

	/* Rather than make this a global, will keep a pointer to it */
	buf->app = app;

	/* Buffer information */
	/*
	 * Set up the scale and pad
	 */
	buf->type  = type;
	buf->scale = scale; /* I haven't found a use for this yet */
	buf->pad   = extra_pad + (TEST_IS_ZSTR(buf->app->type) ? 1 : 0);

	/*
	 * Here is the concatenatation support for DST buffers
	 * the pad is bumped to the worst case, which assumes the dst and
	 * src scales are the same.
	*/
	if (( type > TEST_SRC ) && TEST_IS_CAT(buf->app->type))
		buf->pad += max_size * scale;
	
	/* store the maximum alignment offset */
	buf->max_align = app->align;
	
	min_size   = buf->scale * min_size + buf->pad;
	max_size   = buf->scale * max_size + buf->pad;

	buf->max_stride = test_compute_cl_stride(max_size,buf->max_align);
	buf->min_stride = test_compute_cl_stride(min_size,buf->max_align);

	if ( type >= TEST_SRC  && !(TEST_IS_ONE_BUFFER(buf->app->type) ) ) {
		buf->min_stride = buf->max_stride;
	}

	buf->size = test_compute_buffer_size(buf->min_stride,buf->max_stride);
	buf->alloc = malloc(buf->size);

	/* find effective size of the buffer */
	buf->buf   = test_compute_next_cl(buf->alloc);
	buf->buf_size  = buf->size - (buf->buf - buf->alloc);
	buf->buf_size -= buf->buf_size % TEST_CL_SIZE;

	if ( type == TEST_SRC )
		buf->pat = TEST_PAT_SRC;
	else
		buf->pat = TEST_PAT_DST;

	/* Set up (but don't fill) the toc */
	buf->toc_size	= buf->buf_size / buf->min_stride;
	buf->toc	= malloc(buf->toc_size * sizeof(T_test_sample));
	buf->toc_type	= ( buf->app->rand ) ? TOC_VAR : TOC_CONST;
	buf->toc_num	= 0;
	buf->toc_current= 0;
	buf->last_count = buf->app->end + 1; /* force initialization */

	/*
	 * Each buffer needs a deck for random fills and or aligns
	 */
	if ( buf->app->rand ) {
		buf->count_deck = test_app_count_deck_create(buf->app);
#ifdef ZZZ_THINKS_OF_WHY_WAS_THIS_GOOD_IDEA
/*
 * At some point I added the following logic, but I'm quite unclear
 * as to the value.  The pad is bumped by max size above s.t. we
 * should always have sufficient destination space in the buffers
 * The though might have been to keep the dst fixed for testing
 * randomized "cat" operation
 */
		if ( buf->max_stride == buf->min_stride ) {
			size_t cn;
			T_deck *d = buf->count_deck;
			for (cn=0; cn < d->size; cn++ ) {
				d->cards[cn].data = (void *)buf->app->end;
			}
		}
#endif
	}
	if ( buf->app->rand_align ) {
		unsigned int seed = rand_r(&app->useed);
		buf->align_deck = deck_create(buf->max_align+1,seed,NULL,0);
	}

	/* Callback information */
	/*
	 * This are the default actions that likely do the right thing
	 * for these test types. This can be overridden to change
	 * the behavior as needed
	 */
	if ( TEST_IS_ZSTR(buf->app->type) )
		buf->f_fill = test_buffer_cb_fill_zstr;
	else if ( TEST_IS_MEM(buf->app->type) )
		buf->f_fill = test_buffer_cb_fill_mem;

	/* load the "base class" wrap and fill functions */
	buf->f_count = test_buffer_count_change;
	buf->f_wrap = test_buffer_wrap;
	return buf;
}

/**
 *  Flush all the cachelines in the buffer
 *  @param b the 'this' buffer to affect
 */
void test_buffer_flush(T_test_buffer *b) {
	size_t cl_count = b->buf_size / TEST_CL_SIZE;
	size_t cl;
	void *p = b->buf;
	for (cl = 0; cl < cl_count; cl++, p += TEST_CL_SIZE ) {
		__asm__ __volatile__ ( 
			"clflush (%0)"
			:
			: "a" (p)
		);
	}
}


/**
 * \brief
 * Default action to take on a buffer "count change" event, can be overriden
 * by setting T_test_buffer::f_count
 *
 * Sets the toc for the current count if it has changed.
 * Also responsible * for initialization (as count change happens in timetest().
 * Override this function by defining T_test_buffer::f_count
 * \param b the 'this' buffer to affect
 * \param count the current count
 * \param data arbitrary private for the count change (ignored)
 */
void test_buffer_count_change(T_test_buffer *b, size_t count, void *data)
{
	if ( !b->toc_num ) {
		/* Uninitialized buffer */
		if (!b->app->rand)
			test_buffer_toc_const(b, count);
		else
			test_buffer_toc_rand(b,count);
		test_buffer_fill_from_toc(b);
#ifdef TEST_FORCE_FLUSH
		test_buffer_flush(b);
#endif
		
	} else if ( !b->app->rand && (count != b->last_count) ) {
		test_buffer_toc_const(b, count);
		test_buffer_fill_from_toc(b);
#ifdef TEST_FORCE_FLUSH
		test_buffer_flush(b);
#endif
	}
}

/**
 * \brief
 * Default action to take on a buffer "wrap" event. Can be overridden by
 * setting T_test_buffer::f_wrap
 *
 * Fills the toc with a new random distribution if app->rand.
 * Note that if f_fill is NULL not actually fill anything.
 * Override this function by defining T_test_buffer::f_wrap
 * \param b the 'this' buffer to affect
 * \param count the current count
 * \param data arbitrary private for the wrap (ignored)
 */
void test_buffer_wrap(T_test_buffer *b, size_t count, void *data)
{
	if ( b->app->rand ) {
		test_buffer_toc_rand(b,count);
	} 
	test_buffer_fill_from_toc(b);
#ifdef TEST_FORCE_FLUSH
	test_buffer_flush(b);
#endif
	b->toc_current = 0;
}

/**
 * \brief
 * Get the next sample from the buffer, handling the count change and
 * buffer wrap cases.
 * \param count the current count (ignored for rand)
 * \param b	the buffer to get the next sample from
 * \return pointer to the next sample in the the buffer
 */
T_test_sample *test_buffer_next(T_test_buffer *b, size_t count)
{
	T_test_sample *sample;

	if ( b->last_count != count && b->f_count ) {
		b->f_count(b, count, b->data_count);
	}

	if ( b->toc_current >= b->toc_num && b->f_wrap) {
		b->f_wrap(b,count, b->data_wrap);
	}

	sample = &b->toc[b->toc_current++];
	return sample;
}

/*@}*/
/* end test_utils.c */
