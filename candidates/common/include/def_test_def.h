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
#ifndef _DEF_TEST_DEF_H_
#define _DEF_TEST_DEF_H_
/**
 * \file
 * \brief
 * Defines useful default/common macros for def_test.h to use
 */

/** \def DEF_NEXT_SAMP_DEF(b,tc,samp,data)
 *  \brief Get the next sample and data pointers for the given buffer */
#define DEF_NEXT_SAMP_DEF(b,tc,samp,data) { \
	samp = test_buffer_next((b),tc);	\
	data = samp->entry;	\
}

/** \def DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp)
 *  \brief Update the src and dest samples */
#define DEF_NEXT_BOTH_DEF(a,tc,c,s,sp,d,dp) { \
	DEF_NEXT_SAMP_DEF(a->bufs[TEST_SRC],tc,s,sp);	\
	DEF_NEXT_SAMP_DEF(a->bufs[TEST_DST],tc,d,dp);	\
	c = s->count;	\
}

/** \def DEF_NEXT_SRC_DEF(a,tc,c,s,sp) \brief Update just the src sample */
#define DEF_NEXT_SRC_DEF(a,tc,c,s,sp) { \
	DEF_NEXT_SAMP_DEF(a->bufs[TEST_SRC],tc,s,sp);	\
	c = s->count;	\
}

#endif
