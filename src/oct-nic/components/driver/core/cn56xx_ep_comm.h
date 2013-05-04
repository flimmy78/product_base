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

/*! \file  cn56xx_ep_comm.h
    \brief Core Driver: Structures & simple executive API for
	                    endpoint-to-endpoint communication.
*/


#ifndef  __CN56XX_EP_COMM_H__
#define  __CN56XX_EP_COMM_H__



#include "cvmx.h"
#include "cvm-drv.h"
#include "cvm-driver-defs.h"
#include "octeon-opcodes.h"

#define  MAX_EP_PKT_BUFS  4


/** Core: Structure used by driver for buffer management in end-point 
	      communication. */
typedef  struct {

	/** List of buffers */
	cvmx_buf_ptr_t  buf[MAX_EP_PKT_BUFS];

	/** Internal gather pointer used by driver. */
	cvmx_buf_ptr_t  gptr;

} cn56xx_ep_buflist_t;




/** Core: Structure passed by application to send data to another endpoint. */
typedef struct {

	/** Tag in IH to use in input queue command. */
	uint32_t  tag;

	/** Tagtype in IH to use in input queue command. */
	uint8_t   tagtype;

	/** Param value in IRH to use in input queue command. */
	uint8_t   param;

	/** Opcode value in IRH to use in input queue command. */
	uint16_t  opcode;

	/** Group value in IH to use in input queue command. */
	uint8_t   grp:4;

	/** QOS in IH to use in input queue command. */
	uint8_t   qos:3;

	uint8_t   free:1;

	/** Number of input buffers */
	uint8_t   bufcount;

	/** The Peer Input queue to use to send data. */
	uint8_t   piq_id;

	uint8_t   reserved;

	/** List of input buffers */
	cvmx_buf_ptr_t  buf[MAX_EP_PKT_BUFS];

} cn56xx_ep_packet_t;






int   cn56xx_alloc_peeriq_memory(void);

void  cn56xx_setup_peeriq_op_handler(void);

void  cn56xx_ep_update_peeriq(int piq_id);



int
cn56xx_send_ep_packet(cn56xx_ep_packet_t  *pkt);

int
cn56xx_send_ep_packet_to_octeon(int oct_id, cn56xx_ep_packet_t  *pkt);



int  cn56xx_send_ep_test_pkt_to_peer(int peer_id);
int  cn56xx_send_ep_test_pkt_to_octeon(int oct_id);


void  cn56xx_process_ep_test_packet(cvmx_wqe_t   *wqe);

void  cn56xx_print_peer_iq_stats(int piq_id);
void  cn56xx_print_ep_pkt_count();


#endif


/* $Id: cn56xx_ep_comm.h 53786 2010-10-08 22:09:32Z panicker $  */

