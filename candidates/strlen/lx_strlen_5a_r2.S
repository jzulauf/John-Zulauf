/* strlen implementation for geode
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
 * Based on i468_strlen()
 * - Uses i468_strlen() alignment logic
 * - Uses Darwin "magic" algorithm for zero finding
JMZ */

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

#define PARMS	LINKAGE	/* space for no saved regs */
#define STR	PARMS
#define DEST	RTN+RTN_SIZE
#define SRC	DEST+PTR_SIZE

/*JMZ
 *Renamed function
JMZ */

/* The dword comparison for "any byte zero" 
 * works by subtracting 1 from each byte
 * and looking for sign flips that will only
 * occur for 0 -1. The tricky part is the that
 * while 0xFE *is* -2 if all lower bytes aren't
 * 0 there will be a carry.
 */
#define magic     0xfefefeff

	.text
ENTRY (BP_SYM (lx_strlen_5a_r2))
	ENTER

	movl	STR(%esp), %eax
	CHECK_BOUNDS_LOW (%eax, STR(%esp))

	movl	%eax, %ecx

	/* LGPL code */

	/* The following is quoted from the 486 strlen glibc routine LPGL'd */
	andl $3, %ecx		/* mask alignment bits */
	jz L(1)			/* aligned => start loop */
	cmpb %ch, (%eax)	/* is byte NUL? */
	je L(end)			/* yes => return */
	incl %eax		/* increment pointer */

	xorl $3, %ecx		/* was alignment = 3? */
	jz L(1)			/* yes => now it is aligned and start loop */
	cmpb %ch, (%eax)	/* is byte NUL? */
	je L(end)			/* yes => return */
	addl $1, %eax		/* increment pointer */

	subl $1, %ecx		/* was alignment = 2? */
	jz L(1)			/* yes => now it is aligned and start loop */
	cmpb %ch, (%eax)	/* is byte NUL? */
	je L(end)			/* yes => return */
	addl $1, %eax		/* increment pointer */

	/* end LGPL code */

/* This is the dword unroll */
L(1):	
	movl	(%eax), %edx
	prefetch 0x20(%eax)
	leal	magic(%edx), %ecx
	not	%edx
	andl	%edx,%ecx
	testl	$0x80808080,%ecx
	jnz	L(find_zero)

	movl	4(%eax), %edx
	leal	magic(%edx), %ecx
	not	%edx
	andl	%edx,%ecx
	testl	$0x80808080,%ecx
	jnz	L(add_offset_4)

	movl	8(%eax), %edx
	leal	magic(%edx), %ecx
	not	%edx
	andl	%edx,%ecx
	testl	$0x80808080,%ecx
	jnz	L(add_offset_8)

	movl	12(%eax), %edx
	leal	magic(%edx), %ecx
	not	%edx
	andl	%edx,%ecx
	testl	$0x80808080,%ecx
	jnz	L(add_offset_12)

	addl	$16, %eax
	jmp L(1)

L(add_offset_12):
	addl	$4, %eax
L(add_offset_8):
	addl	$4, %eax
L(add_offset_4):
	addl	$4, %eax

L(find_zero):
L(4):
	movl	(%eax), %edx
	testb 	%dl, %dl
	jz	L(end)
	incl 	%eax

	testb 	%dh, %dh
	jz	L(end)
	incl 	%eax

	testl 	$0x00FF0000, %edx
	jz	L(end)
	incl 	%eax

L(end):	
	subl	STR(%esp), %eax
	
	LEAVE
	RET_PTR
END (BP_SYM (lx_strlen_5a_r))
libc_hidden_builtin_def (lx_strlen_5a_r)
