/* strlen -- Compute length of NUL terminated string.
   Highly optimized version for AMD Geode LX.
   Copyright (C) 1995,1996,1997,2000,2002,2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper, <drepper@gnu.ai.mit.edu>.

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

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

/* The version is modified to perform optimally on the AMD Geode (tm) LX
   processor.  This is mainly done by utilizing the prefetch and repne.

   The LX has neither superscalar, nor speculative execution.  We unroll
   the loop to allow for prefetch to avoid cacheline related stalls.  */



#define PARMS	LINKAGE		/* no space for saved regs */
#define STR	PARMS

/*JMZ
 *Renamed function
JMZ */
	.text
ENTRY (BP_SYM (lxus_strlen))
	ENTER

	pushl %edi
	movl STR+4(%esp), %edi
	CHECK_BOUNDS_LOW (%edi, STR(%esp))

	/* set up the rep */
	cld
	xorl %eax, %eax /* we are looking for zero */
	mov %edi,%edx /* Save a copy of the starting address */
2:
	movl $32, %ecx
	prefetch 32(%edi) /* Load the next cl */
	repnz
	scasb
	jnz 2b

	/* Compute the size */
	subl %edx, %edi
	decl %edi
	mov  %edi, %eax
	popl %edi

	/* TODO figure out the right thing here for check bounds */
	CHECK_BOUNDS_HIGH (%eax, STR(%esp), jb)
	LEAVE
	ret
END (BP_SYM (strlen))
libc_hidden_builtin_def (strlen)
