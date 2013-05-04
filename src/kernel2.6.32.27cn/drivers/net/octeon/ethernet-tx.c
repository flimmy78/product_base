/*********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2010 Cavium Networks
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <net/dst.h>
#ifdef CONFIG_XFRM
#include <linux/xfrm.h>
#include <net/xfrm.h>
#endif /* CONFIG_XFRM */

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-wqe.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>
#include <asm/octeon/cvmx-helper.h>

#include <asm/octeon/cvmx-gmxx-defs.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-tx.h"
#include "ethernet-util.h"


/* ipfwd_learn modle handler */
/* ipfwd_learn add dev_type to identify the caller of different net_device driver 20121127*/
int (*cvm_ipfwd_learn_tx_hook)(struct sk_buff *,uint8_t dev_type) = NULL;
EXPORT_SYMBOL(cvm_ipfwd_learn_tx_hook);

/*
 * You can define GET_SKBUFF_QOS() to override how the skbuff output
 * function determines which output queue is used. The default
 * implementation always uses the base queue for the port. If, for
 * example, you wanted to use the skb->priority fieid, define
 * GET_SKBUFF_QOS as: #define GET_SKBUFF_QOS(skb) ((skb)->priority)
 */
#ifndef GET_SKBUFF_QOS
#define GET_SKBUFF_QOS(skb) 0
#else

#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
#error "GET_SKBUFF_QOS doesn't work with LOCKLESS"
#endif

#endif

#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
#define CVM_OCT_LOCKLESS 1
#include "ethernet-xmit.c"
#endif

#undef CVM_OCT_LOCKLESS
#include "ethernet-xmit.c"

/**
 * cvm_oct_transmit_qos - transmit a work queue entry out of the ethernet port.
 *
 * Both the work queue entry and the packet data can optionally be
 * freed. The work will be freed on error as well.
 *
 * @dev: Device to transmit out.
 * @work_queue_entry: Work queue entry to send
 * @do_free: True if the work queue entry and packet data should be
 *           freed. If false, neither will be freed.
 * @qos: Index into the queues for this port to transmit on. This is
 *       used to implement QoS if their are multiple queues per
 *       port. This parameter must be between 0 and the number of
 *       queues per port minus 1. Values outside of this range will be
 *       change to zero.
 *
 * Returns Zero on success, negative on failure.
 */
int cvm_oct_transmit_qos(struct net_device *dev,
			 void *work_queue_entry,
			 int do_free,
			 int qos)
{
	unsigned long			flags;
	cvmx_buf_ptr_t			hw_buffer;
	cvmx_pko_command_word0_t	pko_command;
	int				dropped;
	struct octeon_ethernet		*priv = netdev_priv(dev);
	cvmx_wqe_t			*work = work_queue_entry;
	cvmx_pko_lock_t lock_type;

	if (!(dev->flags & IFF_UP)) {
		DEBUGPRINT("%s: Device not up\n", dev->name);
		if (do_free)
			cvm_oct_free_work(work);
		return -1;
	}

	if (priv->tx_lockless) {
		qos = cvmx_get_core_num();
		lock_type = CVMX_PKO_LOCK_NONE;
	} else {
		/*
		 * The check on CVMX_PKO_QUEUES_PER_PORT_* is designed to
		 * completely remove "qos" in the event neither interface
		 * supports multiple queues per port
		 */
		if ((CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 > 1) ||
			(CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 > 1)) {
			if (qos <= 0)
				qos = 0;
			else if (qos >= priv->num_tx_queues)
				qos = 0;
		} else
			qos = 0;
		lock_type = CVMX_PKO_LOCK_CMD_QUEUE;
	}

	/* Start off assuming no drop */
	dropped = 0;

	local_irq_save(flags);

	cvmx_pko_send_packet_prepare_pkoid(priv->pko_port, priv->tx_queue[qos].queue, lock_type);

	/* Build the PKO buffer pointer */
	hw_buffer.u64 = 0;
	hw_buffer.s.addr = work->packet_ptr.s.addr;
	hw_buffer.s.pool = CVMX_FPA_PACKET_POOL;
	hw_buffer.s.size = CVMX_FPA_PACKET_POOL_SIZE;
	hw_buffer.s.back = work->packet_ptr.s.back;

	/* Build the PKO command */
	pko_command.u64 = 0;
	pko_command.s.n2 = 1; /* Don't pollute L2 with the outgoing packet */
	pko_command.s.dontfree = !do_free;
	pko_command.s.segs = work->word2.s.bufs;
	pko_command.s.total_bytes = work->word1.len;

	/* Check if we can use the hardware checksumming */
	if (unlikely(work->word2.s.not_IP || work->word2.s.IP_exc))
		pko_command.s.ipoffp1 = 0;
	else
		pko_command.s.ipoffp1 = sizeof(struct ethhdr) + 1;

	/* Send the packet to the output queue */
	if (unlikely(cvmx_pko_send_packet_finish_pkoid(priv->pko_port, priv->tx_queue[qos].queue, pko_command, hw_buffer, lock_type))) {
		DEBUGPRINT("%s: Failed to send the packet\n", dev->name);
		dropped = -1;
	}
	local_irq_restore(flags);

	if (unlikely(dropped)) {
		if (do_free)
			cvm_oct_free_work(work);
		dev->stats.tx_dropped++;
	} else
	if (do_free)
		cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));

	return dropped;
}
EXPORT_SYMBOL(cvm_oct_transmit_qos);

