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
//#include "copies.h"

#define BIG  270
#define START  4
#define STEP 1
#define BIGGER 256*1024
char src_buffer[BIGGER];
char ref_src_buffer[BIGGER];
char dst_buffer[BIGGER];
char ref_dst_buffer[BIGGER];
char src_pat[]="1234567890";
char dst_pat[]="!@#$%^&*()";
#define FILL_CHAR 'A'
#define RET_SRC_DIFFERS	1
#define RET_DST_DIFFERS	2

/** this define gives test it's own copy of the test list */
#define def_tests udef_tests
#include "def_test.h"

#define DO_FUNC(f,n) (f)(dst_buffer+n,FILL_CHAR,n)
/*
 * NOTE: I'm intentionally NOT using the libc functions as I need
 * to use an INDEPENDENT implementation to test the system implementation
 * which I may be changing in the course of this testing
 */

/*
 * Fill a buffer with a repeating pattern
 * Pat is a NULL terminated string
 */

void pat_fill(char *buff, size_t n, char *pat)
{
	size_t i;
	size_t len = ( pat  ? strlen(pat) : 0 );

	if ( !len ) {
		bzero(buff,n);
	} else { 
		for ( i=0; i<n; i++ )
			buff[i] = pat[i%len];
	}
}

int buffer_compare(char *a, char *b, size_t n) {
	int ret=0;	
	size_t i;
	for (i=0; i<n && !ret; i++ ) 
		ret = a[i] != b[i];

	return ret;
}
		

/*
 * Fill the buffers the way that memset should with "margin" checks
 */
void init_buffers(const size_t n)
{
	size_t count;
	char fill[2] = { FILL_CHAR , 0 };

	pat_fill(dst_buffer,3*n,dst_pat);
	pat_fill(ref_dst_buffer,3*n,dst_pat);
	pat_fill(ref_dst_buffer+n,n,fill);

}

int test_buffers(const size_t n)
{
	int result=0;
	if (buffer_compare(dst_buffer, ref_dst_buffer,3*n) )
		result += RET_DST_DIFFERS;
	return result;
}
	


void contest(void)
{
	size_t count;
	size_t test;
	int result;
	void *ret;


	

	for (test=0;test<NUM_TEST; test++) {
		T_def_Fn test_f = udef_tests[test].f;
		for ( count=START;  count <= BIG; count += STEP ) {
			init_buffers(count);
			ret = DO_FUNC(test_f,count);
			result =test_buffers(count);
			if ( ret != (void *)(dst_buffer+count) )	
				result |= 0x4;
			if (result)
				printf("test=%s count=%d result=%d\n",
					udef_tests[test].name,count,result);
		}
	}
}

int main(int argc, char **argv) {
	contest();
}
