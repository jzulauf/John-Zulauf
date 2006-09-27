/* Highly optimized version for ix86, x>=6.
   Copyright (C) 1999, 2000, 2002, 2003 Free Software Foundation, Inc.
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

/* JMZ
 * BASED ON i686/strcmp.S
 * uses x86 addressing to reduce the number of increments to 1
 * 16 byte unroll the loop to allow for prefetching of s1 and s2
 * uses offset allow increments only once per unroll (a 10% savings)
JMZ */


#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

#define PARMS	LINKAGE		/* no space for saved regs */
#define STR1	PARMS
#define STR2	STR1+PTR_SIZE

/* JMZ
 * renamed function
 */
        .text
ENTRY (BP_SYM (i686_strcmp_r2))
	ENTER

	movl	STR1(%esp), %ecx
	movl	STR2(%esp), %edx
	CHECK_BOUNDS_LOW (%ecx, STR1(%esp))
	CHECK_BOUNDS_LOW (%edx, STR2(%esp))

/* JMZ
 * create the s1, s2 offset used below
JMZ */
	subl	%edx,%ecx

L(oop):

	movb	(%edx,%ecx), %al
	cmpb	(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)


	movb	1(%edx,%ecx), %al
prefetch 0x20(%edx,%ecx)
	cmpb	1(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)


	movb	2(%edx,%ecx), %al
prefetch 0x20(%edx)
	cmpb	2(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	3(%edx,%ecx), %al
	cmpb	3(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	4(%edx,%ecx), %al
	cmpb	4(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	5(%edx,%ecx), %al
	cmpb	5(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	6(%edx,%ecx), %al
	cmpb	6(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	7(%edx,%ecx), %al
	cmpb	7(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	8(%edx,%ecx), %al
	cmpb	8(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	9(%edx,%ecx), %al
	cmpb	9(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	10(%edx,%ecx), %al
	cmpb	10(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	11(%edx,%ecx), %al
	cmpb	11(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	12(%edx,%ecx), %al
	cmpb	12(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	13(%edx,%ecx), %al
	cmpb	13(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	14(%edx,%ecx), %al
	cmpb	14(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	movb	15(%edx,%ecx), %al
	cmpb	15(%edx), %al
	jne	L(neq)
	testb	%al, %al
	jz	L(al_is_zero)

	leal	16(%edx),%edx

	jmp	L(oop)

L(al_is_zero):
	xorl	%eax, %eax
	/* when strings are equal, pointers rest one beyond
	   the end of the NUL terminators.  */
	/* FIXME -- bounds checking will be wrong for ecx */
	CHECK_BOUNDS_HIGH (%ecx, STR1(%esp), jbe)
	CHECK_BOUNDS_HIGH (%edx, STR2(%esp), jbe)
	LEAVE
	ret

#ifndef __BOUNDED_POINTERS__
L(neq):	movl	$1, %eax
	movl	$-1, %ecx
	cmovbl	%ecx, %eax
#else
L(neq):	movl	$1, %eax
	ja	L(chk)
	negl	%eax
	/* When strings differ, pointers rest on
	   the unequal characters.  */
	/* FIXME -- bound checking will be wrong for ecx */
L(chk):	CHECK_BOUNDS_HIGH (%ecx, STR1(%esp), jb)
	CHECK_BOUNDS_HIGH (%edx, STR2(%esp), jb)
#endif

	LEAVE
	ret
END (BP_SYM (strcmp))
libc_hidden_builtin_def (strcmp)
