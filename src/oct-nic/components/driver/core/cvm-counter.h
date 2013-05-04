/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */

#ifndef  __CVM_COUNTER_H__
#define  __CVM_COUNTER_H__

#define  ENABLE_PROFILING

/* \cond */
typedef struct {
	char        name[32];
	uint64_t    max;
	uint64_t    min;
	uint64_t    total;
	uint64_t    updates;
} cvm_counter_t;

/* \endcond */


#ifdef  ENABLE_PROFILING
static inline void
cvm_counter_update(cvm_counter_t  *s, uint64_t  val64)
{
	if(val64 > s->max)
		s->max = val64;
	if(val64 < s->min)
		s->min = val64;
	s->total += val64;
	s->updates ++;
	CVMX_SYNCW;
}
#else
#define cvm_counter_update(s, val64)  do { }while(0)
#endif


static inline void
cvm_counter_init(cvm_counter_t   *s, char *name)
{
	if(strlen(name))
		strcpy(s->name, name);
	else
		s->name[0] = 0;
	s->max = s->total = s->updates = 0;
	s->min = (uint64_t)-1;
}



#ifdef  ENABLE_PROFILING
static inline void
cvm_counter_print(cvm_counter_t  *s)
{
	printf("%s\n  Updates: %llu Total: %llu Max: %llu  Min: %llu Avg: %llu\n",
		 s->name, CAST64(s->updates), CAST64(s->total), CAST64(s->max),
		 (s->updates)?CAST64(s->min):0, (s->updates)?CAST64(s->total/s->updates):0);
}
#else
#define cvm_counter_print(s)  do { }while(0)
#endif



#endif


/* $Id: cvm-counter.h 43039 2009-06-03 00:51:33Z panicker $ */


