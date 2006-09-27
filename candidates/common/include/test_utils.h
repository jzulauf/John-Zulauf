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
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_
#include <limits.h>
#include "deck.h"
#include "table.h"

/**
 * \file
 * \brief types and prototypes for \ref test
 */

/**
 * \brief
 * The overall type of the test.  Defines default behavior for the buffers
 */
typedef enum {
	MEM,		/*!< one buffer memory operation */
	MEM_TO_MEM,	/*!< two buffer memory operation dst is size of src*/
	ZSTR,		/*!< one buffer string operation */
	ZSTR_TO_ZSTR,	/*!< two buffer string operation dst is size of src*/
	ZSTR_CAT_ZSTR,	/*!< two buffer string operation w/ lengths add */

	CUSTOM_ONE,	/*!< non-std, one buffer, no default fill action */
	CUSTOM_MORE,	/*!< non-std, >1 buffer, no default fill action */
	CUSTOM_CAT,	/*!< non-std, >1 buffer, non-src buffers are 2x size,
				no default fill action */
} T_test_type;

/**
 * test if test type only uses one buffer
 */
#define TEST_IS_ONE_BUFFER(t) ( \
	( ( MEM        == t ) ||	\
	  ( ZSTR       == t ) ||	\
	  ( CUSTOM_ONE == t ) ) ? 1 : 0 )

/**
 * test if test type uses null terminate strings
 */
#define TEST_IS_ZSTR(t) ( \
	( ( ZSTR          == t ) ||	\
	  ( ZSTR_TO_ZSTR  == t ) ||	\
	  ( ZSTR_CAT_ZSTR == t ) ) ? 1 : 0 )

/**
 * test if test type use memory (i.e. not strings)
 */
#define TEST_IS_MEM(t) ( \
	( ( MEM        == t ) ||	\
	  ( MEM_TO_MEM == t ) ) ? 1 : 0 )

/**
 * test if test type appends source to dest (i.e. length is additive)
 */
#define TEST_IS_CAT(t) ( \
	( ( ZSTR_CAT_ZSTR == t ) ||	\
	  ( CUSTOM_CAT    == t ) ) ? 1 : 0 )

/* forward declaration */
struct T_test_buffer;

/** The maximum number of tables to emit */
#define TEST_APP_TABLE_MAX 5
/**
 * The format for the test report
 */
typedef struct {
	int compare;		/*!< does the output do a compare */
	char opts[UCHAR_MAX]; /*!< a checklist of the format flags set */
	char opts_zstr[UCHAR_MAX+1]; /*!< opts as a NULL terminated str */
	char emit_num;		/*!< the number of tables to produce */
	char emit[TEST_APP_TABLE_MAX+1];/*!< a string representing the tables to emit */
	size_t data_cols; /*!< the number of data columns to reserve */
	T_table_fmt *tbl_fmt; /*!< the format for the table */
} T_test_app_fmt;
/**
 * \brief
 * Stores the standard options and setting for a test program
 */
typedef struct {
	T_test_type type;	/*!< type of operation tested */
	
	const char *name;	/*!< program name */
	const char *vers;	/*!< program name */
	long start;	/*!< lowest count to test (inclusive) */
	long end;	/*!< highest count to test (inclusive) */
	long incr;	/*!< step size for between tests */
	long num_try;	/*!< number of attemps for each count and function */
	long align;	/*!< alignment for test */
	long rand_align;/*!< randomize alignment for test */
	long verbose;	/*!< level of detail for output */
	long baseline;	/*!< test number to use as the comparison basis */
	long seed;	/*!< seed for random walk */
	long rand;	/*!< random walk the count */
	long testmask;	/*!< bitmask of tests to \b skip */
	T_test_app_fmt fmt; /*!< the format for the test report */
	void *test_data;/*!< test specific data */
	long clock;	/*!< the clock speed of the host in MHz */
	double bandwidth_scale; /*!< the total IO for each byte in count */
	
	
	/*
	 * These are valued computed/allocated base on user inputs
	 */
	unsigned int useed;	/*!<The persistant state for rand_r for this
				 * app, initialized to T_test_app::seed */
	size_t num_counts;	/*!<The number of counts we test at */
	struct T_test_buffer **bufs; /*!< the test buffers */
} T_test_app;


#ifndef TEST_BUFFER_MAX
/**
 * The number of buffers in the test.  Default is 2 for src and dst
 */
#	define TEST_BUFFER_MAX 2
#endif

#ifndef TEST_PAT_SRC
/** The default data pattern for source buffers */
#	define TEST_PAT_SRC "ABCDEFGHIJKLMNOP"
#endif
#ifndef TEST_PAT_DST
/** The default data pattern for destination buffers */
#	define TEST_PAT_DST "abcdefghijklmnop"
#endif

#ifndef TEST_PAT_PAD
/** The default data pattern for padding that \b should be found */
#	define TEST_PAT_PAD "0123456789:;<=>?"
#endif

#ifndef TEST_PAT_BAD
/** The default data pattern for padding that \b should'nt be found */
#	define TEST_PAT_BAD " !\"#$%&'()*+`-./"
#endif

#ifndef TEST_CL_SIZE
/** The cacheline size */
#	define TEST_CL_SIZE 32
#endif

/**
 * \def TEST_EXTRA_PREFETCH 
 * \brief sets the maximum number of extra cachlines prefetched, w.r.t. the
 * last cl processed by a test_function.  Three is default as this is the
 * number of prefetch slots on a Geode LX processor
 */
#ifndef TEST_EXTRA_PREFETCH
/** How far ahead do the test functions prefetch (in TEST_CL_SIZE) */
#	define TEST_EXTRA_PREFETCH 3 
#endif

#ifndef TEST_ALIGN_SIZE
/** maximum shift of the source/dest start points for alingment testing (+1) */
#	define TEST_ALIGN_SIZE TEST_CL_SIZE
#endif

#ifndef TEST_MIN_BUFFER_SIZE
/** the smallest the test buffers can be.  
 * Should be 4x at least 2x the cachesize
 */
#	define TEST_MIN_BUFFER_SIZE 256*1024
#endif

#ifndef TEST_MAX_BUFFER_SIZE
/** the largest the test buffers can be.  Should be << system memory.  */
#	define TEST_MAX_BUFFER_SIZE 1024*1024
#endif

#ifndef TEST_BUFFER_REP_MIN
/** The fewest number of test sample to fit in a buffer */
#	define TEST_BUFFER_REP_MIN 8
#endif

#ifndef TEST_BUFFER_REP_MAX
/** The greatest number of test sample to fit in a buffer */
#	define TEST_BUFFER_REP_MAX 512
#endif

/**
 * \brief
 * This is a data sample that a test function works on.
 */
typedef struct {
	char *entry; /*!<pointer to a sample in a test buffer */
	size_t count;	/*!<the count associated with this entry */
	size_t stride; /*!<the stride of this entry */
	size_t align; /*!<the alignment of this entry */
} T_test_sample;


/**
 * what type of count is used in the toc's samples
 */
typedef enum { TOC_CONST, TOC_VAR }  T_test_toc_type;

/** callback function type for wrap and count change functions */
typedef void (*T_test_cb_samp)(struct T_test_buffer *, size_t, void *);

/** callback function type for sample fill functions */
typedef void (*T_test_cb_fill)(struct T_test_buffer *, T_test_sample *, void *);

/**
 * \brief
 * The data and callbacks for a given test buffer
 */
typedef struct T_test_buffer {
	T_test_app *app;	/*!< pointer to the application state
				     the test_buffer needs this information
				     probably a bit too much, but don't
				     know how to avoid it */
	/* buffer type information */
	size_t type;		/*!< type (index) of the buffer) */
	size_t scale;		/*!< size of sample vs. count */
	size_t pad;		/*!< additional fixed pad */
	size_t min_stride;	/*!< the smallest sample can be */
	size_t max_stride;	/*!< the largest a sample can be */
	size_t max_align;	/*!< the largest the alignment offset can be */

	/* buffer information */
	size_t size;		/*!< size allocated for buffer */
	char *alloc;		/*!< the malloc'd memory for the buffer */
	size_t buf_size;	/*!< cacheline aligned size buffer */
	char *buf;		/*!< the first full cacheline of alloc */
	const char *pat;	/*!< Fill pattern for for buffer */
	T_deck *count_deck;	/*!< The valid count deck for randomization */
	T_deck *align_deck;	/*!< The valid count deck for randomization */

	/* toc information */
	size_t last_count;	/*!< the count used for the last fill */
	size_t toc_size;	/*!< number of samples in the toc */
	T_test_sample *toc;	/*!< the table of contents to the samples */
	T_test_toc_type toc_type;	/*!< enum CONSTANT count or VARIABLE */
	size_t toc_num;		/*!< number of filled entries in the toc */
	size_t toc_current;	/*!< next sample in toc to return */

	/* callback information */
	T_test_cb_samp f_wrap;	/*!< Buffer Wrap function */
	void *data_wrap;	/*!< Data passed to f_wrap */
	T_test_cb_samp f_count;	/*!< Buffer Count change function */
	void *data_count;	/*!< Data passed to f_count */
	T_test_cb_fill f_fill;	/*!< Buffer fill function */
	void *data_fill;	/*!< Data passed to f_fill */
} T_test_buffer;


/** The default index for the source buffer */
#define TEST_SRC 0
/** The default index for the destination buffer */
#define TEST_DST 1

/* Default setup API */
T_test_app *test_init(const char *progname, const char *progversion,
				 T_test_type type);
void test_create_buffers(T_test_app *app, size_t scale, size_t extra);

int test_action_oneshot(void *ptr_to_app_opts);

/* The application state "methods" */
T_test_app *test_app_create(const char *progname, const char *progversion,
				 T_test_type type);
void test_app_default_opts(T_test_app *app);
void test_app_print_options(T_test_app *app);
void test_app_validate_options(T_test_app *app);
size_t test_app_fill_data(T_test_app *app, T_table *t, size_t row, size_t col);

/* Common logic for computing sizes and strides */
size_t test_compute_cl_stride(size_t size, size_t align);
size_t test_compute_buffer_size(size_t min, size_t max);
char *test_compute_next_cl(char *addr);

/* Toc creators and fillers */
void test_buffer_toc_const(T_test_buffer *b, size_t count);
void test_buffer_toc_rand(T_test_buffer *b, size_t current_count);
void test_buffer_fill_from_toc(T_test_buffer *b);
void test_buffer_cb_fill_zstr(T_test_buffer *b, T_test_sample *s, void *data);
void test_buffer_cb_fill_mem(T_test_buffer *b, T_test_sample *s, void *);

/* Buffer structure manipulators*/
T_test_buffer *test_buffer_create(size_t type, T_test_app *app,
                                 size_t scale, size_t extra_pad );
void test_buffer_count_change(T_test_buffer *b, size_t count, void *data);
void test_buffer_wrap(T_test_buffer *b, size_t count, void *data);
T_test_sample *test_buffer_next(T_test_buffer *b, size_t count);


/* Various utilities */
void test_pat_fill(char *buff, size_t n, const char *pat);

#endif

