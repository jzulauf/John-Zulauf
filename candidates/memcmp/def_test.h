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
 * \file memcmp/def_test.h
 * \brief
 * This file contains the test definitions common to ptest and utest
 * for \ref memcmp_test
 */

/**
 * \defgroup memcmp_test Memcmp performance test
 * \brief
 * The performance and optimzation test for memcmp.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * \section memcmp_status Status
 *
 * The testing and optimization has be completed
 *
 * \section memcmp_results Current Standings
 *
 *	- Winner: lxDw lxd_memcmp 26-66% improvement vs. local (i486)
 *	- Honorable mention: i686 28-58% improvement vs. local (i486)
 *
 * \section memcmp_candidates Candidates
 *
 *	- \b local the i486 glibc memcmp
 *	- \b i386 the i386 glibc assembly code
 *	- \b i686 the i686 glibc assembly code (unrolled movl; movl; cmpl;)
 *	- \b lxCond a rep/byte compare (repe;cmpsb)
 *	- \b lxDw a rep/dword compare (repe;cmpsl), with remainder cmpsb
 *
 * \section memcmp_notes Notes
 *
 * The performance of the unrolled i686, while slightly less than lxDw's
 * \c repe approach, is suggestive of approaches to take for other
 * optimizations.
 */

#ifndef _DEF_TEST_H_
#define _DEF_TEST_H_
#include "def_test_def.h"

/* Declare the tests */
/** renamed version of sysdeps/i386/memcmp.S */
int i386_memcmp(const void *s1, const void *s2, size_t n);
/** renamed version of sysdeps/i386/i686/memcmp.S */
int i686_memcmp(const void *s1, const void *s2, size_t n);
/** derived from i386_memcmp() with cmov based return value logic */
int lxc_memcmp(const void *s1, const void *s2, size_t n);
/** derived from i386_memcmp() with dword comparison and
 *  cmov based return value logic */
int lxd_memcmp(const void *s1, const void *s2, size_t n);

/** declares the signature of the function under test, used by \ref loop_test */
typedef int (*T_def_Fn)(const void *, const void *, size_t  );


/** The \ref loop_test requires at least f and name fields */
typedef	struct {
		T_def_Fn f; /*!< pointer to a function to test */
		char *name; /*!< human readable string for the function */
} T_def_test;

/** the list of tests and their name */
T_def_test def_tests[] = {
	{memcmp, "local"},
	{i386_memcmp, "i386"},
	{i686_memcmp, "i686"},
	{lxc_memcmp, "lxCond"},
	{lxd_memcmp, "lxDw"},
};

/** The number of entries in tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

/** the "get next sample" macro for this test, used by loop_inner() */
#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) 

/** the "run the test function" macro for this test, used by loop_inner() */
#define DEF_RUN(f,c,ps,pd) f(ps,pd,c)

/** the "record the results" macro for this test, used by loop_inner() */
#define DEF_REC(c,t,lr,d) DEF_REC_DEF(c,t,lr,d)

#endif
