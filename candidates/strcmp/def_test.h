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
 * \file strcmp/def_test.h
 * \brief
 * This file defines the functions types and names for the strcmp performance
 * optimization test. /ref strcmp_test
 */
/**
 * \defgroup strcmp_test Strcmp performance test
 * \brief
 * The performance and optimzation test for strcmp.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * \section strcmp_status Status
 *
 * The testing and optimization has be completed
 *
 * \section strcmp_results Current Standings
 *
 *	- Winner: \ref i686_strcmp_r2() -- 2.3x improvement
 *	- Honorable mention: 
 *
 * \section strcmp_candidates Candidates
 *
 *	- \ref __strcmp_gg()
 *	- \ref gen_strcmp()
 *	- \ref i386_strcmp()
 *	- \ref i486__strcmp_gg()
 *	- \ref i686_strcmp()
 *	- \ref i686_strcmp_r1i()
 *	- \ref i686_strcmp_r1iu()
 *	- \ref i686_strcmp_r2()
 *	- \ref lx_strcmp_1dw()
 *
 * \section strcmp_notes Notes
 * Reference is made to the 'Darwin "magic" algorithm.' By this we are
 * referring to the dword zero find algorithm which uses the 0xfefefeff
 * "magic" number.  This is similar to (and mathematically equivalent to)
 * the i486 implementation but is implemented  in fewer instructions and
 * fewer conditional jumps. 
 *
 * Note also that as the Darwin code was PPC assembly, thus only the
 * algorithm and not the implementation is reflected in the
 * implementation here.
 *
 * The performance of the unrolled i686, while slightly better than lxDw's
 * \c repe approach, is suggestive of approaches to take for other
 * optimizations.
 */

#include "def_test_def.h"

/** the gg version from libc */
int __strcmp_gg (const char *p1, const char *p2);
/** "generic" the sysdeps/generic version */
int gen_strcmp (const char *p1, const char *p2);
/** "i386" the i386 inline version */
int i386_strcmp (const char *s1, const char *s2);
/** "i486" the i486 inline version */
int i486__strcmp_gg (__const char *s1, const char *s2);
/** "i686" the i686 assembly version */
int i686_strcmp (const char *s1, const char *s2);
/** "i686_1i" the i686 assembly version, with a tweak to allow a single incr */
int i686_strcmp_r1i (const char *s1, const char *s2);
/** "i686_1iu" the i686 assembly version, with a tweak to allow a single incr,
 *   16 byte unrolled loop with prefetch of s1 and s2 */
int i686_strcmp_r1iu (const char *s1, const char *s2);
/** "i686_1iu" with extra incl's removed */
int i686_strcmp_r2 (const char *s1, const char *s2);
/** "1dw" dword 0 attack on one string */
int lx_strcmp_1dw (const char *s1, const char *s2);


/* The timetest_boilerplate requires at least f, name, and results */
typedef int (*T_def_Fn)(const char *, const char * );


/**
 * \brief
 * the structure that \ref loop_test uses to define the article under test
 */
typedef	struct {
		T_def_Fn f; /*!<pointer to a function to test */
		char *name; /*!<human readable name for function to test */
} T_def_test;

T_def_test def_tests[] = {
		{strcmp, "local" },
		{__strcmp_gg, "local_gg" },
		{gen_strcmp, "generic"}, /* the sysdeps/generic version */
		{i386_strcmp, "i386"},	/* the i386 inline version */
		{i486__strcmp_gg, "i486"},	/* the i486 inline version */
		{i686_strcmp, "i686"},	/* the i686 assembly version */
		{i686_strcmp_r1i, "i686_1i"},	/* the i686 assembly version, with the 1 increment trick */
		{i686_strcmp_r1iu, "i686_1iu"},
		{i686_strcmp_r2, "i686_r2"},
		{lx_strcmp_1dw, "1dw"},	/* see comment above */
};

/* /def The number of entries in def_tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) 

#define DEF_RUN(f,c,sp,dp) f(sp,dp)

#endif
