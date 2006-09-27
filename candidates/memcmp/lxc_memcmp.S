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

/*JMZ
 *Renamed function
JMZ */
	.text
ENTRY (BP_SYM (lxc_memcmp))
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

	repe			/* Compare at most %ecx bytes.  */
	cmpsb

/* JMZ
 * Expirement: trade conditional jump 2 cmov's -- 0-1% slower on Geode LX
JMZ */
	mov    $1,   %ecx
	cmova  %ecx, %eax
	mov    $-1,  %ecx
	cmovb %ecx, %eax

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
