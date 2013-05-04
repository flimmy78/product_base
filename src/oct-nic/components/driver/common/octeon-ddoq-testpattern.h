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
#ifndef __OCTEON_DDOQ_TESTPATTERN_H__
#define __OCTEON_DDOQ_TESTPATTERN_H__

#define DDOQSIGNBYTES 64

static  uint8_t  ddoqsign[DDOQSIGNBYTES] =

{
 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67
};



#define INCR_SIGNIDX(idx)  {if (++(idx) == DDOQSIGNBYTES) (idx) = 0;}

#endif

