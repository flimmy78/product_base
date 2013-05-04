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



#ifndef __CVMCS_CNTQ_TEST_H__
#define __CVMCS_CNTQ_TEST_H__

#include  "cvmcs-cntq-defs.h"


/* Enable this define to send test packets to CNTQ on host. */
//#define   CVMCS_TEST_CNTQ


/* Enable this define to send test packets to DDOQ on host. */
//#define   CVMCS_TEST_DDOQ


#define  CNTQ_TEST_SHIFT_FREQ  10


/* Enable this define for the data transmitted to host to be filled with byte
   values. */
#define CVMCS_SIGN_DATA


#define MAX_TEST_DDOQS        1

#define TEST_DDOQ_PKT_LEN     16000
#define MAX_DDOQ_GATHER_PTRS  30



#if   defined(CVMCS_TEST_DDOQ)
void   cvmcs_init_ddoq_test(void);
int    cvmcs_do_ddoq_test(void);
int    cvmcs_test_ddoq_send(uint32_t  ddoq_id);
#else
#define   cvmcs_init_ddoq_test()  do {  } while(0)
#define   cvmcs_do_ddoq_test()          (0) 
#define   cvmcs_test_ddoq_send(ddoq_id) (1) 
#endif

#if   defined(CVMCS_TEST_CNTQ)
int    cvmcs_do_cntq_test(void);
#else
#define   cvmcs_do_cntq_test()          (1) 
#endif

#endif
/* $Id: cvmcs-cntq-test.h 63359 2011-08-10 22:08:08Z panicker $ */
