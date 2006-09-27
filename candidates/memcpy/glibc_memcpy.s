/* 
	This file was created by disassembly of the i586 memcpy function
	the orginal source (and thus this file) is LGPL'd.  Function was
	renamed to allow for test

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

	.file "glibc_memcpy.s"
	.section .text
	.p2align 2,,3
.globl glibc_memcpy
	.type glibc_memcpy, @function
glibc_memcpy:
# Function header (no push/pop or frame)
	movl	0xc(%esp),%ecx
	movl	%esi, %eax
	movl	0x8(%esp),%esi
	movl	%edi, %edx
	movl	0x4(%esp),%edi

# glibc basic memcpy: move the byte, word, and dwords
	cld
# move a byte
	shrl	$1, %ecx
	jae	no_byte
	movsb
no_byte:
# move a word
	shrl	$1, %ecx
	jae	no_word
	movsw
no_word:
# move the dwords
	rep
	movsl

# Function footer: restore registers and set return value
	movl	%eax, %esi
	movl	%edx, %edi
	movl	0x4(%esp), %eax

	ret
	
