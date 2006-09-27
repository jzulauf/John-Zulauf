/* Optimized, inlined string functions.  i486 version.
   Copyright (C) 1997,1998,1999,2000,2001,2002,2003,2004
   	Free Software Foundation, Inc.
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
 * extracted from sysdeps/i386/i486/bits/string.h 
 *
 * Modifications for performance testing John M. Zulauf
 * Copyright (C) 2006 Advanced Micro Devices, Inc
JMZ */

/*JMZ
 *Renamed function
JMZ */
i486_inline_strlen (__const char *__str)
{
  register char __dummy;
  register __const char *__tmp = __str;
  __asm__ __volatile__
    ("1:\n\t"
     "movb	(%0),%b1\n\t"
     "leal	1(%0),%0\n\t"
     "testb	%b1,%b1\n\t"
     "jne	1b"
     : "=r" (__tmp), "=&q" (__dummy)
     : "0" (__str),
       "m" ( *(struct { char __x[0xfffffff]; } *)__str)
     : "cc" );
  return __tmp - __str - 1;
}
