#
# Copyright (c) 2006, Advanced Micro Devices, Inc.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met:
# 
#    * Redistributions of source code must retain the above copyright 
#      notice, this list of conditions and the following disclaimer.
#    * Neither the name of Advanced Micro Devices, Inc. nor the names
#      of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written
#      permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
APPS = ptest utest
all : $(APPS)

inc_path= -I . -I $(PWD)/../common/include
common_dir=  ../common
include $(common_dir)/make.inc

utils_dir= $(common_dir)/utils
include $(utils_dir)/make.inc

OPT = -O2
#DEBUG = -g
COMMON_FLAG = $(DEBUG) $(inc_path)
CFLAGS= $(COMMON_FLAG) $(OPT)  -DDO_STATS -DPROG_DIR='$(prog_dir)' -DSTATS_NUM_MODES=20
ASFLAGS= $(COMMON_FLAG)
#LDFLAGS= -L. -llgpl

#
# The LGPL_OBJS need to be isolated in a library for licensing reasons
LGPL_OBJS =	i386_memset.o	\
		i586_memset.o	\
		i686_memset.o	\
		i686_memset_fix.o \
		lx_memset_zan.o	\
		lx_memset_simp.o \
		lx_memset_hyb.o	\
		lx_memset_hyb_BA.o	\
		lx_memset_mmx.o \
		lx_memset_mmx64.o \
		lx_memset_mmx64simp.o \
		lx_memset_mmx64_BA.o	\
		lx_memset_mmx_h2.o

liblgpl.a : $(LGPL_OBJS)
	$(AR) $(ARFLAGS) $@ $^ 

COMMON_OBJS =  $(utils_objs)


OBJS = $(addsuffix .o,$(APPS)) $(COMMON_OBJS) $(LGPL_OBJS)


ptest : ptest.o $(COMMON_OBJS) liblgpl.a

utest : utest.o $(COMMON_OBJS) liblgpl.a

vars:
	echo $(prog_dir)

clean :
	rm -f $(OBJS) $(APPS) liblgpl.a

