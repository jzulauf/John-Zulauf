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
#ifndef INCLUDE_FAKE_H
#define  INCLUDE_FAKE_H
#define HAVE_ELF
/**
 * \file
 * \brief
 * Defines to allow glibc routines to compile unaltered
 */

/* from sysdep.h */
/* Local label name for asm code. */
#ifndef L
#ifdef HAVE_ELF
#define L(name)         .L##name
#else
#define L(name)         name
#endif
#endif

/* Unbounded pointers occupy one word.  */
#   define PTR_SIZE 4
/* Unbounded pointer return values are passed back in the register %eax.  */
#   define RTN_SIZE 0
/* Use simple return instruction for unbounded pointer values.  */
#   define RET_PTR ret
/* Don't maintain frame pointer chain for leaf assembler functions.  */
#   define ENTER
#   define LEAVE
/* Stack space overhead of procedure-call linkage: return address only.  */
#   define LINKAGE 4
/* Stack offset of return address after calling ENTER.  */
#   define PCOFF 0

#   define CHECK_BOUNDS_LOW(VAL_REG, BP_MEM)
#   define CHECK_BOUNDS_HIGH(VAL_REG, BP_MEM, Jcc)
#   define CHECK_BOUNDS_BOTH(VAL_REG, BP_MEM)
#   define CHECK_BOUNDS_BOTH_WIDE(VAL_REG, BP_MEM, LENGTH)
#   define RETURN_BOUNDED_POINTER(BP_MEM)

#   define RETURN_NULL_BOUNDED_POINTER

#   define PUSH_ERRNO_LOCATION_RETURN
/* end from sysdep.h

/* override from sysdep.h */
#define ENTRY(name) 		\
	.globl name;		\
	.type name, @function;	\
	name:

#define END(name)
/* misc override */
#define libc_hidden_builtin_def(name)
#define weak_alias(sym1, sym2)

#define ALIGN(log) .align 1<<log

/* from bp-sym.h */
/*#define BP_SYM(name) test_##name*/
#define BP_SYM(name) name

/* from generic/memcopy.h */
#define op_t    unsigned long
#define OPSIZ       (sizeof(op_t))

#endif
