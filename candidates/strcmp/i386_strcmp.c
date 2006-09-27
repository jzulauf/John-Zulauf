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
JMZ */

/* JMZ
 * extracted from sysdeps/i386/bits/string.h, renamed function
 */
int i386_strcmp (__const char *__s1, __const char *__s2)
{
  register unsigned long int __d0, __d1;
  register int __res;
  __asm__ __volatile__
    ("cld\n"
     "1:\n\t"
     "lodsb\n\t"
     "scasb\n\t"
     "jne	2f\n\t"
     "testb	%%al,%%al\n\t"
     "jne	1b\n\t"
     "xorl	%%eax,%%eax\n\t"
     "jmp	3f\n"
     "2:\n\t"
     "sbbl	%%eax,%%eax\n\t"
     "orb	$1,%%al\n"
     "3:"
     : "=a" (__res), "=&S" (__d0), "=&D" (__d1)
     : "1" (__s1), "2" (__s2),
       "m" ( *(struct { char __x[0xfffffff]; } *)__s1),
       "m" ( *(struct { char __x[0xfffffff]; } *)__s2)
     : "cc");
  return __res;
}
