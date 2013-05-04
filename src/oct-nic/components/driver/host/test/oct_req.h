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

/*! \file  oct_req.h
    \brief Host test app: Header file for linux user-space request/response
	                      test application.
*/


/* Refer to the html documentation about restriction on input and
   output sizes for user-space requests.
*/


/* The octeon device on which requests must be sent. */
#define OCTEON_ID    0

#define REQ_IQ_NO    0

#define TEST_THREAD_COUNT  1


/* Enable this definition to add signature data to input buffers which gets
   verified from the output buffers on response. The core application is
   responsible for copying the input buffer contents to the output buffers.
*/
#define VERIFY_DATA 1


/* Turn on this flag to generate diff input/output buf sizes when MAX_INBUFS
 * and MAX_OUTBUFS > 1 */
//#define RANDOMIZE   


/* Maximum number of input and output buffers. The actual number used for
   a request is a random selection in the range (1, MAX_USERBUFS). This
   value should be in the range (1 <= MAX_USERBUFS <= MAX_BUFCNT).
   MAX_BUFCNT is defined in driver/host/include/cavium_defs.h.
   Also when using SCATTER dma mode the max output buffers cannot exceed 13.
*/
#define MAX_INBUFS 1
#define MAX_OUTBUFS 1



/* Size of each input buffer. Choose values such that maximum input size
   (MAX_INBUFS * INBUF_SIZE) does not exceed maximum bytes defined for
   each dma mode. See include/cavium_defs.h for max data values.
*/
#define INBUF_SIZE    (1 * 1024)




/* Size of each output buffer. Choose values such that maximum output size
   (MAX_OUTBUFS * OUTBUF_SIZE) does not exceed maximum bytes defined for
   each dma mode. See include/cavium_defs.h for max data values.
   The minimum valid size is 16 bytes. The maximum supported value is 
   (122744) for scatter. See driver documentation for more details.
*/
#define OUTBUF_SIZE    (1 * 1024)



#define REQUEST_TIMEOUT   500


#define MAX_NB_REQUESTS   256



/* DMA mode to use for requests. Pick one of
   OCTEON_DMA_DIRECT; OCTEON_DMA_GATHER; OCTEON_DMA_SCATTER; 
   OCTEON_DMA_SCATTER_GATHER.

   Default is OCTEON_DMA_DIRECT.
*/
OCTEON_DMA_MODE        TEST_DMA_MODE  =   OCTEON_DMA_DIRECT;




/* Response order to use for requests. Pick one of
   OCTEON_RESP_NORESPONSE; OCTEON_RESP_ORDERED; OCTEON_RESP_UNORDERED;

   Note: ORDERED mode is not supported for user-space requests. This 
         mode is useful only as a negative test for the driver.

   Default is OCTEON_RESP_NORESPONSE.
*/
OCTEON_RESPONSE_ORDER   TEST_RESP_ORDER  =  OCTEON_RESP_NORESPONSE;




/* Response mode to use for requests. Pick one of
   OCTEON_RESP_NON_BLOCKING; OCTEON_RESP_BLOCKING;

   Default is OCTEON_RESP_NON_BLOCKING.
*/
OCTEON_RESPONSE_ORDER   TEST_RESP_MODE  =  OCTEON_RESP_NON_BLOCKING;




/* $Id: oct_req.h 53786 2010-10-08 22:09:32Z panicker $ */
