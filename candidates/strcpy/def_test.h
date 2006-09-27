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
#ifndef _DEF_TEST_H_
#define _DEF_TEST_H_
/**
 * \file strcpy/def_test.h
 * \brief
 * This file defines the functions types and names for \ref strcpy_test
 */
/**
 * \defgroup strcpy_test strcpy performance test
 * \brief
 * The performance and optimzation test for strcpy.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * \section strcpy_status Status
 *
 * The testing and optimization has been completed
 *
 * \section strcpy_results Results
 *
 *	- Winner: lx_strcpy_r3() 1.65x performance gain on LX 
 *	- Honorable mention: lx_strcpy_5a() 1.2x peformance gain on K7
 *
 * \section strcpy_candidates Candidates 
 *	- \ref lx_strcpy_r3()
 *	- \ref lx_strcpy_r4()
 *	- \ref lx_strcpy_5a()
 *	- \ref lx_strcpy_5a_r()
 *
 * \section strcpy_notes Notes
 * Reference is made to the 'Darwin "magic" algorithm.' By this we are
 * referring to the dword zero find algorithm which uses the 0xfefefeff
 * "magic" number.  This is similar to (and mathematically equivalent to)
 * the i586 implementation but is implemented  in fewer instructions and
 * fewer conditional jumps. 
 *
 * Note also that as the Darwin code was PPC assembly, thus only the
 * algorithm and not the implementation is reflected in the
 * implementation here.
 */
 
#include <string.h>
#include "def_test_def.h"

/** Strcpy implementation using 
 * - the 586 alignment logic
 * - Darwin "magic" algorithm
 * */
char *lx_strcpy_5a(char *dst, const char *src);
/** Strcpy implementation using 
 * - the 586 alignment logic
 * - Darwin "magic" algorithm
 * - Unrolled last dword logic
 * */
char *lx_strcpy_5a_r(char *dst, const char *src);
/** Strcpy implementation using 
 * - New align logic using negative offsets in the align blocks
 * - Darwin "magic" algorithm
 * - Unrolled last dword logic
 * */
char *lx_strcpy_r3(char *dst, const char *src);
/** \b special version allowing search for ideal prefetch placement
 * in conjunction with tools/permute.sh and dodef.h
 */
char *lx_strcpy_r4(char *dst, const char *src);

/* The timetest_boilerplate requires at least f, name, and results */
typedef char *(*T_def_Fn)(char *, const char *);


/**
 * \brief
 * the structure that \ref loop_test uses to define the article under test
 */
typedef	struct {
		T_def_Fn f; /*!<pointer to a function to test */
		char *name; /*!<human readable name for function to test */
} T_def_test;

T_def_test def_tests[] = {
		{strcpy, "local" },
		{lx_strcpy_r3, "lx_r3"},
		/*{lx_strcpy_r4, "lx_r4"}, /* uncomment for prefetch placement
					    * testing */
		{lx_strcpy_5a, "lx_5a"},
		{lx_strcpy_5a_r, "lx_5a_r"}, 

};

/* /def The number of entries in def_tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) 

#define DEF_RUN(f,c,sp,dp) f((char *)dp,(char *)sp)

#endif
