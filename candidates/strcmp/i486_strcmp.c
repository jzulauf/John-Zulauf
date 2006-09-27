/* Copyright (C) 1991, 1996, 1997, 2003 Free Software Foundation, Inc.
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
 * Modifications for performance testing
 * 2006 John M. Zulauf Advanced Micro Devices, Inc
 * john.zulauf@amd.com
 *
 * Extracted from sysdeps/i386/i486/bits/string.h, renamed function
JMZ */
int i486__strcmp_gg (__const char *__s1, __const char *__s2)
{
  register int __res;
  __asm__ __volatile__
    ("1:\n\t"
     "movb	(%1),%b0\n\t"
     "leal	1(%1),%1\n\t"
     "cmpb	%b0,(%2)\n\t"
     "jne	2f\n\t"
     "leal	1(%2),%2\n\t"
     "testb	%b0,%b0\n\t"
     "jne	1b\n\t"
     "xorl	%0,%0\n\t"
     "jmp	3f\n"
     "2:\n\t"
     "movl	$1,%0\n\t"
     "jb	3f\n\t"
     "negl	%0\n"
     "3:"
     : "=q" (__res), "=&r" (__s1), "=&r" (__s2)
     : "1" (__s1), "2" (__s2),
       "m" ( *(struct { char __x[0xfffffff]; } *)__s1),
       "m" ( *(struct { char __x[0xfffffff]; } *)__s2)
     : "cc");
  return __res;
}
