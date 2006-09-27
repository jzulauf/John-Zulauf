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
# vi:syntax=make
#
# Include this to get the Makefile rules for the utils
bucket_stats.o : $(utils_dir)/bucket_stats.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

opts_utils.o : $(utils_dir)/opts_utils.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

test_utils.o : $(utils_dir)/test_utils.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

fake_lib.o : $(utils_dir)/fake_lib.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

deck.o : $(utils_dir)/deck.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

loop_test.o : $(utils_dir)/loop_test.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

table.o : $(utils_dir)/table.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

zstring.o : $(utils_dir)/zstring.c
	 $(COMPILE.c) $(OUTPUT_OPTION) $<

utils_objs =	test_utils.o bucket_stats.o opts_utils.o fake_lib.o \
		deck.o loop_test.o table.o zstring.o

utils: $(utils_objs)

