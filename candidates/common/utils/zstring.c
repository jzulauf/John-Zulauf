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
 * Implements a simple string API \ref zstring
 */
#include "zstring.h"
#include <stdlib.h>
/**
 * \defgroup zstring "zstring_" Simple string API
 * \brief
 * A simple string API with class-like semantics
 */

/*@{*/

/**
 * Creator for T_zstring
 *
 * @param s initial string value, NULL for empty string ""
 * @return the created T_zstring
 */

T_zstring *zstring_create(const char *s)
{
	T_zstring *z = malloc(sizeof(T_zstring));
	z->str = calloc(1,sizeof(char));
	z->len = 0;
	zstring_set(z,s);
	return z;
}

/**
 * Destroyer for T_zstring
 *
 * @param z	the T_zstring to destroy
 */

void zstring_destroy(T_zstring *z) 
{
	free(z->str);
	free(z);
}

/**
 * Set the T_zstring to a copy of the passed string
 * @note resets buffer length if > 2x strlen, invalidating the
 *       last zstring_alloc()
 * @param z	the T_zstring to set
 * @param s_arg the string to copy, or NULL for ""
 */
void zstring_set(T_zstring *z, const char *s_arg)
{
	const char *s = ( NULL != s_arg ) ? s_arg : "";
	size_t slen = strlen(s);
	if ( (slen > z->len ) || (slen < (z->len >> 2) ) ){
		z->str = realloc(z->str, slen + 1);
		z->len = slen;
	}

	strcpy(z->str,s);
	return;
}

/**
 * The accessor for the string value
 *
 * @param z	the T_zstring to access
 * @return the string value
 */
const char * zstring_get(T_zstring *z)
{
	return z->str;
}

/**
 * concatenate the passed string to the T_zstring
 *
 * @param z	the T_zstring to add to
 * @param s_arg the string to add, or NULL for ""
 */
void zstring_cat(T_zstring *z, const char *s_arg)
{
	const char *s = ( NULL != s_arg ) ? s_arg : "";
	size_t new_len = strlen(s) + strlen(z->str);
	if ( new_len > z->len ){
		z->str = realloc(z->str, new_len + 1);
		z->len = new_len;
	}

	strcat(z->str,s);
	return;
}

/** ensure that the string buffer is at least len+1 
 * @note zstring_set() is free to resize the buffer if 
 *       it is much larger strlen, which will invalidate the zstring_alloc()
 * @param z	the T_zstring to add to
 * @param len	the minimum required length, or 0 to get the current length
 * @return the max strlen which will fit into the currently allocated str buffer
 */
size_t zstring_alloc(T_zstring *z, size_t len )
{
	if ( len > z->len ) {
		z->str = realloc(z->str, len + 1);
		z->len = len;
	}
	return z->len;
}

/** pad the string with the given number of the passed character
 * @param z     the T_zstring to pad
 * @param c	the pad character (cast to unsigned char)
 * @param len   the number of c characters to append to z
*/
void zstring_pad(T_zstring *z, int c, size_t len )
{
	unsigned char pad = (unsigned char)c;
	size_t slen = strlen(z->str);
	size_t padded_len = slen+len;
	size_t i;
	/* use the allocator to ensure the string is big enough */
	zstring_alloc(z,padded_len);
	for (i=slen;i<padded_len;i++) {
		z->str[i] = pad;
	}
	z->str[padded_len] = '\0';
}

/*@}*/
