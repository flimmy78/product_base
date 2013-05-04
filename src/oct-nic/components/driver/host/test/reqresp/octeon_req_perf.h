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

#ifndef  __OCTEON_REQ_PERF_H__
#define  __OCTEON_REQ_PERF_H__

#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "octeon-opcodes.h"
#include "oct_test_list.h"

#ifndef __KERNEL__
#include "octeon_user.h"
#endif

//#define  DEBUG_ON

#define  ENFORCE_CONSTRAINTS

#define  VERIFY_DATA

#define  MAX_REQUESTS           256

#define  MAX_PENDING_REQUESTS   (MAX_REQUESTS/2)

#define  MAX_FAILURES           16

#define  REQUEST_TIMEOUT        500

#define  MAX_LOOP_COUNT         1000000


#ifdef  __KERNEL__   /* Kernel mode definitions */

#ifdef   DEBUG_ON
#define  __print(format, ...)   printk(format, ## __VA_ARGS__)
#else
#define  __print(format, ...)   do{ } while(0)

#endif

#else

#ifdef   DEBUG_ON
#define  __print(format, ...)   printf(format, ## __VA_ARGS__)
#else
#define  __print(format, ...)   do{ } while(0)
#endif

#endif

struct test_pkt {

	struct test_node         node;

	int                      pkt_no;

	volatile int             pkt_state;

	volatile int             send_state;

	int                      reqdatasize;

	octeon_soft_request_t    r;

	octeon_request_info_t    rinfo;

	struct test_list        *tl;
};




struct __test_info {

	int        dma_mode;

	int        resp_order;

	int        resp_mode;

	int        octeon_id;
	int        iq_no;
	int        pkt_cnt;

	int        datagen;
	int        min_bufs;
	int        max_bufs;
	int        min_datasize;
	int        max_datasize;

};



struct __unit_test_stats {

	uint64_t   mindata;
	uint64_t   maxdata;
	uint64_t   maxbufs;
	uint64_t   minbufs;
	uint64_t   dmacnt[4];
	uint64_t   send_passed;
	uint64_t   send_failed;
	uint64_t   resp_passed;
	uint64_t   resp_timeout;
	uint64_t   resp_error;
};



struct __test_stats {
	struct __unit_test_stats   u_b;
	struct __unit_test_stats   u_nb;
	struct __unit_test_stats   o;
	struct __unit_test_stats   nr;
};


struct __sequence {
	int   curr;
	int   incr;
};

struct __seq_params {
	struct __sequence   bufcnt;
	struct __sequence   datasize;
};




#define  UPDATE_SEND             1
#define  UPDATE_RESP             2

#define  TREQ_PKT_UNUSED         0xff
#define  TREQ_PKT_USED           1
#define  TREQ_SEND_PENDING       2
#define  TREQ_SEND_SUCCESS       4
#define  TREQ_SEND_FAILED        8
#define  TREQ_RESP_EXPECTED      16

#define  RANDOM                  (-1)
#define  SEQUENTIAL              (-2)



#endif

/* $Id: octeon_req_perf.h 53786 2010-10-08 22:09:32Z panicker $ */

