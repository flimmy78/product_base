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



/*!  \file   octeon_stats.h
     \brief  Host Driver: Statistics provided by the driver for different queues and
             buffer pool.
*/


#ifndef  __OCTEON_STATS_H__ 
#define  __OCTEON_STATS_H__ 

#include "octeon_config.h"

#define   OCTEON_BUFFER_POOL_STATS_ON      0x01
#define   OCTEON_CNTQ_STATS_ON             0x02


/** Statistics table for octeon device. */
typedef struct {

	/** Number of interrupts received. */
        uint64_t  interrupts; 

	/** Poll thread ran these many times. */
        uint64_t  poll_count; 

	/** Request completion tasklet ran these many times */
	uint64_t  comp_tasklet_count; 

	/** DROQ tasklet ran these many times */
	uint64_t  droq_tasklet_count;

	/** CNTQ tasklet ran these many times */
	uint64_t  cntq_tasklet_count;
} oct_dev_stats_t;



/** Buffer pool statistics. Each of the buffer pool has four stats fields.*/
typedef struct {

	/** Buffer created in this pool at init time. */
	uint32_t   max_count;

	/** Buffers allocated from this pool by driver. */
	uint32_t   alloc_count;

	/** Fragmented buffer count. */
	uint32_t   frag_count;

	/** Buffers moved to other pools. */
	uint32_t   other_pool_count;

} oct_bufpool_stats_t;

#define OCT_BUFPOOL_STATS_SIZE  (sizeof(oct_bufpool_stats_t))



/** Input Queue statistics. Each input queue has four stats fields. */
typedef struct {

  uint64_t   instr_posted; /**< Instructions posted to this queue. */
  uint64_t   instr_processed; /**< Instructions processed in this queue. */
  uint64_t   instr_dropped;   /**< Instructions that could not be processed. */
  uint64_t   bytes_sent;      /**< Bytes sent through this queue. */
  uint64_t   sgentry_sent;    /**< Gather entries sent through this queue. */

} oct_iq_stats_t;

#define OCT_IQ_STATS_SIZE   (sizeof(oct_iq_stats_t))





/** Output Queue statistics. Each output queue has four stats fields. */
typedef struct {

  uint64_t   pkts_received; /**< Number of packets received in this queue. */
  uint64_t   bytes_received;/**< Bytes received by this queue. */
  uint64_t   dropped_nodispatch; /**< Packets dropped due to no dispatch function. */
  uint64_t   dropped_nomem; /**< Packets dropped due to no memory available. */
  uint64_t   dropped_toomany; /**< Packets dropped due to large number of pkts to process. */
} oct_droq_stats_t;

#define OCT_DROQ_STATS_SIZE   (sizeof(oct_droq_stats_t))



/** Statistics for Octeon DMA Counter queues (CNTQ) */
typedef struct  {

	uint64_t   pkts_received;
	uint64_t   bytes_received;
	uint64_t   dropped_nodispatch;
	uint64_t   dropped_nomem;
	uint64_t   credits_posted;
	uint64_t   credits_failed;

} oct_cntq_stats_t;

#define OCT_CNTQ_STATS_SIZE    (sizeof(oct_cntq_stats_t))




/** Each DDOQ in the system stores its stats in this format. */
typedef struct {

	uint64_t  packets_recvd;
	uint64_t  multibuf_pkts_recvd;
	uint64_t  packets_dropped;

	uint64_t  bytes_received;
	uint64_t  buffers_recvd;
	uint64_t  credit_instr_cnt;
	uint64_t  credit_desc_cnt;
	uint64_t  credit_instr_failed;
	
	uint64_t  desc_pending_refill;
	uint64_t  desc_pending_credit;
	uint64_t  pkts_pending;
	uint64_t  state;

} oct_ddoq_stats_t;

#define OCT_DDOQ_STATS_SIZE    (sizeof(oct_ddoq_stats_t))





/** System-wide statistics for DDOQ implementation. */
typedef struct {

	uint64_t   ddoqs_created;
	uint64_t   create_failed;
	uint64_t   active_ddoqs;
	uint64_t   ddoqs_pending_eot;
	uint64_t   del_b4_crt;

} oct_ddoq_list_stats_t;

#define OCT_DDOQ_LIST_STATS_SIZE    (sizeof(oct_ddoq_list_stats_t))




/** Octeon device statistics */
typedef struct {

	uint64_t              magic; /**< Magic word to verify correctness of structure. */
	uint8_t               oct_id;
	uint8_t               debug_level; /**< The current debug level. */
	uint16_t              components; /**< Indicates what components are present. */
	uint16_t              reserved;
	char                  dev_state[32]; /** < Current state of the octeon device. */
	oct_bufpool_stats_t   bufpool[BUF_POOLS]; /**< The buffer pool statistics. */
	oct_iq_stats_t        iq[32]; /**< The input queue statistics. */
	oct_droq_stats_t      droq[32]; /**< The output queue statistics. */
	oct_cntq_stats_t      cntq[MAX_OCTEON_DMA_QUEUES]; /**< DMQ Counter queue statistics. */
	oct_ddoq_list_stats_t ddoq_list; /**< DDOQ system-wide statistics */

} oct_stats_t;

#define  OCT_STATS_SIZE   (sizeof(oct_stats_t))




#define CAVIUM_STATS_MAGIC  0x1111222233334444ULL

#endif  /* __OCTEON_STATS_H__ */


/* $Id: octeon_stats.h 59212 2011-04-14 18:46:46Z panicker $ */

