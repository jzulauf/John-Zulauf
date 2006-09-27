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

int main(int argc, char **argv) {
	T_test_app *app;
	int c;
	char progname[80]= "ptest -- ";
	char progversion[80]= "1.0";
	
	if ( strlen(PROG_DIR) < sizeof(progname)-strlen(progname) ) {
		strcat(progname,PROG_DIR);
	}
	
	app = test_init(progname, progversion, MEM);

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

	loop_test(app);

	exit(0);
}

