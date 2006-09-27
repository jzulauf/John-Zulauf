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
APPS = deck_test table_test zstr_test
all : $(APPS)

inc_path= -I . -I $(PWD)/../common/include
common_dir=  ../common
include $(common_dir)/make.inc

utils_dir= $(common_dir)/utils
include $(utils_dir)/make.inc

#OPT = -O2
DEBUG =-g
CFLAGS= $(OPT) $(inc_path) $(DEBUG) -DDO_STATS -DPROG_DIR='$(prog_dir)' 
ASFLAGS= $(DEBUG) $(inc_path)

#
# The LGPL_OBJS need to be isolated in a library for licensing reasons
LGPL_OBJS = 
#LGPL_LIB = liblgpl.a
	
# Not a win lxu_strlen.o

liblgpl.a : $(LGPL_OBJS)
	$(AR) $(ARFLAGS) $@ $^ 

COMMON_OBJS =  $(utils_objs)


OBJS = $(addsuffix .o,$(APPS)) $(COMMON_OBJS) $(LGPL_OBJS)


deck_test : deck_test.o deck.o

table_test: table_test.o table.o zstring.o
zstr_test : zstr_test.o zstring.o

vars:
	echo $(prog_dir)

clean :
	rm -f $(OBJS) $(APPS) liblgpl.a

