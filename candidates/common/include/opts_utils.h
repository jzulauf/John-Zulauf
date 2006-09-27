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
#ifndef _OPTS_UTIL_H_
#define _OPTS_UTIL_H_
/**
 * \file
 * \brief
 * Prototypes and macros for using \ref opts
 */
/** enables the long options processing in getopt.h */
#define _GNU_SOURCE
#include <getopt.h>

#include <limits.h>
/** The most options we can have.  Options > CHAR_MAX will have no short names*/
#define OPTS_MAX UCHAR_MAX
/** The highest valid ordinal for an option */
#define OPTS_LIM (OPTS_MAX + 1)

/** The option handling callback function type */
typedef int (*T_opts_action)(void *);

/**
 * \brief
 * Stores the information about a single option
 */
typedef struct {
	int   set;		/*!<Non-zero if this option been set */
	char *arg; 		/*!<The descriptive name of the argument
					(if any) */
	T_opts_action action;	/*!<The callback when this option is found */
	void *data;		/*!<The data passed to the action callback */
	char *desc;		/*!<Help text for this message %s for arg */
	struct option *lopt;	/*!<The longopt info, ->val == shortname */
} T_opts_def;

/* Opts API */
void opts_init(const char *progname, const char *progversion);
void opts_default_opts(); 
void opts_def(	char short_name, char *long_name,
		char *description, char *arg_name,
		T_opts_action action, void *data);
int opts_getopt(int argc, char *const *argv);
void opts_usage(void);

/* Convenience macros */
/** declares a option with no action, or argument. The calling program
 * must "catch" this option (don't do this unless you have to)
 */
#define OPTS_DEF(short_name,long_name,description)	\
	opts_def(short_name,long_name,description,NULL,	\
		NULL,NULL)
/** declares an option with no action.  The calling program
 * must catch this option (don't do this unless you have to)
 */
#define OPTS_DEF_ARG(short_name,long_name,description,arg_name)	\
	opts_def(short_name,long_name,description,arg_name,	\
		NULL,NULL)
/** declares an option which takes a "long" argument and stores it in v */
#define OPTS_DEF_STRTOL(short_name,long_name,description,arg_name,v)	\
	opts_def(short_name,long_name,description,arg_name,		\
		opts_action_strtol,(void *)&(v))
/** declares an option which sets v to 0 */ 
#define OPTS_DEF_CLEAR(short_name,long_name,description,v)	\
	opts_def(short_name,long_name,description,NULL,		\
		opts_action_clear,(void *)&(v))
/** declares an option which increments v */ 
#define OPTS_DEF_INCR(short_name,long_name,description,v)	\
	opts_def(short_name,long_name,description,NULL,		\
		opts_action_incr,(void *)&(v))
/** declares an option which sets v to 1 */ 
#define OPTS_DEF_SET(short_name,long_name,description,v)	\
	opts_def(short_name,long_name,description,NULL,		\
		opts_action_set,(void *)&(v))

/* Standard actions */
int opts_action_clear(void *ptr_to_long);
int opts_action_incr(void *ptr_to_long);
int opts_action_set(void *ptr_to_long);
int opts_action_strtol(void *ptr_to_long);
int opts_action_usage(void *ignored);
int opts_action_version(void *ignored);
/* end opts_util.h */

#endif
