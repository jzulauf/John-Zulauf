/* Compare two memory blocks for differences in the first COUNT bytes.
   Copyright (C) 1995, 1996, 1997, 2000, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
 * Modifications for performance testing and optimization John M. Zulauf
 * Copyright (C) 2006 Advanced Micro Devices, Inc
JMZ */

#include <sysdep.h>
#include "asm-syntax.h"
#include "bp-sym.h"
#include "bp-asm.h"

#define PARMS	LINKAGE+4	/* space for 1 saved reg */
#define BLK1	PARMS
#define BLK2	BLK1+PTR_SIZE
#define LEN	BLK2+PTR_SIZE

/* JMZ
 *Renamed function
JMZ */
	.text
ENTRY (BP_SYM (lxd_memcmp))
	ENTER

	pushl %esi		/* Save callee-safe registers.  */
	movl %edi, %edx		/* Note that %edx is not used and can
				   so be used to save %edi.  It's faster.  */

	movl BLK1(%esp), %esi
	movl BLK2(%esp), %edi
	movl LEN(%esp), %ecx
	CHECK_BOUNDS_LOW (%esi, BLK1(%esp))
	CHECK_BOUNDS_LOW (%edi, BLK2(%esp))

	cld			/* Set direction of comparison.  */

	xorl %eax, %eax		/* Default result.  */

/* JMZ
 * Optimization: Compare memory dword at a time
JMZ */
	shrl $2, %ecx
	repe			/* Compare at most %ecx/4 dwords  */
	cmpsl
	jz L(remainder)

	/* We found a mismatch in the last dword  */
	/* Rewind the dword and do a cmpsb */
	subl $4, %esi
	subl $4, %edi
	movl $4, %ecx
	jmp L(bytewise_cmp)


L(remainder):
	/* Do the remaining bytes */
	movl LEN(%esp), %ecx	/* reload %ecx */
	andl $3, %ecx

/* JMZ
 * End Optimization
JMZ */
L(bytewise_cmp):
	repe			/* Compare at most %ecx bytes.  */
	cmpsb
	jz L(1)			/* If even last byte was equal we return 0.  */

/* JMZ
 * Experimental: Use cmov to set the return code (not fully optimized)
JMZ */
	mov    $1,   %ecx
	cmova  %ecx, %eax
	mov    $-1,  %ecx
	cmovb %ecx, %eax
#if 0 
/* JMZ
 * this is the orignal retcode approach 
JMZ */
	/* The memory blocks are not equal.  So result of the last
	   subtraction is present in the carry flag.  It is set when
	   the byte in block #2 is bigger.  In this case we have to
	   return -1 (=0xffffffff), else 1.  */
	sbbl %eax, %eax		/* This is tricky.  %eax == 0 and carry is set
				   or not depending on last subtraction.  */

	/* At this point %eax == 0, if the byte of block #1 was bigger, and
	   0xffffffff if the last byte of block #2 was bigger.  The latter
	   case is already correct but the former needs a little adjustment.
	   Note that the following operation does not change 0xffffffff.  */
	orb $1, %al		/* Change 0 to 1.  */
#endif

L(1):	CHECK_BOUNDS_HIGH (%esi, BLK1(%esp), jbe)
	CHECK_BOUNDS_HIGH (%edi, BLK2(%esp), jbe)
	popl %esi		/* Restore registers.  */
	movl %edx, %edi

	LEAVE
	ret
END (BP_SYM (memcmp))

#undef bcmp
weak_alias (BP_SYM (memcmp), BP_SYM (bcmp))
libc_hidden_builtin_def (BP_SYM (memcmp))
