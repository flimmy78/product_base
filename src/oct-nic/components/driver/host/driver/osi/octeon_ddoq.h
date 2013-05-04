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



/*! \file octeon_ddoq.h
    \brief  Definitions for DDOQ in the PCI host driver.
*/


#ifndef  __OCTEON_DDOQ_H__
#define  __OCTEON_DDOQ_H__



#include "cavium_sysdep.h"
#include "octeon_cntq_defs.h"
#include "octeon-cntq-common.h"
#include "octeon_stats.h"





/*-------------------------------  DDOQ    ---------------------------------*/


#define DDOQ_INSTR_TYPES   5



/** Type of DDOQ Instruction */

typedef enum {
     DDOQ_INIT   = 0,
     DDOQ_CREDIT = 1,
     DDOQ_QUERY  = 2,
     DDOQ_RESET  = 3,
     DDOQ_DELETE = 4
} OCTEON_DDOQ_INSTR_CODE;


#ifdef USE_DDOQ_THREADS
extern int oct_ddoq_thread(void* arg); 
extern int cvm_ddoq_num_threads;
#endif


typedef struct {

     octeon_resp_hdr_t    resp_hdr;

#if  __CAVIUM_BYTE_ORDER == __CAVIUM_BIG_ENDIAN
     uint32_t             reserved;
     uint32_t             status;

#else
     uint32_t             status;
     uint32_t             reserved;
#endif

} ddoq_rptr_space_t;






/** Structure of a DDOQ in host memory. */

typedef struct  {

   cavium_list_t          list;

   /** Lock to control access to this DDOQ. */
   cavium_spinlock_t      lock;

   /** A flag that indicates that this DDOQ is ready to accept packets. */
   uint32_t               state;

   /** ID of this DDOQ. */
   uint32_t               ddoq_id;

   /** The setup parameters sent by the user during creation of this DDOQ. */
   octeon_ddoq_setup_t    ddoq_setup;

   /**  The location of the descriptor ring for this DDOQ   */
   octeon_ddoq_desc_t     *desc_ring;

   /** The virtual address of start of info ptr memory   */
   octeon_ddoq_info_t     *info_ptr;

   /** The DMA mapped address of the info pointer */
   unsigned long          info_ptr_dma;

   /** The virtual address of the buffers are kept in this array */
   void                   **buffer_list;

   /** The index into the descriptor ring where the next packet will be read
       by host  */
   uint32_t               host_read_index;


   /** The number of descriptors available for new packets. */
   uint32_t               avl_desc;

   /** Number of available info ptrs. This can be less than no. of desc if 
       initial fill was != no of desc in ddoq setup. */
   uint32_t               avl_info_ptrs;

   /** The number of descriptors that were processed but not refilled with
       new buffers. */
   uint32_t               desc_pending_refill;

   /** The index in the descriptor ring where the refill should begin  */
   uint32_t               refill_index;

   /** Packets (not descriptors) pending to be processed. */
   cavium_atomic_t        pkts_pending;

   /** Flag that indicates ddoq packets are being processed. */
   cavium_atomic_t        in_process_ddoq;

   /** Count of descriptors that were refilled but not credited to core. */
   uint32_t               desc_pending_credit;

   /** The index in descriptor ring from where the next credit will fetch
       its descriptors. */
   uint32_t               credit_index;

   /** The tag to use for instructions sent for this DDOQ */
   uint32_t               tag;

   /** Pointer to octeon device used by callback functions. */
   void                   *octeon_dev;


   /**    Packet statistics    */
   oct_ddoq_stats_t       stats;


   /** Copy of UserDefinedData for this DDOQ */
   uint8_t                ddoq_udd[MAX_DDOQ_UDD_SIZE];

   /** The next packet counter value for this DDOQ. */
   uint64_t               next_pkt_cntr;

   /** Space for response for instructions posted for this ddoq */
   ddoq_rptr_space_t      rptr_space;

   /** Host copy of dptr_start structure. The copy in "front" is DMA'ed. */
#if defined (_WIN32)
  __declspec (align (8))    ddoq_dptr_start_t        dptr_start;
#else
   ddoq_dptr_start_t  __attribute__ ((aligned(8)))   dptr_start;
#endif

   /** Front data sent to core. */
   ddoq_front_data_t      front;

} octeon_ddoq_t ;

#define  OCT_DDOQ_SIZE           (sizeof(octeon_ddoq_t))











/** Called by CNTQ when there is a packet in a DDOQ.
  * @param octeon_dev  - pointer to the octeon device.
  * @param ddoq        - pointer to ddoq where a packet has arrived.
  * @param pkt_cnt     - number of packets to be processed in the ddoq.
  */
void
octeon_process_ddoq(octeon_device_t     *octeon_dev,
                    octeon_ddoq_t       *ddoq,
                    uint32_t               pkt_cnt);


#endif

/* $Id: octeon_ddoq.h 67088 2011-11-15 19:39:17Z mchalla $ */
