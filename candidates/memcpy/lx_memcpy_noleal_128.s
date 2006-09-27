/*
 * Copyright (c) 2006, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 *    * Redistributions of source code must retain the above copyright 
 *      notice, this list of conditions and the following disclaimer.
 *    * Neither the name of Advanced Micro Devices, Inc. nor the names
 *      of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
# Author: John M. Zulauf, Advanced Micro Device, Inc.
# This is an derived version of lx_memcpy_noleal 
#	Unroll is 128 bytes (4 cachelines)
#	Has too many prefetches, uses 4, but the Geode GX and LX have
#	only 3 prefetch slots
	.file "lx_memcpy_noleal_128.s"
	.section .text
	.p2align 2,,3
.globl lx_memcpy_noleal_128
	.type	lx_memcpy_noleal_128, @function
lx_memcpy_noleal_128:

# Function header (simplified recognizing the actual register impacts)
	pushl	%esi
	movl	%edi, %edx

# Setup the rep move and grab the 1st two cachelines
# note the offset are off by 4 w/ the ebx push above
	movl	0x08(%esp), %edi
	movl	0x0c(%esp), %esi
	movl	0x10(%esp), %ecx
	cld
# We are ready for simple_copy *HERE*

	prefetch	(%esi)
	prefetch	0x20(%esi)
	prefetch	0x40(%esi)
	
#Compute the number of 128 byte copies to do
# -- for < 128 bytes, only simple_copy runs
	movl	%ecx, %eax
	shrl	$7, %eax
	je	simple_copy # bail if none

# Note %edi, %esi, and %ecx are set up for a repmov here 
	prefetch	0x80(%esi)
	prefetch	0xA0(%esi)
	prefetch	0xC0(%esi)
				# ensure we have at least two (alignment)
	pushl	%ecx	# save a copy of the length for remainder


	.p2align 2,,3		# dword align loop
two_cacheline_loop:
	prefetch	0xE0(%esi)
	prefetch	0x100(%esi)
	movl	$0x20, %ecx 
	prefetch	0x120(%esi)
	prefetch	0x140(%esi)
	rep
	movsl
	
	decl	%eax
	jne	two_cacheline_loop

	# restore the count at the start of cacheline_aligned
	popl	%ecx
	andl	$0x7F, %ecx
	je	common_return # bail on zero

	.p2align 2,,3		# dword align simple_copy
simple_copy:
# Move the words then move the bytes	
# Assumes %ecx, %edi, and %esi are good to go
	movb	%cl, %al
	shrl	$2,%ecx
	rep
	movsl
	movb	 %al, %cl
	andb	$3, %cl
	rep
	movsb

common_return:
# need to set eax to dest
	popl %esi
	movl %edx, %edi
	movl 0x4(%esp),%eax #set the return to dst
	ret
