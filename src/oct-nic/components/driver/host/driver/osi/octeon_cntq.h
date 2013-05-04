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




/*! \file octeon_cntq.h
    \brief  Definitions for CNTQ in the PCI host driver.
*/

#ifndef  __OCTEON_CNTQ_H__
#define  __OCTEON_CNTQ_H__



#include "octeon_device.h"
#include "octeon_instr.h"
#include "octeon_cntq_defs.h"




/*----------------------------    CNTQ   -------------------------------*/

#define MAX_OCTEON_CNTQ   2

/** CNTQ Priority */
typedef enum {
     OCTEON_CNTQ0 = 0,
     OCTEON_CNTQ1 = 1
} OCTEON_CNTQ;



/** CNTQ can send 4 types of instructions to the core. */

typedef enum {
     CNTQ_INIT   = 0,
     CNTQ_CREDIT = 1,
     CNTQ_DELETE = 2,
} OCTEON_CNTQ_INSTR_CODE;

#define CNTQ_INSTR_TYPES     3



/** The config structure contains the parameters to initialize the 
    Counter Queues of Octeon. */
typedef struct {

   /** Size of the CNTQ ring in bytes */
   uint32_t          size;

   /** Interrupt threshold values for CNTQ 
       - No. of pkts buffered by Octeon before a interrupt is issued.*/
   uint32_t          pkt_count_threshold;

   /** Timeout with non-zero pkt count in Octeon before a interrupt is issued */
   uint32_t          intr_time_threshold;
   
   /** Number of packets to process per interrupt. */
   uint32_t          pkts_per_interrupt;

   /** Credit threshold - Number of CNTQ bytes processed before crediting the core. */
   uint32_t          credit_threshold;

} octeon_cntq_config_t;


/** The CNTQ structure embedded in the octeon_device_t structure.
 There are 2 DMA Counter queues in Octeon and each is represented
 in the host by this structure.
 The iq and iq_mode fields will be calculated at init time.
*/

typedef struct {

   /** Lock for the CNTQ structure   */
   cavium_spinlock_t            lock;
   
   /** The base (start) address of the CNTQ ring in host memory. */
   uint64_t                      *base_addr;

   /** The DMA mapped address of CNTQ ring */
   unsigned long            base_addr_dma;
   
   /** The register to read packet count for this DMA queue. */
   void                        *dma_cnt_reg;

   /** The tag to use for instructions sent for this DMA queue to Octeon */
   uint32_t                       tag;

   /** Offset into CNTQ ring in host memory where the next packet
      should be read */
   uint16_t                       host_read_index;

   /* The first address in host memory beyond the CNTQ ring. */
   uint8_t                       *end;

   /** Size of the CNTQ ring in 8B blocks */
   uint16_t                       size;

   /** Statistics for this DMA queue are kept in this structure. */
   oct_cntq_stats_t             stats;

   /** The id of this CNTQ  (0 <= cntq_no <= 1). */
   OCTEON_CNTQ                  cntq_no; 
   
   /** The octeon instruction queue to use to send this CNTQ's instructions */
   int                          iq;

   /** The mode of the selected instruction queue (32B/64B command mode) */
   OCTEON_IQ_INSTRUCTION_MODE   iq_mode;

   /** Interrupt threshold values for CNTQ 
       - No. of pkts buffered in Octeon before a interrupt is issued. */
   uint32_t                       pkt_count_threshold;

   /** Timeout with non-zero pkt count in Octeon before a interrupt is issued */
   uint32_t                       intr_time_threshold;

   /** Number of packets to process per interrupt. */
   uint32_t                       pkts_per_interrupt;

   /** Credit threshold - Number of pkts to process before crediting the core. */
   uint32_t                       credit_threshold;

   /**  Soft instruction for each type of CNTQ instruction */
   octeon_soft_instruction_t   *instr[CNTQ_INSTR_TYPES];
   
   /** A flag indicating that the CNTQ has been successfully initialized 
      in Octeon core. */  
   uint32_t                         initialized;

   /** A credit instruction is sent to Octeon every time we have "credit_store"
       bytes freed after packet processing. */
   uint32_t                         credit_store;

   cavium_atomic_t                  pkts_pending; 

} octeon_cntq_t;


#define OCT_CNTQ_SIZE    (sizeof(octeon_cntq_t))




int   octeon_cntq_intr_handler(void       *dev,
                               uint64_t    intr);


void octeon_cntq_bh(unsigned long);
                                                                                 

int
octeon_cntq_get_stats(int octeon_id, int cntq_no, oct_cntq_stats_t  *cntq_stats);


int
octeon_change_cntq_status(int octeon_id, int status);

int
octeon_get_cntq_status(int octeon_id, int cntq_no);



#endif  /* __OCTEON_CNTQ_H__ */

/* $Id: octeon_cntq.h 53785 2010-10-08 22:08:08Z panicker $ */
