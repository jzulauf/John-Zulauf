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

#define RET_TYPE  char *

/* Define this to match the prototype */
RET_TYPE do_func( T_def_Fn f, const size_t count, void *sp, void *dp)
{
	RET_TYPE r;
	r = (f)((char *)dp+count,(char *)(sp)+count);
	return r;
}

int do_test_return(RET_TYPE ret, size_t count, const void *psrc, const void *pdst)
{
	int result = 0;
	if ( ret != ( (char *)pdst+count) )  {
		result |= RET_RET_DIFFERS;
	}
	return result;
}

int do_test_buffers(size_t count, const void *psrc, const void *pdst,
		const char *sref, const char *dref)
{
	int result = 0;
	const char *src = psrc;
	const char *dst = pdst;

	if  (memcmp(sref,src,3*count) != 0 ) {
		result |= RET_SRC_DIFFERS;
	};
	if ( ( 0 != memcmp(dref,dst,count) ) ||
	     ( 0 != memcmp(sref+count,dst+count,count+1) ) ||
	     ( 0 != memcmp(dref+2*count+1,pdst+2*count+1,count-1) ) ) {
		result |= RET_DST_DIFFERS;
	}
	return result;
}
		
void create_reference(const size_t count, const void *psrc, const void *pdst,
		char *sref, char *dref)
{
	memcpy(sref,psrc,3*count);
	memcpy(dref,pdst,3*count);
	return;
}

void contest(T_test_app *app)
{
	size_t test, test_count;
	int result;
	RET_TYPE ret;

	T_test_sample *samp;

	/* Variables set by DEF_NEXT */
	size_t count; 
	T_test_sample *src;
	void *psrc;

#ifndef DEF_NO_DECLARE_DST
	T_test_sample *dst;
	void *pdst;
#endif
	char sref[(3*BIG)+1];
	char dref[(3*BIG)+1];

	for ( test_count=START;  test_count <= BIG; test_count += STEP ) {
		for (test=0;test<NUM_TEST; test++) {
			T_def_Fn   f = def_tests[test].f;
			DEF_NEXT(app,test_count,count,src,psrc,dst,pdst);
			//if (app->verbose > 0 ) {
			//	printf("C%ld ", count);
			//	printf("S%ld ", strlen(_ptr+count));
			//}
			create_reference(count,psrc,pdst,sref,dref);
			/* torture test... setd and see if anything breaks*/
			asm ( "std;" );
			ret = do_func(f,count,psrc,pdst);
			result	 = do_test_return(ret,count,psrc,pdst);
			result	|= do_test_buffers(count,psrc,pdst,sref,dref);
			if (result)
				printf("test=%s count=%d result=%d\n",
					def_tests[test].name,count,result);
		}
	}
}

void utest_buffer_cb_fill_zstr(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry,s->stride,b->pat);
	s->entry[2*s->count] = 0;
}
	
void utest_buffer_cb_fill_zstr_dst(T_test_buffer *b, T_test_sample *s, void *data)
{
	test_pat_fill(s->entry,s->stride,b->pat);
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
	test_create_buffers(app,3,0);

	/* override the fill function */
	app->bufs[TEST_SRC]->f_fill = utest_buffer_cb_fill_zstr;
	app->bufs[TEST_DST]->f_fill = utest_buffer_cb_fill_zstr_dst;
	/* override the toc_size so constant stride testing isn't slow */
	if ( !app->rand )
		app->bufs[TEST_SRC]->toc_size = NUM_TEST + 1;

	contest(app);

	exit(0);
}
