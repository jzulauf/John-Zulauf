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
 * Implements a small subset of the glibc "fake_lib"
 *
 */

#include <stdlib.h>
#include "fake_lib.h"

/**
 * \defgroup fake_lib "fake_" Fake Libc
 * \brief
 * Implements a subset of libc.  This is usefull when one needs to use an
 * INDEPENDENT implementation to either to test the system implementation
 * which may be changing in the course of this testing of function when the
 * system implementation is being worked on
 *
 */

/*@{*/

/**
 * \brief
 * Counts the number of bytes in a NULL terminated string (excluding the NULL)
 * \param c the string to count
 * \return the length of the string
 */
size_t fake_strlen(const char *c)
{
	size_t l = 0;
	while ( *c++ )
		l++;
	return l;
}

/**
 * \brief
 * fill memory with a constant byte
 * \param s  the memory to fill
 * \param c  the byte to fill it with
 * \param n  the number of bytes to fill
 * \return the pointer to memory area s
 */
void *fake_memset(void *s, int c, size_t n)
{
	unsigned char *m = s;
	unsigned char  f = (unsigned char)c;
	size_t i;
	for ( i=0; i < n; i++ )
		m[i]=f;

	return s;
}

/**
 * \brief
 * sets the first n bytes of the byte area starting at s to zero.
 * \param s  the memory to fill
 * \param n  the number of bytes to fill
 */
void fake_bzero(void *s, size_t n)
{
	fake_memset(s,0,n);
}

/**
 * \brief
 * function compares the first n bytes of the memory areas 'a'
 * and 'b'.  It returns an integer less than, equal  to,  or  greater  than
 * zero  if  'a'  is  found, respectively, to be less than, to match, or be
 * greater than 'b'. 
 * \param a first buffer to compare
 * \param b second buffer to compare
 * \param n number of bytes to compare
 * \return -1 if a < b , 0 if a == b, and 1 if a > b  
 */
int fake_memcmp(const char *a, const char *b, size_t n)
{
	int ret=0;	
	size_t i;
	for (i=0; i<n && !ret; i++ )  {
		if ( a[i] >  b[i] )
			ret = 1;
		else if ( a[i] <  b[i] )
			ret = -1;
	}

	return ret;
}

/*@}*/

