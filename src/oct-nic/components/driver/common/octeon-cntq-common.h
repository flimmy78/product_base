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


/*!  \file  octeon-cntq-common.h
     \brief Common: Structures and macros used by both core and host driver for
                    Counter Queue (CNTQ) operations. 
*/


#ifndef  __OCTEON_CNTQ_COMMON_H__
#define  __OCTEON_CNTQ_COMMON_H__

#include "octeon-common.h"

#define CNTQ_DISABLE  0
#define CNTQ_ENABLE   0x11000011


/*------------------------   CNTQ ------------------------------*/
/** Format of the first word of a CNTQ instruction */ 
typedef union {
   struct  {
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
       uint64_t    q_no:1;
       uint64_t    reserved2:7;
       uint64_t    q_size:16;
       uint64_t    reserved:40;
#else
       uint64_t    reserved:40;
       uint64_t    q_size:16;
       uint64_t    reserved2:7;
       uint64_t    q_no:1;
#endif
  } s;  
  uint64_t    u64;
}oct_cntq_init_word0_t;




/** Format of a CNTQ INIT instruction */
typedef struct {

	oct_cntq_init_word0_t   word0;
	uint64_t                base_addr;
	uint64_t                host_resp_addr;

} oct_cntq_init_instr_t;

#define  OCT_CNTQ_INIT_INSTR_SIZE  (sizeof(oct_cntq_init_instr_t))


/* CNTQ CREDIT & DELETE instructions consist of first word (see above) only. */

typedef oct_cntq_init_word0_t  oct_cntq_credit_instr_t;
#define OCT_CNTQ_CREDIT_INSTR_SIZE  (sizeof(oct_cntq_credit_instr_t))

typedef oct_cntq_init_word0_t  oct_cntq_delete_instr_t;
#define OCT_CNTQ_DELETE_INSTR_SIZE  (sizeof(oct_cntq_delete_instr_t))






/*------------------------   DDOQ ------------------------------*/


typedef enum {
	DDOQ_TYPE_NULL  = 0x0,
	DDOQ_TYPE_TCP   = 0x1,
	DDOQ_TYPE_TEST  = 0x2,
	MAX_DDOQ_TYPES  = 3
} DDOQ_TYPES;


typedef enum {
	DDOQ_ACTION_NULL   = 0x0,
	DDOQ_ACTION_INIT   = 0x1,
	DDOQ_ACTION_DELETE = 0x2,
	DDOQ_ACTION_CREDIT = 0x3,
	MAX_DDOQ_ACTIONS   = 4
} DDOQ_ACTIONS;


/* 32-bit state value that are endian-nuetral. */
typedef enum {
	DDOQ_STATE_CREATE_INIT           = 0x11000011,
	DDOQ_STATE_DELETE                = 0x22000022,
	DDOQ_STATE_READY                 = 0x33000033,
	DDOQ_STATE_DELETE_INIT           = 0x44000044,
	DDOQ_STATE_STOP_RX               = 0x55000055,
	DDOQ_STATE_CORE_FAILED           = 0x66000066,
	DDOQ_STATE_NULL                  = 0x0
} OCTEON_DDOQ_STATE;

#define MAX_DDOQ_UDD_SIZE    32



/** Format of the info field of a ddoq descriptor. */
typedef struct {
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
  uint64_t              buf_size:16; 
  uint64_t              pkt_size:16;
  uint64_t              reserved:32;
#else
  uint64_t              reserved:32;
  uint64_t              pkt_size:16;
  uint64_t              buf_size:16;
#endif
  uint64_t              resp_hdr;

} octeon_ddoq_info_t;

#define  OCT_DDOQ_INFO_SIZE     (sizeof(octeon_ddoq_info_t))




/** Format of a DDOQ descriptor. */
typedef struct {

  uint64_t      info_ptr;
  uint64_t      buffer_ptr;

} octeon_ddoq_desc_t;

#define  OCT_DDOQ_DESC_SIZE     (sizeof(octeon_ddoq_desc_t))




/** DDOQ packet indicator sent as CNTQ payload by core to host */
typedef union {

	uint64_t    u64;

	struct  {
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
		uint64_t   pkt_cnt:16;
		uint64_t   rsvd:48;
#else
		uint64_t   rsvd:48;
		uint64_t   pkt_cnt:16;
#endif
	} s;
} octeon_ddoq_ind_t;




/** DDOQ Front data field sent in instructions to core. */
typedef union {
	uint64_t    u64;
	struct {
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
		/** The size of buffers used in this DDOQ. */
		uint64_t    buf_size:16;
		/** For a INIT instruction, it gives the number of descriptors
		    to be used for this DDOQ. For a CREDIT instruction, it gives
		    the number of descriptors that have new buffers allocated 
		    for them.  */
		uint64_t    desc_count:16;
		uint64_t    type:4;
		uint64_t    reserved2:6;
		/** The id of the DDOQ for which the instruction applies. */
		uint64_t    ddoq_id:22;
#else
		uint64_t    ddoq_id:22;
		uint64_t    reserved2:6;
		uint64_t    type:4;
		uint64_t    desc_count:16;
		uint64_t    buf_size:16;
#endif
	} s;
} ddoq_dptr_start_t;

#define OCT_DDOQ_DPTR_START_SIZE   (sizeof(ddoq_dptr_start_t))
 
typedef ddoq_dptr_start_t   ddoq_udd_front_t;



/** Front data attached to a DDOQ INIT instruction sent from host to core. */
typedef struct {

	/** Host expects the next packet with this value in request id of its
	    cntq_pkt. */
#if  __CAVIUM_BYTE_ORDER  == __CAVIUM_LITTLE_ENDIAN
	uint64_t             next_pkt_cntr:24;
	uint64_t             refill_threshold:16;
	uint64_t             reserved:8;
	uint64_t             avl_desc:16;
#else
	uint64_t             avl_desc:16;
	uint64_t             reserved:8;
	uint64_t             refill_threshold:16;
	uint64_t             next_pkt_cntr:24;
#endif

	uint64_t             host_status_addr;

	ddoq_dptr_start_t    start;

	uint8_t              udd[MAX_DDOQ_UDD_SIZE];

} ddoq_front_data_t;


#endif


/* $Id: octeon-cntq-common.h 63418 2011-08-11 21:17:53Z panicker $ */
