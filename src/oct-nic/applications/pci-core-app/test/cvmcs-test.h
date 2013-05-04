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


#ifndef __CVMCS_TEST_H__
#define __CVMCS_TEST_H__

#include "cvmcs-common.h"

#ifdef  CN56XX_PEER_TO_PEER
#include "cn56xx_ep_comm.h"
#endif




#define PKO_TEST_FIXED_DATA           1
#define PKO_TEST_SEQUENTIAL_DATA      2
#define PKO_TEST_RANDOM_DATA          3



/* Enable this define to test the PCI output queues. */
//#define CVMCS_TEST_PKO  PKO_TEST_SEQUENTIAL_DATA


/* Enable this flag to run a simple PKO test where each core attempts to send
   as many 64-byte packets as it can. See cvmcs_max_pkt_pko_test() in 
   cvmcs-pko-test.c file.
*/
//#define MAX_PACKET_RATE_TEST


/* Enable PROGRAM_THRU_SCRATCH if you want to control test parameters
   through the NPEI_SCRATCH register in CN56XX.
   The scratch register operation type may be different for different tests.
   Scratch register bit definition:
    ---------------------------------------------
   | operation type       | value                |
    ---------------------------------------------
   63                      31                   0

  PKO Test:
   Operation type: 1 -> value is pkts/sec (32 bit value)
                   2 -> value is output queue mask (32 bit mask with 1 bit for
                        each PCI output queue on which packet should be sent.
                   3 -> value is core mask (32 bit mask with 1 bit for each
                        core that should participate in the test.
  If scratch register is written using 32-bit writes, then write the value
  first followed by the operation.
*/

//#define  PROGRAM_THRU_SCRATCH         1



/* Enable this flag to run the Inbound/Outbound DMA performance test. */
//#define DMA_PERF_TEST 


/* Enable this define when running the DMA demo application on host. */
//#define CVMCS_DMA_DEMO



#ifdef  CN56XX_PEER_TO_PEER
#define  TEST_PEER_TO_PEER
#endif








void cvmcs_print_reg_contents();
int cvmcs_process_instruction(cvmx_wqe_t  *wqe);
int cvmcs_process_scatter_instruction(cvmx_wqe_t  *wqe);


#ifdef CVMCS_DMA_DEMO
int cvmcs_process_dma_demo(cvmx_wqe_t  *wqe);
#endif


#ifdef DMA_PERF_TEST
int   cvmcs_dma_perf_send_first_cmds(void);
int   cvmcs_dma_perf_process_wqe(cvmx_wqe_t *);
void  cvmcs_print_dma_perf_stats(void);
#endif



#if defined(CVMCS_TEST_PKO) || defined(MAX_PACKET_RATE_TEST)


#define PKO_TEST_PKT_RATE            10240

/* Enable this define for the data transmitted to host to be filled with byte
   values. */
#define CVMCS_SIGN_DATA


#define  CVM_MIN_DATA           1

/* The maximum size of data to be used in the PKO test. */
/* This value should not exceed 65520. */
/* The actual size sent will be (CVMCS_MAX_DATA + 8). 8 bytes are used by the
   PCI core driver as header. */
#define CVM_MAX_DATA             8192 //65500

void  cvmcs_do_pko_test();
void  cvmcs_test_pko_global_init(void);

#endif


int     cvmcs_test_process_wqe(cvmx_wqe_t  *wqe);
uint8_t cvmcs_sign_data(uint8_t  *buf, uint32_t  size, uint8_t  start);
void    cvmcs_test_print_options(void);
void    cvmcs_test_global_init(void);





#endif

/* $Id: cvmcs-test.h 59275 2011-04-15 23:07:14Z panicker $ */
