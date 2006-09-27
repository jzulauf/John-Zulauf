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
#include "permuted_proto.h"

#define BIGGER 256*1024
char src_buffer[BIGGER];
char ref_src_buffer[BIGGER];
char dst_buffer[BIGGER];
char ref_dst_buffer[BIGGER];

void init_buffers(const size_t n)
{
	size_t count;
#define MOD 10
	char srclist[]="1234567890";
	char dstlist[]="!@#$%^&*()";
	
	for ( count=0; count < n; count++ ) {
		src_buffer[count] = srclist[count%MOD];
		ref_src_buffer[count] = src_buffer[count];
		dst_buffer[count] = dstlist[count%MOD];
		ref_dst_buffer[count] = dst_buffer[count];
	}
	for ( count=n; count < 2*n; count++ ) {
		src_buffer[count] = 'A' + (count-n)%26;
		ref_src_buffer[count] = src_buffer[count];
		dst_buffer[count] = dstlist[count%MOD];
		ref_dst_buffer[count] = src_buffer[count];
	}
	for ( count=2*n; count < 3*n; count++ ) {
		src_buffer[count] = srclist[count%MOD];
		ref_src_buffer[count] = src_buffer[count];
		dst_buffer[count] = dstlist[count%MOD];
		ref_dst_buffer[count] = dst_buffer[count];
	}
		
}

int test_buffers(const size_t n)
{
	int result=0;
	if (memcmp(src_buffer, ref_src_buffer,3*n) )
		result +=1;
	if (memcmp(dst_buffer, ref_dst_buffer,3*n) )
		result +=2;
	return result;
}
	
#define def_tests udef_tests
#include "def_test.h"

#define BIG  270
#define START  4
#define STEP 1
void contest(void)
{
	size_t count;
	size_t test;
	int result;
	void *ret;

	

	for (test=0;test<NUM_TEST; test++) {
		T_def_Fn copy = def_tests[test].f;
		for ( count=START;  count <= BIG; count += STEP ) {
			init_buffers(count);
			ret = copy(dst_buffer+count,src_buffer+count,count);
			result =test_buffers(count);
			if ( ret != (void *)(dst_buffer+count) )	
				result |= 0x4;
			if (result)
				printf("test=%d count=%d result=%d\n",
					test,count,result);
		}
	}
}

int main(int argc, char **argv) {
	contest();
}
