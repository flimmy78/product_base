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
*********************************************************************/
#include <linux/skbuff.h>

#include <asm/octeon/cvmx-pip.h>

#define DEBUGPRINT(format, ...) do { if (printk_ratelimit()) 		\
					printk(format, ##__VA_ARGS__);	\
				} while (0)

/**
 * cvm_oct_get_buffer_ptr - convert packet data address to pointer
 * @pd: Packet data hardware address
 *
 * Returns Packet buffer pointer
 */
static inline void *cvm_oct_get_buffer_ptr(union cvmx_buf_ptr pd)
{
	return phys_to_virt(((pd.s.addr >> 7) - pd.s.back) << 7);
}

static inline struct sk_buff **cvm_oct_packet_to_skb(void *packet)
{
	char *p = packet;
	return (struct sk_buff **)(p - NET_SKB_PAD - sizeof(void *));
}
