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
/**
 * \file
 * \brief
 * Implements the "Simple Approximate Statitistics Package"
 *
 */

#include "rdtsc.h"

/**
 * \brief
 * causes stats_* variables to be declared (allocated) in this file
 */
#define STATS_DECLARE_EXTERNS
#include "bucket_stats.h"


/**
 * \defgroup stats_static Static utility functions (internal)
 * \ingroup stats
 * \brief
 * static utility functions implementing Simple Approximate Statitistics Package
 */

/*@{*/

static int stats_verbose = 0;
static T_stats_range_def stats_range_defs[STATS_MAX_BUCKET_RANGE];

/**
 * \brief
 * Initializes the mode structure array
 * \param modes mode structure array (T_stats_modes)
 * \return void
 */
static void init_modes(T_stats_modes modes)
{
	memset(modes,0,sizeof(T_stats_modes));
}

/**
 * \brief
 * Updates the modes given a bucket's data
 * \param T_stats_modes modes	mode array to update
 * \param size_t	hits	the number of hits in the bucket
 * \param double	avg	the average value of the hits
 * \return void
 */
static void update_modes(T_stats_modes modes, size_t hits, double avg)
{
	int i,j;
	for (i = 0; i < STATS_NUM_MODES; i++) {
		if( hits >= modes[i].num ) {
			for ( j = STATS_NUM_MODES -1; j > i ; j--)
				modes[j] = modes[j-1];
			modes[i].num = hits;
			modes[i].avg = avg;
			break;
		}
	}
}

/**
 * \brief
 * Updates the modes given a bucket's data
 * \param T_stats_modes modes	mode array to update
 * \return double	weight average of the modes (except IGNORES)
 */
static double weighted_mode_avg(T_stats_modes modes)
{
	int i;
	double total=0.;
	size_t count=0;
	size_t tignore=0;
	/*
	 *
	 */
	for (i = 0; i < STATS_NUM_MODES; i++)
		tignore += modes[i].num;
	tignore  /= STATS_IGNORE_THRESHOLD;

	for (i = 0; i < STATS_NUM_MODES; i++) {
		if ( modes[i].num > tignore ) {
			total += modes[i].num*modes[i].avg;
			count += modes[i].num;
		} else {
			modes[i].ignored = 1;
		}
	}
	if (count)
		total = total/count;
	else
		total = 0;
	return total;
}
	
/** \brief
 *  prints out detailed statistics if verbosity set at 2 or more
 *  \param l	lower bucket bound
 *  \param u	upper bucket bound
 *  \param v	number of hits in bucket
 *  \param a	average value in bucket
 *  \return void
 */
static void stat_line(T_tick l, T_tick u, size_t v, double a)
{
	if ( stats_verbose > 1 )
		printf("STATS\t%6lld\t%6lld\t%6d\t%8.1lf\n",(l),(u),(v),(a));
}

/*@}*/

/**
 * \defgroup stats "stats_" Simple Approximate Statitistics Package
 * \brief
 * A simple statistics gathering library with the following features
 *
 *  - Wide dynamic range (up to 32 bits)
 *  - Constant precision (as a % of value)
 *  - Configurable accuracy
 *  - Mode computation and modal weighted averages
 *  - Mode "ignore" for lightly popluated modes
 *
 * and the following limitations
 *
 *  - only a single active dataset (this restriction could be removed)
 *  - does not track if "bucket,range" accumulation is exact (i.e. all
 *    values the same)
 *  - does not provide exact statistics with a given range
 *  - does not support mean, median, std. deviation etc. (only mode)
 */
/*@{*/
/**
 * \brief
 * Initializes the Simple Statistics Package, call once before using.
 * \return void
 */
void stats_init_package()
{
	T_stats_range_def *b;
	int i, j;
	
	/*
	 * Bucket zero is special, it covers the span where
	 * the size of the sub-buckets is 1
	 */
	b = &stats_range_defs[0];
	b->min = 0;
	b->max = STATS_BUCKETS - 1;
	b->shift = 0;
	for ( j = 0; j < STATS_BUCKETS; j++ ) {
		b->bmin[j]=j;
		b->bmax[j]=j;
	}
	
	for (i = 1; i < STATS_MAX_BUCKET_RANGE ; i++ ) {
		b = &stats_range_defs[i];
		b->shift = i-1;
		b->min = (1 << (STATS_BUCKET_SHIFT+i-1) );
		b->max = (1 << (STATS_BUCKET_SHIFT+i) ) -1;
		for ( j = 0; j < STATS_BUCKETS; j++ ) {
			b->bmin[j] = b->min + (j << b->shift);
			b->bmax[j] = b->min + ((j+1) << b->shift) - 1;
		}
	}

	b->max= ULLONG_MAX;
	b->bmax[STATS_BUCKETS-1]= ULLONG_MAX;
}


/**
 * \brief
 * Initializes the statistics gather record.  Call before each test run.
 * Erases all previous statitistics
 * \return void
 */
void stats_record_init(T_stats_record *rec)
{
	memset((void *)rec,0, sizeof(T_stats_record));
	rec->result.min = ULLONG_MAX;
}

/**
 * \brief
 * Allocates and initalizes statistics record
 * \return the created record
 */
T_stats_record *stats_record_create()
{
	T_stats_record *rec = malloc(sizeof(T_stats_record));
	stats_record_init(rec);
	return rec;
}

/**
 * \brief
 * Records one datapoint
 * \param rec the statistics record to update (T_stats_record *)
 * \param t value to record (T_tick)
 * \return void
 */
void stats_record_datapoint(T_stats_record *rec, T_tick t)
{
	/*
	 * put this statistic to the into the range and bucket
	 */
	T_stats_range_def *b = &stats_range_defs[0];
	int range=0;
	int bucket;
	/* Save the min value */
	if ( rec->result.min < t ) {
		rec->result.min = t;
	}
	/* update the bucket counts */
	/* TODO -- it is possible that the MSB logic is faster and less
	 * intrusive in both caches -- but this is clearer.  Need to determine
	 * whether it matters
	 */
	while ( t > b->max )  {
		b++;
		range++;
	}
	bucket = (t >> b->shift) & STATS_BUCKET_MASK;
	rec->count[range][bucket]++;
	rec->total[range][bucket] += t;
}


	
/**
 * \brief
 * Computes the modes and averages from the current statistics record
 * \param rec the statistics record to update (T_stats_record *)
 * \param count	the unit count for normalizing dwavg (size_t)
 * \param name	the name for verbose output (char *)
 * \param result where to put the computed values (T_stats_result *)
 * \return void
 */
void stats_record_compute(T_stats_record *rec,
			size_t count, char *name, T_stats_result *result)
{
	int i,j;
	double wavg;
	double dwavg;
	T_stats_record *stats = rec;
	init_modes(rec->modes);
	if ( stats_verbose )
		printf("STATS\tstart\tbytes=%d\tfunc=%s\n", count,name);
	for (i = 0; i < STATS_MAX_BUCKET_RANGE; i++ ) {
		T_stats_range_def *b = &stats_range_defs[i];
		for (j = 0; j < STATS_BUCKETS; j++ ) {
			size_t hits = rec->count[i][j];
			if ( hits  ) {
				double avg  = rec->total[i][j]/hits;
				stat_line(b->bmin[j],b->bmax[j], hits,avg);
				update_modes(rec->modes,hits,avg);
			}
		}
	}
	rec->result.wavg= weighted_mode_avg(rec->modes);
	if ( count ) {
		rec->result.dwavg = rec->result.wavg / (double)count;
	} else 
		rec->result.dwavg = rec->result.wavg;

	if (stats_verbose ) {
		for (i = 0; i < STATS_NUM_MODES &&
				rec->modes[i].num > 0; i++)  {
			printf("STATS\tmode\t%ld\t%ld\t%8.1lf",
				i, rec->modes[i].num,rec->modes[i].avg);
			if ( rec->modes[i].ignored ) {
				printf("\tIGNORED");
			}
			printf("\n");
		}
		printf("STATS\tmode_avg\t%8.1lf\n", rec->result.wavg);
	}
	if (stats_verbose) {
		printf("STATS\tsummary\t%s\t%ld\t%8.1lf\n", name, count,
			rec->result.dwavg);
	}

	/*
	 * Return the result if the user sends a ptr.
	 */
	if (result)
		*result = rec->result;
}

/**
 * \brief
 * Get the verbosity level
 * \return the verbosity level
 */
int  stats_get_verbose()
{
	return stats_verbose;
}

/**
 * \brief
 * Set the verbosity level
 * \param verbose the verbosity level to set
 */
void stats_set_verbose(int verbose)
{
	stats_verbose = verbose;
}

/**
 * \brief
 * Get the range defs
 * \return the range defs
 */
T_stats_range_def *stats_get_range_defs()
{
	return stats_range_defs;
}

/*@}*/
