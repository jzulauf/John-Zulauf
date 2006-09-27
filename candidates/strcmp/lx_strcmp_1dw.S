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
 * Optimizations for performance testing John M. Zulauf
 * Copyright (C) 2006 Advanced Micro Devices, Inc
JMZ */
/* JMZ
 * This is a wildly modified version of the strcpy routine to try
 * to make a faster strcmp. The alignment logic is the only piece
 * that remains of the original.  The 0 finding magic is the
 * algorithm discussed in the comments of Darwin strcpy
 *
 * Approach:
 *	we will make one of the strings dword aligned,
 *	then to a dword aligned "end-of-string" test
 *	on that string, with a byte by byte compare with
 *	the other
JMZ */

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

#define PARMS	LINKAGE+4	/* space for 2 saved regs */
#define RTN	PARMS
#define STR1	RTN+RTN_SIZE
#define STR2	STR1+PTR_SIZE

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
ENTRY (lx_strcmp_1dw)

	pushl	%esi

	movl	STR1(%esp), %esi
	movl	STR2(%esp), %edx
	CHECK_BOUNDS_LOW (%esi, STR1(%esp))
	CHECK_BOUNDS_LOW (%edx, STR2(%esp))

	subl	%esi,%edx /* The one increment trick */
	leal	-1(%esi), %eax
	andl	$3, %eax

/* FIXME: the PIC path is fully broken here... */
#ifdef PIC
	call	2f
2:	popl	%edx
	/* 0xb is the distance between 2: and 1: but we avoid writing
	   1f-2b because the assembler generates worse code.  */
	leal	0xb(%edx,%eax,8), %ecx
#else
	leal	1f(,%eax,8), %ecx
	leal	(%ecx,%eax,4), %ecx
	leal	(%ecx,%eax), %ecx
#endif

	jmp	*%ecx
	.align 8
1:
	
	movzx	(%esi), %eax
	testb	%al,%al
	jz	L(string_one_is_zero)
	subb	(%esi,%edx),%al
	jnz	L(compute_return)
	incl	%esi
	
	movzx	(%esi), %eax
	testb	%al,%al
	jz	L(string_one_is_zero)
	subb	(%esi,%edx),%al
	jnz	L(compute_return)
	incl	%esi
	
	movzx	(%esi), %eax
	testb	%al,%al
	jz	L(string_one_is_zero)
	subb	(%esi,%edx),%al
	jnz	L(compute_return)
	incl	%esi

L(1):	
	movl	(%esi), %eax
	leal	magic(%eax), %ecx
	not	%eax
	andl	%eax,%ecx
	testl	$0x80808080,%ecx
	jnz	L(string_one_dw_has_zero)

	movzx	(%esi),%eax
	subb	(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	1(%esi),%eax
	subb	1(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	2(%esi),%eax
	subb	2(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	3(%esi),%eax
	subb	3(%esi,%edx),%al
	jnz	L(compute_return)

	leal	4(%esi),%esi
	jmp L(1)

L(string_one_is_zero):
	subb	(%esi,%edx),%al

L(compute_return):
	/* movl	$-1,%ecx
	movl	$0,%eax
	cmovbl	%ecx,%eax
	neg	%ecx
	cmoval	%ecx,%eax */

	/* JMZ FIXME: check high bounds  */
	/* JMZ FIXME: needs a glibc compatible ending */
	popl	%esi
	ret

L(string_one_dw_has_zero):
	movzx	(%esi),%eax
	subb	(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	1(%esi),%eax
	subb	1(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	2(%esi),%eax
	subb	2(%esi,%edx),%al
	jnz	L(compute_return)

	movzx	3(%esi),%eax
	subb	3(%esi,%edx),%al
	jmp L(compute_return)


