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
#include <unistd.h>
#include <stdio.h>
#include "opts_utils.h"

/**
 * \file
 * \brief
 * Implements a callback based command line options handler wrapping
 * getopt_long
 */


/**
 * \defgroup opts_static Static variables and functions (internal)
 * \ingroup opts
 */
/*@{*/
/** \var holds the options data */
static T_opts_def opts_table[OPTS_LIM];
/** \var keeps track of the next available entry in \ref opts_long */
static int opts_count = 0;
/** \var the getopt_long native data */
static struct option opts_long[OPTS_LIM];
/** \var the list of options in getopt native form */
static char opts_string[2*OPTS_LIM];
/** \var points to the NULL terminator of \ref opts_string */
static char *opts_str_ptr = opts_string;
/** \var the default program name for usage and version */
static const char *opts_progname = "BOGUS_programname";
/** \var the default program version for usage and version */
static const char *opts_progversion = "BOGUS_version";
/** \var have we initialized the subsystem -- we should really check this */
static int opts_initialized = 0;

/**
 * \brief print the usage information for one options
 * \param c the short name that uniquely indentifies the option
 */
static void opts_print_option(char c)
{
	T_opts_def *opt = &opts_table[c];

	if ( c == ':' ) 
		return;
	if ( !opt->desc ) {
		printf("PROGAMMER ERROR: no opts_table entry for %c\n",c);
		return;
	}
	if ( opt->arg ) {
		printf("\t-%c %-8s\n",c,opt->arg);
		printf("\t--%s %-8s\n\t\t",opt->lopt->name,opt->arg);
		printf(opt->desc,opt->arg);
	} else {
		printf("\t-%c\n",c);
		printf("\t--%s\n\t\t",opt->lopt->name);
		printf(opt->desc);
	}
	printf("\n");
}

/**
 * \brief print error information for and option
 * \param c the option that caused the error
 */
static int opts_report_error(int c)
{
	if (isprint (optopt))
		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
		fprintf (stderr, "Unknown option character `\\x%x'.\n",
				optopt);
}

/*@}*/

/**
 * \defgroup opts "opts_" Callback based command line options handler
 * \brief
 * This API wraps the getopt_long functionality and provides a general
 * call back mechanism to define the behavior of command line flags. 
 *
 * \b TO \b USE:
 *	- opts_init() to initialize the subsystem 
 *	- opts_default_opts() to add useful default opts (help and version)
 *	- Define the arguments using:
 *		- opts_def()
 *		- #OPTS_DEF 
 *		- #OPTS_DEF_ARG, 
 *		- #OPTS_DEF_CLEAR 
 *		- #OPTS_DEF_INCR 
 *		- #OPTS_DEF_SET 
 *		- #OPTS_DEF_STRTOL 
 *	- opts_getopt() parses the known options
 *		- -1 end of options (no error)
 *		- Anything else, and unknown flag either
 *			- catch in a traditional getopts switch such as
 * \code
 *       while ((c = opts_getopt (argc, argv)) != -1) {
 *               switch (c) {
 *               // add stuff opts_getopt() can't handle
 *               case 'Z':
 *                       Z_option_handler();
 *                       break;
 *               // end of stuff opts_utils can't handle 
 *               default:
 *                       opts_usage();
 *               }
 *       }
 * \endcode
 *			- opts_usage() to display usage message and exit.
 *
 * \b TODO
 *	- Add optional argument support
 *	- Allow use of non-printable shortnames for longname only options
 */

/*@{*/
/**
 * \brief initialize the \ref opts subsystem.  Call this before calling
 * \return 0 for success non-zero for failure
 */
void opts_init(const char *progname, const char *progversion) {
	/* JMZ ... getopts_long doesn't need opt_err? hmmm... */
	/*opt_err = 0; */
	opts_initialized = 1;
	opts_progname    = progname;
	opts_progversion = progversion;
	bzero(opts_table,sizeof(opts_table));
	bzero(opts_long,sizeof(opts_long));
	bzero(opts_string,sizeof(opts_string));
}

/**
 * \brief
 * add the given option to the \ref opts_table
 * \param short_name the 1 character that uniquely identifies this option
 * \param long_name the long (no spaces) name for this option.  Must be
 * \b non-NULL.
 * \param description descriptive text for this option. If arg_name is
 * non-NULL, \%s in description will insert arg_name.
 * \param arg_name the descriptive name of the argument the option
 * takes (if an option takes an argument), or NULL if it doesn't.
 * \param action the callback action if the flag is found
 * \param data	pointer to the data the callback will need.
 */
void opts_def(	char short_name, char *long_name,
		char *description, char *arg_name,
		T_opts_action action, void *data)
{
	T_opts_def *def = &opts_table[short_name];
	def->set  = 1;
	def->arg  = arg_name;
	def->action = action;
	def->data = data;
	def->desc = description;
	def->lopt = &opts_long[opts_count++];
	def->lopt->name = long_name;
	def->lopt->has_arg = (arg_name) ? required_argument: no_argument;
	def->lopt->flag = NULL;
	def->lopt->val = short_name;
	*(opts_str_ptr++) = short_name;
	if ( arg_name )
		*(opts_str_ptr++) = ':';
}

/**
 * \brief
 * add the default opts to \ref opts_table
 */
void opts_default_opts() {
	opts_def('h',"help","show this message",NULL,opts_action_usage,NULL);
	opts_def('V',"version","Output the version and exit",NULL,
		opts_action_version,NULL);
}


/**
 * \brief
 * parse the known options, calling their callback actions, and returning
 * on unknown options with the identical semantics of getopt
 * \param argc the argument count
 * \param argv the argument array
 * \return -1 for end of arguments, anything else for an unknown options
 */
int opts_getopt (int argc, char *const *argv) {
	int index;
	T_opts_def *def;
	int c;

	/* We'll loop on this as long as we know what to do */
	while ((c= getopt_long(argc,argv,opts_string,opts_long,&index)) != 1 ) {
		if ( c > OPTS_MAX ) 
			return c;	/* Option is not a character, hmmm? */
		def =  &opts_table[c];
		if ( !def->action )
			return c;	/* opts has no action to take */
		if ( def->action(def->data) )
			return c;	/* action didn't work, give caller a
					   chance to catch */
		if ( '?' == c ) {
			opts_report_error(c);
			return c;
		}
	}
}

/**
 * \brief output the usage message and exit
 */
void opts_usage(void) {
	char *c = opts_string;

	printf("Usage:\n%s",opts_progname);
	while (*c) {
		if ( *c != ':' ) {
			printf(" -%c",*c);
			if ( opts_table[*c].arg )
				printf(" %s", opts_table[*c].arg);
		}
		c++;
	}

	c = opts_string;
	printf("\n\nDescription\n");
	while (*c) {
		opts_print_option(*c);
		c++;
	}
	exit(1);
}

/*@}*/

/**
 * \defgroup opts_actions Standard callback actions 
 * \ingroup opts
 */

/*@{*/
/**
 * \brief callback action to zero a long value if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_clear(void *ptr_to_long)
{
	long *i = (long *)ptr_to_long;
	if (!i) 
		return 1;
	*i = 0;
	return 0;
}

/**
 * \brief callback action to increment a long value if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_incr(void *ptr_to_long)
{
	long *i = (long *)ptr_to_long;
	if (!i) 
		return 1;
	*i = (*i) + 1;
	return 0;
}


/**
 * \brief callback action to set to 1 a long value if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_set(void *ptr_to_long)
{
	long *i = (long *)ptr_to_long;
	if (!i) 
		return 1;
	*i = 1;
	return 0;
}

/**
 * \brief callback action to set long value to the supplied argument
 * if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_strtol(void *ptr_to_long)
{
	long *i = (long *)ptr_to_long;
	if (!i) 
		return 1;
	*i = strtol(optarg, NULL,0);
	return 0;
}

/**
 * \brief callback action display the usage message if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_usage(void *ignored) {
	opts_usage();
	return 0;
}

/**
 * \brief callback action display the usage version message if a flag is present
 * \return 0 for success non-zero for failure
 */
int opts_action_version(void *ignored) {
	printf("%s: Version %s\n",opts_progname, opts_progversion);
	exit(0);
	return 0;
}

/*@}*/
