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


/*! \file cvm-cntq.h
    \brief  Core Driver: Definitions for CNTQ in the PCI core driver.
*/

#ifndef __CVM_CNTQ_H__
#define __CVM_CNTQ_H__

#include "cvm-driver-defs.h"
#include "cvm-drv.h"
#include "cavium-list.h"
#include "octeon-opcodes.h"
#include "octeon-cntq-common.h"




#define CVM_CNTQ_FPA_POOL      CVMX_FPA_WQE_POOL
#define CVM_CNTQ_SCRATCHPAD    CVM_DRV_SCR_WQE_BUF_PTR


#define CVM_CNTQ_ALLOC_BUFFER(ptr)    \
      ptr = cvm_drv_fpa_alloc_async(CVM_CNTQ_SCRATCHPAD, CVM_CNTQ_FPA_POOL);

                                                                                
#define CVM_CNTQ_FREE_BUFFER(ptr)     \
{  \
      cvm_drv_fpa_free(ptr, CVM_CNTQ_FPA_POOL, 0);  \
      ptr = NULL; \
}

                                                                                
#define CVM_CNTQ_SET_FPA_POOL(lptr)   \
      lptr.s.pool = CVM_CNTQ_FPA_POOL;




                                                                             
/** Core: The CNTQ structure in PCI core driver. */
typedef struct {

   /** Lock for the shared CNTQ structure  */
   cvmx_spinlock_t       lock;

   /** The priority of this queue. */
   uint64_t              pri;

   /** Starting address of the CNTQ ring in the host's memory */
   uint64_t              base_addr;

   /** The location in host memory where the next CNTQ packet will
      be written
   */
   uint64_t              core_wr_ptr;

   /** The next location in host memory where the host should look
      for a CNTQ packet 
   */
   uint64_t              host_rd_ptr;

   /** The first address beyond the CNTQ ring. This field helps wrap-
      around calculation
   */
   uint64_t              end_addr;

   /** The current available 8B blocks in CNTQ ring as calculated in the core. */
   uint16_t              avl_size;

   /** The total size of the CNTQ ring in 8B blocks */
   uint16_t              total_size;

   /**  No. of pkts linked to the wait list */
   uint16_t              wait_count;

   /**  No. of pkts linked to the wait list */
   uint16_t              initialized;

   /** No. of credits received from the host driver. */
   uint64_t              credits_recvd;

   /** Head of wait list where packets are stored if CNTQ on host has no space. */
   cavium_list_t         wait_list;
} cvm_cntq_t;





/** Statistics for the local cntq structure (useful for debug purposes) */
typedef struct {

	/** Total packets sent */
	uint64_t   pkts_sent;

	/** Total packets dropped due to lack of memory */
	uint64_t   pkts_dropped_nomem;

	/** Total packets dropped due to incorrect cntq state*/
	uint64_t   pkts_dropped_nocntq;

	/** Total packets dropped due to PCI DMA error */
	uint64_t   pkts_dropped_dmaerr;

	/** Total packets added to wait list */
	uint64_t   pkts_waitlisted;

	/** Total credits received */
	uint64_t   credits_pkts;

	/** Total credits received that were not of the expected value */
	uint64_t   credits_invalid;

} cvm_cntq_global_stats_t;





/*-------- Format of the CNTQ instructions-------- */

/** Core: Format of CNTQ INIT instruction received by the core driver. */
typedef struct {

	/** The Host PCI driver RAW instruction (IH, RPTR, IRH) */
	cvmx_raw_inst_front_t      front;

	/** The CNTQ init command data from host */
	oct_cntq_init_instr_t      cmd;

}cvm_cntq_init_t;


                                                                                             


/** Core: Format of the CNTQ CREDIT instruction received by core driver. */
typedef struct {

	/** The Host PCI driver RAW instruction (IH, RPTR, IRH) */
	cvmx_raw_inst_front_t      front;

	/** The CNTQ init command data from host */
	oct_cntq_credit_instr_t    cmd;

}cvm_cntq_credit_t;




/** Core: Format of the CNTQ DELETE instruction received by core driver. */
typedef struct {

	/** The Host PCI driver RAW instruction (IH, RPTR, IRH) */
	cvmx_raw_inst_front_t      front;

	/** The CNTQ delete command data from host */
	oct_cntq_delete_instr_t    cmd;

}cvm_cntq_delete_t;



/*-----------end instructions ------------------*/




#define CVM_CNTQ_STATIC_PKT   0
#define CVM_CNTQ_ALLOC_PKT    0x1010

/** Core: Packet Header for a internal CNTQ packet.
    The header keeps information about
    the queue to be used, and the length of the data in the packet. The data
    pointer points to the actual data in the CNTQ packet. The "list" field allows
    the cntq pkt to be chained to lists.
*/
typedef struct __CVM_CNTQ_PKT {

    /** Enables this packet to be added to a linked list. */
    cavium_list_t           list;

    /** Total length of the packet including this header and data
        rounded off to 8B multiple. */ 
    uint16_t                pkt_len;

    /** Determine if this packet was allocated or is on stack/heap. */
    uint16_t                pkt_type;

    /** The DMA queue to use to send this packet. */
    uint16_t                pri;

    /** The pool from which this packet was allocated. */
    uint16_t                pool;

    /** Pointer to the data for this CNTQ packet. */
    uint64_t               *data;

    /** Length of data in this packet */
    uint64_t                len;

    /** Response Header associated with this packet. */
    cvmx_resp_hdr_t         resp_hdr;

} cvm_cntq_pkt_t;

#define CVM_CNTQ_PKT_SIZE       (sizeof(cvm_cntq_pkt_t))

#define CVM_CNTQ_PKT_HDR_SIZE   16




/** Core: Initialize a CNTQ packet allocated dynamically.
  * @param buf  - pointer to the cntq packet.
  * @param pri  - the priority of the cntq on which this packet is sent(0,1).
  * @param bufsize - Size of data in this cntq packet.
  */
static inline  void
cvm_cntq_init_cntq_pkt(cvm_cntq_pkt_t        *buf, 
                       CVMX_DMA_QUEUE_TYPE    pri,
                       uint32_t               bufsize)
{
   memset(buf, 0, CVM_CNTQ_PKT_SIZE);
   CAVIUM_INIT_LIST_HEAD(&(buf->list));
   buf->pkt_len   = ROUNDUP8(bufsize) + CVM_CNTQ_PKT_HDR_SIZE;
   buf->pri  = pri;
   buf->data = (uint64_t *)((uint8_t *)buf + CVM_CNTQ_PKT_SIZE);
   buf->len  = bufsize;
}




/** Core: This macro allows an app to allocate a CNTQ packet buffer. 
  * The macro allocates a 8-byte aligned buffer that stores the 
  * CNTQ packet header and the cntq packet data. 
  * The length field is aligned with the data field, so the 
  * actual CNTQ PCI packet starts from the "len" field.
  * @param pri - the priority of cntq on which the packet will be sent(0,1).
  * @param bufsize - size of data in the cntq packet. 
  * @return Returns pointer a cntq packet if success else NULL.
  */
static inline cvm_cntq_pkt_t *
cvm_alloc_cntq_packet(CVMX_DMA_QUEUE_TYPE   pri, uint32_t bufsize)
{
   cvm_cntq_pkt_t *buf;

   CVM_CNTQ_ALLOC_BUFFER(buf);
   DBG_PRINT(DBG_FLOW, "cntq pkt allocated at 0x%016lx\n", (uint64_t)buf);
   if (buf)  {
      cvm_cntq_init_cntq_pkt(buf, pri, bufsize);
      buf->pkt_type  = CVM_CNTQ_ALLOC_PKT;
      buf->pool      = CVM_CNTQ_FPA_POOL;
   }
   return buf;
}





/** Core: Free a cntq packet previously allocated by cvm_alloc_cntq_packet().
  * @param buf - pointer to the cntq packet.
  */
static inline void
cvm_cntq_free_packet(cvm_cntq_pkt_t *buf)
{
    DBG_PRINT(DBG_FLOW,"cntq pkt at 0x%016llx freed into pool %d\n",
             cast64(buf), buf->pool);
    cvm_drv_fpa_free(buf, buf->pool, 0);
}




/*------ Function Prototypes follow ----------*/

/** Called by the core driver at init time to setup the CNTQ structure.
  */
void       cvm_cntq_setup( CVMX_DMA_QUEUE_TYPE );

/** Called by the core driver to process a CNTQ instruction. 
  * @param wqe - the work queue entry containing the cntq instruction.
  */
void       cvm_cntq_process_instr(cvmx_wqe_t *wqe); 

/** Called by applications or the ddoq portion of PCI core driver
  * to send a packet on CNTQ.
  * @param cntq_pkt - pointer to a cntq packet.
  * @return 0 if the packet was sent with success, else 1.
  */
int        cvm_cntq_send_packet(cvm_cntq_pkt_t   *cntq_pkt);


void cvm_cntq_print( CVMX_DMA_QUEUE_TYPE );

int
cvm_cntq_get_init_status(uint32_t  cntq_no);

int
cvm_drv_setup_dma_queues(void);

#endif

/* $Id: cvm-cntq.h 42868 2009-05-19 23:57:52Z panicker $ */
