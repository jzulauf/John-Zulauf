#!/bin/bash
for i  in 0 1 2 3 4 5 6 7 8 9  10 11 12 13 14 15 16 17 18 19  20 21 22 23 24 25 26 27 28  ; do
echo $i
rm lx_strcpy_r4.o
make CMDLINE="-DDO_PRE$i" ptest
sleep 1
time nice -n 20 ptest  -s0 -e32  -FACc   -t400000 -T 0x75 -b 1 -R > perm_results/pre_${i}.txt

done
