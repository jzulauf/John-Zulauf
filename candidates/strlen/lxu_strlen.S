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
   Additional logic implements cachline alignement for the inner 32byte
   loop.

   The LX has neither superscalar, nor speculative execution.  We unroll
   the loop to allow for prefetch to avoid cacheline related stalls.  */



/* These two must add to 32 (cacheline size */
#define PRE_UNROLL $22
#define UNROLL_EXTRA $10 

/* Must be PRE_UNROLL -1 */
#define PRE_UNROLL1 $21
#define FIRST_UNROLL $42

#define PARMS	LINKAGE		/* no space for saved regs */
#define STR	PARMS

/*JMZ
 *Renamed function
JMZ */
	.text
ENTRY (BP_SYM (lxu_strlen))
	ENTER

	pushl %edi
	movl STR+4(%esp), %edi
	CHECK_BOUNDS_LOW (%edi, STR(%esp))

	/* set up the rep */
	cld
	xorl %eax, %eax

	/* find the length to the end of the cacheline minus the pad */
	movl $0x1f, %edx
	andl %edi, %edx
	movl PRE_UNROLL, %ecx	
	subl %edx, %ecx
	jle 1f

	/* Do the pre unroll */
	repnz
	scasb
	jz 4f

1:	movl UNROLL_EXTRA, %edx
	addl %edx, %ecx
	jmp 3f

2:
	movl $32, %ecx
3:
	prefetch 32(%edi)
	repnz
	scasb
	jnz 2b

4:	
	movl %edi, %eax
	movl STR+4(%esp), %edi
	subl %edi, %eax
	decl %eax
	popl %edi

	/* TODO figure out the right thing here for check bounds */
	CHECK_BOUNDS_HIGH (%eax, STR(%esp), jb)
	LEAVE
	ret
END (BP_SYM (strlen))
libc_hidden_builtin_def (strlen)
