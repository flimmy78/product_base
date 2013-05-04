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

/*!  \file octcntq_config.h
     \brief Host CNTQ Driver: Configuration file for Octeon CNTQ driver.
*/


#ifndef __OCTCNTQ_CONFIG_H__
#define __OCTCNTQ_CONFIG_H__



/*-------------- The DMA output queues -------------- */
/* The DMA queue size in bytes */
#define OCTEON_CNTQ_SIZE             (32 * 1024)


/* The threshold (in bytes) for cntq data processed after which the
   driver should send a credit to the core. */
#define OCTEON_CNTQ_CREDIT_THRESHOLD (OCTEON_CNTQ_SIZE >> 4)


/* The maximum number of packets processed per interrupt per queue */
#define CNTQ_PKTS_PER_INTERRUPT       256


/* The packet count threshold for DMA queue interrupts. */
#define DMA_PKT_CNT_THRESHOLD         64


/* The time threshold (in microseconds) for DMA queue interrupts. */
#define DMA_TIME_THRESHOLD            100


#endif


/* $Id: octcntq_config.h 42868 2009-05-19 23:57:52Z panicker $ */
