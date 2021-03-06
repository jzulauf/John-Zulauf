/* strcpy/stpcpy implementation for i586.
   Copyright (C) 1997, 2000, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

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
 * Optimizations for performance testing on Geode LX -- John M. Zulauf
 * Copyright (C) 2006 Advanced Micro Devices, Inc
JMZ */

/* JMZ
 * Strcpy implementation using
 *  - New align logic using negative offsets in the align blocks
 *  - Darwin "magic" algorithm
 *  - Unrolled last dword logic
JMZ */

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

#define PARMS	LINKAGE+4	/* space for 3 saved regs */
#define RTN	PARMS
#define DEST	RTN+RTN_SIZE
#define SRC	DEST+PTR_SIZE

/*JMZ
 *Renamed function
JMZ */
#ifndef USE_AS_STPCPY
# define STRCPY lx_strcpy_r3
#endif

/* The dword comparison for "any byte zero" 
 * works by subtracting 1 from each byte
 * and looking for sign flips that will only
 * occur for 0 -1. The tricky part is the that
 * while 0xFE *is* -2 if all lower bytes aren't
 * 0 there will be a carry.
 */
#define magic     0xfefefeff

	.text
ENTRY (BP_SYM (STRCPY))
	ENTER

	pushl	%esi
	movl	%edi, %edx

	cld
	movl	SRC(%esp), %esi
	CHECK_BOUNDS_LOW (%esi, SRC(%esp))

	leal	-1(%esi), %ecx
	xorl	%eax,%eax
	andl	$3, %ecx

	leal	3(%esi), %esi

	movl	DEST(%esp), %edi
	CHECK_BOUNDS_LOW (%edi, DEST(%esp))

	andl	$0xFFFFFFFC,%esi

/*FIXME: Won't wont ifdef PIC because I'm using edx to save edi */
#ifdef PIC
	call	2f
2:	popl	%edx
	/* 0xb is the distance between 2: and 1: but we avoid writing
	   1f-2b because the assembler generates worse code.  */
	leal	0xb(%edx,%ecx,8), %ecx
#else
	leal	1f(,%ecx,8), %ecx


#endif

	jmp	*%ecx
	.align 8
1:
	orb   -3(%esi),%al
	stosb
	jz	L(landing)
	xorl  %eax,%eax

	orb   -2(%esi),%al
	stosb
	jz	L(landing)
	xorl  %eax,%eax

	orb   -1(%esi),%al
	stosb
	jz	L(landing)
	xorl  %eax,%eax

/* We have to avoid the jump to the end. suckage, eh? */
	jmp L(1)

/* This target is needed because the unrolls have move L(end)
 * further away than 127 bytes */
L(landing):
	jmp L(end)

L(1):	
	movl	(%esi), %eax
	leal	magic(%eax), %ecx
	not	%eax
	andl	%eax,%ecx
	testl	$0x80808080,%ecx
	jnz	L(4)
	movsl;

	movl	(%esi), %eax
	leal	magic(%eax), %ecx
	not	%eax
	andl	%eax,%ecx
	testl	$0x80808080,%ecx
	jnz	L(4)
	movsl;

	movl	(%esi), %eax
	leal	magic(%eax), %ecx
	not	%eax
	andl	%eax,%ecx
	testl	$0x80808080,%ecx
	jnz	L(4)
	movsl;

	movl	(%esi), %eax
/*#define NO_PREFETCH*/
#ifndef NO_PREFETCH
	prefetch 0x20(%esi)
#endif
	leal	magic(%eax), %ecx
	not	%eax
	andl	%eax,%ecx
	testl	$0x80808080,%ecx
	jnz	L(4)
	movsl;

	jmp L(1)


L(4):
#ifdef USE_AS_STPCPY
	xorl	%ecx, %ecx
#endif

	movb	(%esi),%al
	testb	$0xFF,%al
	movb	%al,(%edi)
#ifdef USE_AS_STPCPY
	incl %ecx
#endif
	jz	L(end)

	movb	1(%esi),%al
	testb	$0xFF,%al
	movb	%al,1(%edi)
#ifdef USE_AS_STPCPY
	incl %ecx
#endif
	jz	L(end)

	movb	2(%esi),%al
	testb	$0xFF,%al
	movb	%al,2(%edi)
#ifdef USE_AS_STPCPY
	incl %ecx
#endif
	jz	L(end)

	movb	3(%esi),%al
	testb	$0xFF,%al
	movb	%al,3(%edi)
#ifdef USE_AS_STPCPY
	incl %ecx
#endif

L(end):	
	/* GKM FIXME: check high bounds  */
#ifdef USE_AS_STPCPY
	decl    %edi
	leal	(%ecx,%edi), %eax
#else
	movl	DEST(%esp), %eax
#endif
	RETURN_BOUNDED_POINTER (DEST(%esp))
	popl	%esi
	movl	%edx,%edi

	LEAVE
	RET_PTR
END (BP_SYM (STRCPY))
#ifndef USE_AS_STPCPY
libc_hidden_builtin_def (strcpy)
#endif
