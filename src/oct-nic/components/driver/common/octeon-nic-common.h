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

/*!  \file  octeon-nic-common.h
     \brief Common: Structures and macros used in PCI-NIC package by core and
                    host driver.
*/


#ifndef __OCTEON_NIC_COMMON_H__
#define __OCTEON_NIC_COMMON_H__

#include "octeon-common.h"


#ifdef ETHERPCI
/* EtherPCI supports 4 virtual OCTEON Ethernet interfaces */
#define   MAX_OCTEON_LINKS    4
#define   OCTNET_POW_GRP      15
#else
#define   MAX_OCTEON_LINKS    8
#define   OCTNET_POW_GRP      0
#endif

#define   OCTNET_MIN_FRM_SIZE        64
#define   OCTNET_MAX_FRM_SIZE        16018
#define   OCTNET_DEFAULT_FRM_SIZE    1518


/** NIC Commands are sent using this Octeon Input Queue */
#define   OCTNET_CMD_Q                0


/* NIC Command types */
#define   OCTNET_CMD_CHANGE_MTU       0x1
#define   OCTNET_CMD_CHANGE_MACADDR   0x2
#define   OCTNET_CMD_CHANGE_DEVFLAGS  0x3
#define   OCTNET_CMD_RX_CTL           0x4



/* Interface flags communicated between host driver and core app. */
typedef enum {
	OCTNET_IFFLAG_PROMISC    = 0x1,
	OCTNET_IFFLAG_ALLMULTI   = 0x2,
	OCTNET_IFFLAG_MULTICAST  = 0x4
} octnet_ifflags_t;



/*
   wqe
   ---------------  0
  |  wqe  word0-3 |
   ---------------  32
  |    PCI IH     |
   ---------------  40
  |     RPTR      |
   ---------------  48
  |    PCI IRH     |
   ---------------  56
  |  OCT_NET_CMD  |
   ---------------  64
  | Addtl 8-BData |
  |               |
   ---------------
*/


typedef union  {

	uint64_t     u64;

	struct {

#if __CAVIUM_BYTE_ORDER == __CAVIUM_BIG_ENDIAN
		uint64_t    cmd:5;

		uint64_t    more:3;

		uint64_t    param1:32;

		uint64_t    param2:16;

		uint64_t    param3:8;

#else

		uint64_t    param3:8;

		uint64_t    param2:16;

		uint64_t    param1:32;

		uint64_t    more:3;

		uint64_t    cmd:5;

#endif
	} s;

} octnet_cmd_t;


#define   OCTNET_CMD_SIZE     (sizeof(octnet_cmd_t))








/** Status of a RGMII Link on Octeon as seen by core driver. */
typedef union  {

	uint64_t   u64;

	struct {
#if __CAVIUM_BYTE_ORDER  ==  __CAVIUM_LITTLE_ENDIAN
		uint64_t   reserved:16;
		uint64_t   speed:16;
		uint64_t   mtu:16;
		uint64_t   status:8;
		uint64_t   duplex:8;
#else
		uint64_t   duplex:8;
		uint64_t   status:8;
 		uint64_t   mtu:16;
		uint64_t   speed:16;
		uint64_t   reserved:16;
#endif
	} s;
} oct_link_status_t;







/** Information for a OCTEON ethernet interface shared between core & host. */
typedef struct {

	oct_link_status_t   link;

	uint64_t            hw_addr;

#if __CAVIUM_BYTE_ORDER  ==  __CAVIUM_LITTLE_ENDIAN
	uint8_t             ifidx;
	uint8_t             txpciq;
    uint8_t             rxpciq;
	uint8_t             rsvd[3];
	uint16_t            gmxport;
#else
	uint16_t            gmxport;
	uint8_t             rsvd[3];
    uint8_t             rxpciq;
	uint8_t             txpciq;
	uint8_t             ifidx;
#endif

} oct_link_info_t;

#define OCT_LINK_INFO_SIZE   (sizeof(oct_link_info_t))



#endif

/* $Id: octeon-nic-common.h 63418 2011-08-11 21:17:53Z panicker $ */


