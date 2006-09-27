This code was written by John Zulauf while at AMD.

This is two items:

1) a test framework for measuring performance of critical routines like
memcpy, memset or other basic routines worth optimizing for a particular
system, and

2) some different implementations of memcmp, memcpy, memset, strcmp, strcpy,
and strlen with results showing the performance differences of particular
possible implementations.

What is needed is to confirm these results on our board, and see the
results integrated upstream (e.g. glibc, the kernel).

Volunteers interested in performance work very welcome.
                      Jim Gettys, OLPC, 9/27/2006
