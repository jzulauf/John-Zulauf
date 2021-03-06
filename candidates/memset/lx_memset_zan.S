/* memset/bzero -- set memory area to CH/0
   Highly optimized version for ix86, x>=6.
   Copyright (C) 1999, 2000, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1999.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* JMZ
 * Optimizations for performance testing John M. Zulauf
 * Copyright (C) 2006 Advanced Micro Devices, Inc
JMZ */

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

/* BEWARE: `#ifdef memset' means that memset is redefined as `bzero' */
#define BZERO_P (defined memset)

#define PARMS	LINKAGE+4	/* space for 1 saved reg */
#if BZERO_P
# define DEST	PARMS
# define LEN	DEST+PTR_SIZE
#else
# define RTN	PARMS
# define DEST	RTN+RTN_SIZE
# define CHR	DEST+PTR_SIZE
# define LEN	CHR+4
#endif

        .text
#if defined PIC && !defined NOT_IN_libc && !BZERO_P
ENTRY (__memset_chk)
	movl	12(%esp), %eax
	cmpl	%eax, 16(%esp)
	jb	HIDDEN_JUMPTARGET (__chk_fail)
END (__memset_chk)
#endif
/*JMZ
 * uses two conditional tests to control execution or skipping of
 * alignement logic (as opposed to the single test of lx_hyb)
JMZ */
ENTRY (BP_SYM (lx_memset_zan))
	ENTER

	cld
	pushl	%edi
	movl	DEST(%esp), %edi
	movl	LEN(%esp), %edx
	CHECK_BOUNDS_BOTH_WIDE (%edi, DEST(%esp), %edx)
#if BZERO_P
	xorl	%eax, %eax	/* fill with 0 */
#else
	movzbl	CHR(%esp), %eax
#endif

#define LX_SKIP_ZERO_JMP
#ifndef LX_SKIP_ZERO_JMP
	cmpl $0, %edx
	jz	1f
#endif

#ifndef LX_SKIP_ALIGN
	/* The align logic is expensive, minimum for the skip align cmp is
	 * 4 as the align logic will fail for count < 4
	 */
	cmpl $160, %edx /* empircally optimial */
	jbe 2f

	/* Dword align the rep stosl below */
	movl	%edi, %ecx
	andl	$3, %ecx
#ifdef LX_JMP_ZERO_ALIGN_REP
        jecxz   2f
#endif
	neg	%ecx
	add	$4, %ecx

#ifndef LX_JMP_ZERO_ALIGN_REP
	/* suppress the 4 byte copy here if
	   we are stopping it with the jump */
	andl	$3, %ecx
#endif
	
	subl	%ecx, %edx
	
	/*
	 * An empty rep might be far better than a misprediction
	 * and certainly better than the three coin flip jumps of
	 * the 686 implementation for misalignment
	 */
	rep
	stosb
#endif
	/* Set the dwords */
2:	movl %edx, %ecx
	shrl $2, %ecx
	andl $3, %edx
#if !BZERO_P
	imul	$0x01010101, %eax
#endif
	rep
	stosl

	/* and the mod 4 bytes */
3:	movl	%edx, %ecx
	rep
	stosb

1:
#if !BZERO_P
	movl DEST(%esp), %eax	/* start address of destination is result */
	RETURN_BOUNDED_POINTER (DEST(%esp))
#endif
	popl	%edi

	LEAVE
#if BZERO_P
	ret
#else
	RET_PTR
#endif
END (BP_SYM (memset))
libc_hidden_builtin_def (memset)
