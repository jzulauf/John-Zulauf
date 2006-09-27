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
  \file
  \brief Contains the overall background information for the project

*/
/**
 * \defgroup project_readme README for Glibc optimization project
 *
 *   \brief
 * 
 *   the glibc candidate (test) function directories
 * 
 * Doxygen documentation is avaiable for the project. Use either
 * 
 * 	doxygen -g doxygen.config
 * 
 * or
 * 
 * 	make doc
 * 
 * to generate.
 * 
 * \section common_elements Common Elements
 * 
 * Each directory below constitutes a performance and unit test suite for
 * evaluating and optimizing a given glibc func.  The functions from
 * the various sysdeps subdirectories are copied here, minimally edited (e.g.
 * renaming the function to \<arch\>_\<func\>), and link with the utest
 * and ptest applications.
 * 
 * \subsection define_ptest ptest
 *
 * 	ptest is the peformance test, and is uses the framework in
 * 	common for standard options handling, test inner and outer loops,
 * 	statitistics gathering and reporting.
 * 
 * \subsection define_utest utest
 *
 * 	the unit test application for each candidate.  Typically these use
 * 	oversized buffers to check for overruns and underruns.
 * 
 * \subsection reference_imp Reference Implementation
 * memset contains the "canonical form" of ptest, which can be branched
 * and modified for any additional candidates.
 * 
 * \section test_customization Creating a new test
 * 
 * \subsection test_must Things you will need to modify:
 * 
 * \b	T_def_Fn
 *
 * 		this needs to be defined to match the prototype of
 * 		the function under test
 * 
 * \b	def_tests[]
 * 
 * 		needs to contain the function pointer and name of the
 * 		implementations to test
 * 
 * \b	DO_TEST_NEXT
 * 
 * 		needs to contain the instructions to move through the buffers
 * 
 * \b	DO_TEST_RUN
 * 
 * 		needs to have the invocation line for the function under test
 * 		(Note: the variables to use must be visible inside the
 * 		 do_test_func boilerplate)
 * 
 * \b	Makefile
 * 
 * 		LGPL_OBJS, BSD_OBJS to include the functions under test
 * 
 * \subsection test_may Things you may need to modify
 * 
 * \b	Options
 * 
 * 		you can add options easily calling the opts_def function
 * 		with useful automated value setting (c.f. "Standard Actions"
 * 		in common/include/opts_util.h). Add these just below
 * 		test_init in ptest.c:main.
 * 
 * \b	main() while ... opts_getopt (in ptest.c)
 * 
 * 		you may need some behavior not easily supported in the
 * 		T_opts_action callback
 * 
 * \b	T_test_buffer::f_wrap
 * 
 * 		You may need to override this if you need to refill the src
 * 		or dst buffer on wrap-around. 
 * 
 * 
 * \b	T_test_buffer::f_count
 * 
 * 		if the buffer contents need to change for each count (for
 * 		example strlen) you can set f_count to a new count change
 * 		operation. Please examine the default implementation for
 * 		semantices.
 * 
 */
