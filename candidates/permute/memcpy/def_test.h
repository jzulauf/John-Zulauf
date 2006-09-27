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
 * \file permuted/memcpy/def_test.h
 * \brief
 * this file is a wrapper for the automatically generated permutations 
 * of the memcpy function -- /ref permute_memcpy_test
 *
 */
/**
 * \defgroup permute_memcpy_test Permuted preamble memcpy performance test
 * \brief
 * The permutation optimzation test for memcpy.  Uses the framework
 * provided by \ref test and \ref loop_test
 *
 * To create the premuted source 
 * \code
 *	make source && make
 * \endcode
 *
 * \section permute_memcpy_notes Notes
 * Use utest and ptest as usual to determine the best of the permutations
 * ensuring that you have a large enough -t value to reduce variability
 * below the permutation variation difference.  The usual rule of "double
 * the repeat count until the data stops changing" applies, with the proviso
 * that a number of the permutations share the same performance profile
 * 
 */
 
#include "def_test_def.h"

#include "permuted_proto.h"

/**  The function pointer type for the function under test */
typedef void *(*T_def_Fn)(void *, const void *, size_t );


/**
 * \brief
 * the structure that \ref loop_test uses to define the function under test
 */
typedef	struct {
		T_def_Fn f; /*!<pointer to a function to test */
		char *name; /*!<human readable name for function to test */
} T_def_test;

T_def_test def_tests[] = {
	memcpy, "local",
#include "permuted_def.h"
};

/** The number of entries in def_tests (above) */
#define NUM_TEST (sizeof(def_tests)/sizeof(T_def_test))

#define DEF_NEXT(a,tc,c,s,sp,d,dp) DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) 

#define DEF_RUN(f,c,sp,dp) f(dp,sp,c)

#endif
