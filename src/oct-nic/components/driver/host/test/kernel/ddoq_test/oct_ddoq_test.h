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

#ifndef  __OCT_DDOQ_TEST_H__
#define  __OCT_DDOQ_TEST_H__

#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "cavium_release.h"
#include "octeon-common.h"
#include "octeon-cntq-common.h"
#include "octeon_cntq_defs.h"

/* ID of octeon device on which to perform the test. */
#define   OCTEON_ID               0


/* Enable this flag to do a packet input test for DDOQ. Only 1 DDOQ is created
   for packet input test.
   This flag is NOT enabled by default. The test program performs DDOQ Create
   & Delete operations when this flag is not set.
*/
//#define   DDOQ_PKT_INPUT_TEST

#ifdef   DDOQ_PKT_INPUT_TEST
#define   MAX_DDOQS      1
#else
#define   MAX_DDOQS      (2 * 1024)
#endif



/* DDOQ Setup parameters */
#define   DT_DESC_COUNT           100
#define   DT_BUF_SIZE             1500
#define   DT_REFILL_THRESHOLD     (DT_DESC_COUNT >> 2)
#define   DT_INITIAL_REFILL       DT_REFILL_THRESHOLD


/* Delete checks the timestamp on each ddoq node and deletes the ddoq when
   the DDOQ was created beyond this time interval. */
#define   DT_CREATE_WAIT_TIME     1


/* Count of packets received and ddoq's created is printed at this interval.*/
#define   DISPLAY_INTERVAL       (10 * HZ)



/* Values used for status field in test_ddoq_info structure. */
#define   DT_STATE_CREATE         1
#define   DT_STATE_DELETE         2

struct ddoq_global_stats{
	uint64_t   total_pkts;
	uint64_t   total_bytes;
	uint64_t   error_pkts;
	
	unsigned long   last_pkt_recv_time;
};



/*  Each test node in the system carries ddoq-specific information in this
    structure. */
struct test_ddoq_info {

    int            ddoq_id;
    int            status;
    unsigned long  prev_time;
    unsigned long  create_time;
    uint64_t       prev_pkt_cnt;
    uint64_t       prev_byte_cnt;
    uint64_t       pkt_cnt;
    uint64_t       byte_cnt;
};


/* Each test node in the system has this format. The node field should always
   be first and is used for linked list management. Application specific
   fields follow the node field. */
struct ddoq_test_node {

	struct test_node       node;
	struct test_ddoq_info  ddoq;

};




/* There are 5 different lists in the system used to store a node in between
   different operations.
*/
struct ddoq_test_list {

	struct test_list  *il;    /* Init List */
	struct test_list  *cl;    /* Create List */
	struct test_list  *dl;    /* Delete List */
	struct test_list  *dcil;  /* Delete Confirmation Init List */
	struct test_list  *dcl;   /* Delete Confirmation List */

};


#endif

