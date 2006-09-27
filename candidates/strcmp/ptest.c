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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bucket_stats.h"
#include "opts_utils.h"
#include "test_utils.h"
#include "loop_test.h"
#include "zstring.h"


/** state to control filling the destination buffer
 * to support toggling of buffer mismatch */
typedef struct {
	T_test_cb_fill base_fill; /*!<the default fill */
	void *base_data;	  /*!<the state for the default fill */
	long mismatch;		  /*!<whether the buffer should differ */
} T_dst_fill_state;

/**
 * fills the destination buffer with alternate fills mismatching
 * @param b	the test buffer (the dest buffer)
 * @param s	the sample we are filling
 * @param data	the state data for filling the sample
 */
void strcmp_dest_fill(T_test_buffer *b, T_test_sample *s, void *data)
{
	T_dst_fill_state *st = (T_dst_fill_state *)data;

        st->base_fill(b,s,st->base_data);

        if ( s->count && st->mismatch ) {
                s->entry[s->count-1] = 0xFF;
        }

	st->mismatch = !st->mismatch;

}

int main(int argc, char **argv) {
	T_test_app *app;
	int c;
	T_zstring *progname = zstring_create("ptest -- ");
	char progversion[80]= "1.0";
	T_dst_fill_state dest_fill_state;
	
	zstring_cat(progname,PROG_DIR);
	
	app = test_init(zstring_get(progname), progversion, ZSTR_TO_ZSTR);

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
	test_create_buffers(app,1,0);

	/* We need some buffer magic to create traffic on the mismatch
 	 * path.  So... we'll override the fill function for the dest buffer
	 */
	dest_fill_state.base_fill =  app->bufs[TEST_DST]->f_fill; 
	dest_fill_state.base_data =  app->bufs[TEST_DST]->data_fill; 
	dest_fill_state.mismatch  =  0;

	app->bufs[TEST_DST]->pat = TEST_PAT_SRC;
	app->bufs[TEST_DST]->f_fill = strcmp_dest_fill;
	app->bufs[TEST_DST]->data_fill = &dest_fill_state;

	loop_test(app);
	zstring_destroy(progname);
	exit(0);
}

