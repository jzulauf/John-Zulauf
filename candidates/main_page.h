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
 * This is the mainpage documentation file for the glibc performance
 * optimization candidates.
 */
/**
 * \mainpage Glibc Performance Optimization Candidate Evaluation
 * 
 * \section intro Introduction
 * This site documents the project to optimize Geode LX processor
 * performance by optimziations of individual glibc functions.
 *
 * \ref project_readme
 *
 * Current work includes:
 *	- \ref memcmp_test -- improved 1.47x
 *	- \ref memcpy_test -- improved 1.24x
 *	- \ref memset_test -- improved 1.1x
 *	- \ref strcmp_test -- improved 2.32x
 *	- \ref strcpy_test -- improved 1.65x
 *	- \ref strlen_test -- improved 1.2x
 *
 * Each of these is built on a common test framework comprising
 *	- \ref test
 *	- \ref loop_test
 *	- \ref stats
 *	- \ref opts
 *	- \ref deck
 *	- \ref fake_lib
 *	- \ref simple_table
 *	- \ref table_cell
 *	- \ref zstring
 *
 * For memcpy, permutation was used to determine the optimal ordering
 * of the preamble see: \ref permute_memcpy_test.
 */
