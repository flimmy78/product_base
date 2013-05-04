/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2007 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
**********************************************************************/

/*
 * A few defines are used to control the operation of this driver:
 *  CONFIG_CAVIUM_RESERVE32
 *      This kernel config options controls the amount of memory configured
 *      in a wired TLB entry for all processes to share. If this is set, the
 *      driver will use this memory instead of kernel memory for pools. This
 *      allows 32bit userspace application to access the buffers, but also
 *      requires all received packets to be copied.
 *  CONFIG_OCTEON_NUM_PACKET_BUFFERS
 *      This kernel config option allows the user to control the number of
 *      packet and work queue buffers allocated by the driver. If this is zero,
 *      the driver uses the default from below.
 *  USE_SKBUFFS_IN_HW
 *      Tells the driver to populate the packet buffers with kernel skbuffs.
 *      This allows the driver to receive packets without copying them. It also
 *      means that 32bit userspace can't access the packet buffers.
 *  USE_32BIT_SHARED
 *      This define tells the driver to allocate memory for buffers from the
 *      32bit sahred region instead of the kernel memory space.
 *  USE_HW_TCPUDP_CHECKSUM
 *      Controls if the Octeon TCP/UDP checksum engine is used for packet
 *      output. If this is zero, the kernel will perform the checksum in
 *      software.
 *  USE_ASYNC_IOBDMA
 *      Use asynchronous IO access to hardware. This uses Octeon's asynchronous
 *      IOBDMAs to issue IO accesses without stalling. Set this to zero
 *      to disable this. Note that IOBDMAs require CVMSEG.
 *  REUSE_SKBUFFS_WITHOUT_FREE
 *      Allows the TX path to free an skbuff into the FPA hardware pool. This
 *      can significantly improve performance for forwarding and bridging, but
 *      may be somewhat dangerous. Checks are made, but if any buffer is reused
 *      without the proper Linux cleanup, the networking stack may have very
 *      bizarre bugs.
 */
#ifndef __ETHERNET_DEFINES_H__
#define __ETHERNET_DEFINES_H__

#include <asm/octeon/cvmx-config.h>


#define OCTEON_ETHERNET_VERSION "2.0"

#ifndef CONFIG_CAVIUM_RESERVE32
#define CONFIG_CAVIUM_RESERVE32 0
#endif

#if CONFIG_CAVIUM_RESERVE32
#define USE_32BIT_SHARED            1
#define USE_SKBUFFS_IN_HW           0
#define REUSE_SKBUFFS_WITHOUT_FREE  0
#else /* !CONFIG_CAVIUM_RESERVE32 */
#define USE_32BIT_SHARED            0
#define USE_SKBUFFS_IN_HW           1
#ifdef CONFIG_NETFILTER
#define REUSE_SKBUFFS_WITHOUT_FREE  0
#else
#define REUSE_SKBUFFS_WITHOUT_FREE  1
#endif /* CONFIG_NETFILTER */
#endif /* CONFIG_CAVIUM_RESERVE32 */

#define USE_HW_TCPUDP_CHECKSUM      0 /* caojia@autelan.com close the function for RPA tcp/udp packet */

/* Enable Random Early Dropping under load */
#define USE_RED                     1
#define USE_ASYNC_IOBDMA            (CONFIG_CAVIUM_OCTEON_CVMSEG_SIZE > 0)

/*
 * Allow SW based preamble removal at 10Mbps to workaround PHYs giving
 * us bad preambles.
 */
#define USE_10MBPS_PREAMBLE_WORKAROUND 1

/**
 * Allow software to configure hardware timestamping even if it
 * isn't supported by the hardware. This can be used to allow
 * debugging timestamp code on Octeons that don't support
 * hardware timestamps. The generated software timestamps will
 * generally be more accurate than Linux's default implementation,
 * so this can be useful even in production systems.
 */
#define ALLOW_TIMESTAMPS_WITHOUT_HARDWARE 1

/*
 * Use this to have all FPA frees also tell the L2 not to write data
 * to memory.
 */
#define DONT_WRITEBACK(x)           (x)
/* Use this to not have FPA frees control L2 */
/*#define DONT_WRITEBACK(x)         0   */

/* Maximum number of SKBs to try to free per xmit packet. */
#define MAX_OUT_QUEUE_DEPTH 1000

#define FAU_NUM_PACKET_BUFFERS_TO_FREE (CVMX_FAU_REG_END - sizeof(u32))

#define TOTAL_NUMBER_OF_PORTS       (CVMX_PIP_NUM_INPUT_PORTS+1)

#define MAX_PORTS_PER_INTERFACE 4
#define MAX_IF_DEVICE_NAME_LEN 16

#define LINK_UP            1
#define LINK_DOWN          0

#define SE_MAGIC_MUN      0x12345678
#define FPA_INIT_WAIT     1
#define FPA_INIT_OK       2
#define IPD_EN_WAIT       3
#define IPD_EN_OK         4
#define SE_NO_EXIST       0
#define SE_COEXIST        1 
#define FROM_SE_GROUP	14


typedef struct kernel_nlmsg_s
{
    int action_type;
    int ipgport;
}kernel_nlmsg_t;
enum netlink_msg_type
{
	BOARD_STATE_NOTIFIER_EVENT,
	SYSTEM_STATE_NOTIFIER_EVENT,
	ASIC_ETHPORT_UPDATE_EVENT,
	OCTEON_ETHPORT_LINK_EVENT,
	NL_MSG_TYPE_MAX
};
typedef struct nl_msg_head{
	int type;
	int object;
    int count;
	int pid;
}nl_msg_head_t;
#endif /* __ETHERNET_DEFINES_H__ */
