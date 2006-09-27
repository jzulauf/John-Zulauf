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
#ifndef _SIMPLE_STATS_H_
#define _SIMPLE_STATS_H_

/**
 * \file
 * \brief
 * Types and default settings for  \ref stats
 */

#include <stdlib.h>
#include "rdtsc.h"

/*
 * The size of the statistics can be set by compile flags, but these are
 * are the defaults
 */
#ifndef STATS_MAX_MSB
/** \def STATS_MAX_MSB \brief the largest msb used to address a bucket range */
#	define STATS_MAX_MSB 24
#endif
#ifndef STATS_BUCKET_SHIFT
/** \def STATS_BUCKET_SHIFT \brief number of bits < msb to address a bucket */
#	define STATS_BUCKET_SHIFT 4
#endif
#ifndef STATS_NUM_MODES
/** \def STATS_NUM_MODES \brief number of modes to use for weighted avg */
#	define STATS_NUM_MODES 16
#endif

/** \def STATS_IGNORE_THRESHOLD
 * \brief 
 * Any mode smaller that this (as a fraction of the total number of datapoints)
 * will be ignored in the computation of the weighted more averages
 *
 * \b NOTE: this is a fairly arbitrary number 100 and 1000 look as good
 *	 of choices, but this seemed like a way to catch the true
 *	 "flyers" without ignoring real data
 */
#ifndef STATS_IGNORE_THRESHOLD
#	define STATS_IGNORE_THRESHOLD 256
#endif

/** \def STATS_BUCKETS \brief number of buckets per range */
/** \def STATS_BUCKET_MASK \brief mask to look at just the bucket index */
/** \def STATS_MAX_BUCKET_RANGE \brief number of bucket ranges */

#define STATS_BUCKETS (1 << STATS_BUCKET_SHIFT )
#define STATS_BUCKET_MASK (STATS_BUCKETS - 1)
#define STATS_MAX_BUCKET_RANGE (STATS_MAX_MSB-STATS_BUCKET_SHIFT)

/**
 * \brief
 * Defines an msb range and bucket limits for easy hashing and reporting
 * All range values are \b inclusive.
 */
typedef struct  {
	T_tick min;			/*!< range minimum 2^msb */
	T_tick max;			/*!< range max 2^(msb+1) -1 */
	int  shift;			/*!< shift to compute bucket mask */
	T_tick  bmin[STATS_BUCKETS];	/*!< minimum for each bucket */
	T_tick  bmax[STATS_BUCKETS];	/*!< maximum for each bucket */
} T_stats_range_def;

/**
 * \brief
 * The result type of stats_compute_stats
 */
typedef struct {
	T_tick   min;	/*!<minimum average bucket value */
	double wavg;	/*!<weighted average of the non-ignored modes */
	double dwavg;	/*!<weighted average normalized by max(1,count) */
} T_stats_result;

/** \brief
 * defines the mode contents structure (internal)
 */
typedef struct {
	size_t num;	/*!< population of mode */
	double avg;	/*!< average value of mode */
	int    ignored; /*!< mode below ignore threshhold */
} T_stats_mode;

/** \brief
  * defines the mode array
  */ 
typedef T_stats_mode T_stats_modes[STATS_NUM_MODES];

/** \brief
  * Defines the statisics accumulation record. 
  * Cleared by stats_init_record().  
  * All fields valid after stats_compute_stats().
  */ 
typedef struct {
	size_t count[STATS_MAX_BUCKET_RANGE][STATS_BUCKETS]; /*!< number of hits in the range/bucket */
	T_tick   total[STATS_MAX_BUCKET_RANGE][STATS_BUCKETS]; /*!< sum of recorded datapoints, for averaging computation */
	T_stats_result result;	/*!< result from last stats_compute_stats(). */
	T_stats_modes  modes;   /*!< modes computed in stats_compute_stats(). */
} T_stats_record;

void stats_init_package();
T_stats_record *stats_record_create();
void stats_record_datapoint(T_stats_record *rec, T_tick t);
void stats_record_compute(T_stats_record *rec, size_t count, char *name,
			T_stats_result *result);
int stats_get_verbose();
void stats_set_verbose(int verbose);
T_stats_range_def *stats_get_range_defs();

#endif
