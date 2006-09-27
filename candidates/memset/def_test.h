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
 * \file memset/def_test.h
 * \brief
 * This file defines the functions types and names for the memset performance
 * optimization test.  \ref memset_test
 *
 */
/**
 * \defgroup memset_test Memset performance test
 * \brief
 * The performance and optimzation test for memset.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * \b Status
 *
 * The testing and optimization is complete.  The winners are noted below.
 *
 * \b Current \b Standings
 *
 * \b Top \b Three \b Overall
 * 	-# \b lx_mmx64_h2 lx_memset_mmx64_h2() lx_memset_mmx_h2.S
 *		- increases the minimum size for mmx set operations to 512B
 * 	-# \b lx_hyb lx_memset_hyb() lx_memset_hyb.S
 * 		- combines size and align test with a cmovz (fewest jumps of
 * 		  implementations handling alignment)
 * 		- 5-15% better than baseline for dword aligned < 512B
 * 		- Up to 6% worse than baseline 0-7B, 2-3% worse 8-11B
 * 			- These are cases handled by inlines by gcc
 * 		- 2-5% better than baseline for 12-512B for unaligned memsets
 * 	-# \b lx_zan lx_memset_zan() lx_memset_zan.S
 * 		- unaligned performance better than lx_hyb for < 100B
 * 		- aligned performance far worse than lx_hyb
 * 
 * \b Special \b Recognition
 *	- \b lx_simp lx_memset_simp() lx_memset_simp.S
 *		- fastest \b <512B \b aligned (17% above baseline) 
 * 		- 10% \b slower than baseline unaligned
 * 	- \b lx_mmx64_BA lx_memset_mmx64_BA() lx_memset_mmx64_BA.S 
 * 		- \b best \b overall \b >512B
 * 		- quadword aligned quadword writes (\b fast)
 * 		- 16% above baseline for aligned
 * 		- 15% above baseline for unaligned (2KB-4KB)
 * 	- \b lx_mmx64simp lx_memset_mmx64simp() lx_memset_mmx64simp.S 
 * 		- \b fastest \b >512B \b aligned
 * 		- 5-7% \b slower than baseline unaligned
 *  - \b Also \b ran (nothing to see here folks... move along)
 *  	- \b local memset() 
 * 		- \b baseline
 *  		- uses i386 'C' with inline asm
 * 	- \b i386 i386_memset() i386_memset.c
 * 		- glibc implementation for x[34]86
 * 		- same as "local" on Gentoo image
 * 	- \b i586 i586_memset() i586_memset.c
 * 		- glibc implementation for x586
 * 		- uses unrolled movl loop
 * 		- slowest overall
 * 	- \b 686 i686_memset() i686_memset.S
 * 		- glibc implementation for x686
 * 		- dword alignment preamble broken patch submitted to
 * 		  glibc project
 * 		- up to 10% worse than baseline for non-aligned
 * 	- \b lx_hyb_BA lx_memset_hyb_BA() lx_memset_hyb_BA.S
 * 		- same as lx_hyb, but alignment logic used quadword boundary
 * 		- BIGGER ALIGN SIZE DOESN'T HELP HYB (and sometimes slows)
 * 	- \b lx_mmx lx_memset_mmx() lx_memset_mmx.S "lx_mmx" }
 * 		- 32 byte unroll
 * 		- never faster than lx_hyb 
 * 		- dword aligned qword writes (takes two writes \b slow
 * 	- \b lx_mmx64 lx_memset_mmx64() lx_memset_mmx64.S
 * 		- 64 byte unroll
 * 		- fast >512B and aligned
 * 		- unaligned uses dword aligned qword writes (\b slow)
 */
 
#include "def_test_def.h"

/** \brief the 386 glibc implementation of memset */
void *i386_memset(void *dst, int c, size_t n);
/** \brief the 586 glibc implementation of memset */
void *i586_memset(void *dst, int c, size_t n);
/** \brief
 * the 686 glibc implementation of memset, alignment preamble is borked */
void *i686_memset(void *dst, int c, size_t n);
/** \brief
 * the 686 glibc implementation of memset, w/ a fix to the alignment preamble */
void *i686_memset_fix(void *dst, int c, size_t n);
/** \brief lx optimization replacing the three conditional jumps of the 686
 * implementation with a rep; stosb for the unaligned portion */
void *lx_memset_zan(void *dst, int c, size_t n);
/** \brief lx optimization with no alignment correction */
void *lx_memset_simp(void *dst, int c, size_t n);
/** \brief lx optimization which combines the "short" and "aligned" tests
 * into a cmovz and a single conditional jump */
void *lx_memset_hyb(void *dst, int c, size_t n);
/** \brief same as lx_memset_hyb(), but with quadword alignment logic instead
 * dword alignment logic */
void *lx_memset_hyb_BA(void *dst, int c, size_t n);
/** \brief lx optimization using a 32 byte unroll loop and movq (slow) */
void *lx_memset_mmx(void *dst, int c, size_t n);
/** \brief lx optimization using a 64 byte unroll loop and movq w/
 * dword alignment logic (which is a bad idea) */
void *lx_memset_mmx64(void *dst, int c, size_t n);
/** \brief lx optimization using a 64 byte unroll loop and movq but w/
 * quadword alignment logic  */
void *lx_memset_mmx64simp(void *dst, int c, size_t n);
/** \brief lx optimization using a 64 byte unroll loop and movq w/ no
 * alignment logic (which is fastest if the dest is quadword aligned)  */
void *lx_memset_mmx64_BA(void *dst, int c, size_t n);

/** skips the mmx loop for < 512 bytes */
void *lx_memset_mmx64_h2(void *dst, int c, size_t n);

/** The function pointer type for the function under test */
typedef void *(*T_def_Fn)(void *, int , size_t );

/**
 * \brief
 * the structure that \ref loop_test uses to define the function under test
 */
typedef	struct {
		T_def_Fn f; /*!<pointer to a function to test */
		char *name; /*!<human readable name for function to test */
} T_def_test;

/** The list of test functions for loop_test() */
T_def_test def_tests[] = {
	{memset, "local"},
	/*{i386_memset, "i386"},  /* Gentoo local */
	/*{i586_memset, "i586"},  /* slowest overall */
	/*{i686_memset, "i686"},  /* alignment preamble broken */
	/*{i686_memset_fix, "i686fix"}, /*  much slower for random alignments */
	{lx_memset_zan, "lx_zan" },   /* overall just worse than _hyb */
	/*{lx_memset_simp, "lx_simp" }, /*fastest aligned, slowest unaligned */
	{lx_memset_hyb, "lx_hyb" },   /* overall winner -R and -A */
	/*{lx_memset_hyb_BA, "lx_hyb_BA" }, /* BIGGER ALIGN SIZE DOESN'T HELP */
	/*{lx_memset_mmx, "lx_mmx" }, /* 32 by unroll is never faster than _hyb */
	/*{lx_memset_mmx64, "lx_mmx64" }, /* faster than hyb for > 512 (256?) */
	/*{lx_memset_mmx64simp, "lx_mmx64simp" }, /* No alignment or skip logic */
	{lx_memset_mmx64_BA, "lx_mmx64_BA" }, /*BIGGER ALIGN SIZE */
	{lx_memset_mmx64_h2, "lx_mmx64_h2" } /*Big align and skip mmx if < 512 */
};

/** The number of entries in def_tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

/** tells loop_inner() not to declare variables for destination buffers */
#define DEF_NO_DECLARE_DST

/** the "get the next sample" macro for this test */
#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_SRC_DEF(a,tc,c,s,sp)

/** the "run the test function" macro for this test */
#define DEF_RUN(f,c,ps,pd) f(ps,'A',c)

/** the "record the results" macro for this test */
#define DEF_REC(c,t,lr,d) DEF_REC_DEF(c,t,lr,d)

#endif
