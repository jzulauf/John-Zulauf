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
#include <string.h>

/** \def gives test it's own copy of the test list */
#define def_tests udef_tests
#include "def_test.h"

#include "test_utils.h"

#define BIG  270
#define START  4
#define STEP 1
#define BIGGER TEST_BUFFER_SIZE

#define RET_SRC_DIFFERS	1
#define RET_DST_DIFFERS	2
#define RET_RET_DIFFERS	4


//#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) 
//#define DEF_RUN(f,c,ps,pd) f(ps,pd,c)

/* Define this to match the prototype */
/*#define DO_FUNC(f,count,ptr) (f)(ptr+count) */
#define DO_TEST_RET(ret,expect) (			\
	(						\
		( (expect >  0) && (ret >  0 ) ) ||	\
		( (expect == 0) && (ret == 0 ) ) ||	\
		( (expect <  0) && (ret <  0 ) ) 	\
	) ? 0 : RET_RET_DIFFERS ) 

#define DO_TEST_BUFFERS(count,ptr) 0
#define RET_TYPE size_t
		

int result_expect = 0 ;
void contest(T_test_app *app)
{
	size_t lcount,test;
	int result;
	RET_TYPE ret;

	T_test_sample *samp;
	char *samp_ptr;
	size_t samp_count;

	/* Variables set by DEF_NEXT */
	size_t count; 
	T_test_sample *src;
	void *psrc;
	T_test_sample *dst;
	void *pdst;


	for ( lcount=app->start;  lcount <= app->end; lcount += app->incr ) {
		for (test=0;test<NUM_TEST; test++) {
			DEF_NEXT_BOTH_DEF(app,lcount,count,src,psrc,dst,pdst);
			if (app->verbose > 0 ) {
				printf("C%ld ", count);
			}
			ret = DEF_RUN(def_tests[test].f,count,psrc,pdst);
			result	 = DO_TEST_RET(ret,strcmp(pdst,psrc));
			if (result)
				printf("test=%s count=%d result=%d\n",
					def_tests[test].name,count,result);
		}
	}
}

void utest_src_fill(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry,s->stride,b->pat);
	s->entry[s->count-1] = 0;
}
	
void utest_mismatch_fill(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry,s->stride,b->pat);
	s->entry[s->count-1] = 0;
	if ( data ) {
		s->entry[s->count/(long)data] = 0xFF;
	}
	
}
	

int main(int argc, char **argv) {
	T_test_app *app;
	int c;
	char progname[80]= "utest -- ";
	char progversion[80]= "1.0";
	
	if ( strlen(PROG_DIR) < sizeof(progname)-strlen(progname) );
		strcat(progname,PROG_DIR);
	
	app = test_init(progname, progversion, MEM_TO_MEM);

	/* set up the utest defaults */
	app->start = START;
	app->end = BIG;
	app->incr = STEP;
	app->num_try=1;

	while ((c = opts_getopt (argc, argv)) != -1) {
		switch (c) {
		/*add stuff opts_utils can't handle * /
		case 'Z':
			Z_handler();
			break;
		/* end of stuff opts_utils can't handle */
		default:
			opts_usage();
		}
	}

	test_app_validate_options(app);
	test_app_print_options(app);
	/* triple the scale to fit the overrun/underrun detection areas */
	test_create_buffers(app,1,0);

	/* override the fill function */
	app->bufs[TEST_SRC]->f_fill = utest_src_fill;
	app->bufs[TEST_DST]->f_fill = utest_src_fill;
	app->bufs[TEST_DST]->pat = TEST_PAT_SRC;
	/* override the toc_size so constant stride testing isn't slow */
	if ( !app->rand )
		app->bufs[TEST_SRC]->toc_size = NUM_TEST + 1;

	contest(app);
	app->bufs[TEST_DST]->f_fill = utest_mismatch_fill;
	app->bufs[TEST_DST]->last_count = app->end + 1;
	
	result_expect = 1 ;
	contest(app);
	app->bufs[TEST_DST]->data_fill = (void*)2;
	app->bufs[TEST_DST]->last_count = app->end + 1;
	result_expect = -1 ;
	contest(app);

	exit(0);
}
