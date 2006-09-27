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
 * \file strlen/def_test.h
 * \brief
 * This file contains the test definitions common for \ref strlen_test
 */

/**
 * \defgroup strlen_test Strlen performance test
 *
 * \brief
 * The performance and optimzation test for strlen.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * \section strlen_status Status
 *
 * The testing and optimization has been completed
 *
 * \section strlen_results Results
 *
 *	- Winner: \ref lx_strlen_5a_r2() 1.2x better overall
 *	- Honorable mention: \ref i486p_strlen() 1.18x better by
 *				just adding the prefetch
 *
 * \section strlen_candidates Candidates 
 *
 *	- \ref i386_strlen()
 *	- \ref i486_inline_strlen()
 *	- \ref i486p_strlen()
 *	- \ref i486_strlen()
 *	- \ref lx_strlen_5a_r2()
 *	- \ref lx_strlen_5a_r()
 *	- \ref lx_strlen()
 *	- \ref lxus_strlen()
 *	- \ref lxu_strlen()
 *
 * \section strlen_notes Notes
 * Reference is made to the 'Darwin "magic" algorithm.' By this we are
 * referring to the dword zero find algorithm which uses the 0xfefefeff
 * "magic" number.  This is similar to (and mathematically equivalent to)
 * the i486 implementation but is implemented  in fewer instructions and
 * fewer conditional jumps. 
 *
 * Note also that as the Darwin code was PPC assembly, thus only the
 * algorithm and not the implementation is reflected in the
 * implementation here.
 */
 
#ifndef _DEF_TEST_H_
#define _DEF_TEST_H_
#include "def_test_def.h"

/* Declare the tests */
/** renamed from sysdeps/i386/strlen.c */
size_t i386_strlen(const char *);
/** extracted from sysdeps/i386/i486/bits/string.h */
size_t i486_inline_strlen(const char *);
/** Strlen implementation
 *  - Based on sysdeps/i486/strlen.S 
 *  - Prefetch added to 'find zero in dword' unroll loop
 */
size_t i486p_strlen(const char *);
/** Copied an renamed from sysdeps/i486/strlen.S */
size_t i486_strlen(const char *);
/** Based on i468_strlen()
 * - Uses i468_strlen() alignment logic
 * - Uses Darwin "magic" algorithm for zero finding
 */
size_t lx_strlen_5a_r2(const char *);
/** Completely gutted from the i486_strlen() implementation
 * - Uses computed address alignment logic
 * - Uses Darwin "magic" algorithm for zero finding
 */
size_t lx_strlen_5a_r(const char *);
/** A simple assembly implementation using repnz;scasb */
size_t lx_strlen(const char *);
/** A assembly implementation using 32 byte repnz;scasb, in
 * an unroll loop */
size_t lxus_strlen(const char *);
/** A assembly implementation using 32 byte repnz;scasb, in
 * an unroll loop.  Alignment logic, puts the string to scan
 * on a cacheline (32 byte) boundary for the unroll */
size_t lxu_strlen(const char *);

/* The timetest_boilerplate requires at least f, name, and results */
typedef size_t (*T_def_Fn)(const char * );


/* The timetest_boilerplate requires at least f, name, and results */
typedef	struct {
		T_def_Fn f;
		char *name;
} T_def_test;

/* /var defines the list of tests and their name */
T_def_test def_tests[] = {
	{strlen, "local"},
	/* {i386_strlen, "i386"}, */
	/*{i486_strlen, "i486"}, /* worse than i486p */
	{i486p_strlen, "i486p"},
	/* {i486_inline_strlen, "i486_ia"}, */
	{lx_strlen, "lxSimp"},
	{lxus_strlen, "lxus"},
	/*{lx_strlen_5a_r, "lx_5a_r"}, /* worse than _r2 */
	{lx_strlen_5a_r2, "lx_5a_r2"},
	/*{lxu_strlen, "lxPref"},*/
};

/* /def The number of entries in tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

#define DEF_NO_DECLARE_DST

#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_SRC_DEF(a,tc,c,s,sp)

#define DEF_RUN(f,c,ps,pd) f(ps)

#define DEF_REC(c,t,lr,d) DEF_REC_DEF(c,t,lr,d)

#endif
