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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/of_net.h>
#include <linux/fs.h>
#include <linux/autelan_product.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>


#include <net/dst.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-ipd.h>
#include <asm/octeon/cvmx-srio.h>
#include <asm/octeon/cvmx-helper.h>

#include <asm/octeon/cvmx-gmxx-defs.h>
#include <asm/octeon/cvmx-smix-defs.h>
#include <asm/octeon/cvmx-csr.h>   /* for cvmx_pip_port_cfg_t */

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-proc.h"
#include "ethernet-mem.h"
#include "ethernet-rx.h"
#include "ethernet-tx.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"
#include "ethernet-rpa.h"
#include "se_agent_def.h"
#include "ethernet-ioctl.h"
extern struct ethtool_ops cvm_oct_ethtool_ops;
extern port_counter_t cvm_stats[TOTAL_NUMBER_OF_PORTS];
 
int se_coexist_flag = SE_NO_EXIST;
int use_skbuffs_in_hw_flag = 1;





module_param(se_coexist_flag, int, 0444);
MODULE_PARM_DESC(se_coexist_flag, "\n"
		"\t\tSE fast-fwd if exist or not.");


#if defined(CONFIG_OCTEON_NUM_PACKET_BUFFERS) \
	&& CONFIG_OCTEON_NUM_PACKET_BUFFERS
int num_packet_buffers = CONFIG_OCTEON_NUM_PACKET_BUFFERS;
#else
int num_packet_buffers = 1024;
#endif
module_param(num_packet_buffers, int, 0444);
MODULE_PARM_DESC(num_packet_buffers, "\n"
	"\tNumber of packet buffers to allocate and store in the\n"
	"\tFPA. By default, 1024 packet buffers are used unless\n"
	"\tCONFIG_OCTEON_NUM_PACKET_BUFFERS is defined.");
int pend_cvm = 1;
module_param(pend_cvm, int, 0644);

int pow_receive_group = 15;
module_param(pow_receive_group, int, 0444);
MODULE_PARM_DESC(pow_receive_group, "\n"
       "\tPOW group to receive packets from. All ethernet hardware\n"
       "\twill be configured to send incomming packets to this POW\n"
       "\tgroup. Also any other software can submit packets to this\n"
       "\tgroup for the kernel to process.");

int pow_receive_fccp_group = 14;
module_param(pow_receive_fccp_group, int, 0444);
MODULE_PARM_DESC(pow_receive_fccp_group, "\n"
		"\t\tPOW group to receive fccp packets from.\n");


static int disable_core_queueing = 1;
module_param(disable_core_queueing, int, 0444);
MODULE_PARM_DESC(disable_core_queueing, "\n"
		"\t\tWhen set the networking core's tx_queue_len is set to zero.  This\n"
		"\t\tallows packets to be sent without lock contention in the packet scheduler\n"
		"\t\tresulting in some cases in improved throughput.");

int max_rx_cpus = -1;
module_param(max_rx_cpus, int, 0444);
MODULE_PARM_DESC(max_rx_cpus, "\n"
	"\t\tThe maximum number of CPUs to use for packet reception.\n"
	"\t\tUse -1 to use all available CPUs.");

int rx_napi_weight = 32;
module_param(rx_napi_weight, int, 0444);
MODULE_PARM_DESC(rx_napi_weight, "The NAPI WEIGHT parameter.");

unsigned int base_local_slot_num;
static char *obc_name = "obc";
static unsigned char obc_mac_base[6] = {0x00, 0x1f, 0x64, 0xff, 0xff, 0xf0};

struct sock *nl_sk = NULL;
struct sock *se_agent_nl_sock = NULL;

//#define NETLINK_OCTEON 21
 /* for 12c salve cpu targe mode */
#ifdef ETHERPCI
#include <asm/octeon/cvmx-sli-defs.h>
#include <asm/octeon/cvmx-pexp-defs.h>
#include <asm/octeon/cvmx-dma-engine.h>
int txcomp_port = -1;
#endif
/**
 * CVM_RPA global net device table and arp-reply send net device on RPA
 * 
 */
cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
EXPORT_SYMBOL(cvm_rpa_dev_index);
EXPORT_SYMBOL(rpa_arp_send_dev);

/**
 * cvm_oct_poll_queue - Workqueue for polling operations.
 */
struct workqueue_struct *cvm_oct_poll_queue;

/**
 * cvm_oct_poll_queue_stopping - flag to indicate polling should stop.
 *
 * Set to one right before cvm_oct_poll_queue is destroyed.
 */
atomic_t cvm_oct_poll_queue_stopping = ATOMIC_INIT(0);

/*
 * cvm_oct_by_pkind is an array of every ethernet device owned by this
 * driver indexed by the IPD pkind/port_number.  If an entry is empty
 * (NULL) it either doesn't exist, or there was a collision.  The two
 * cases can be distinguished by trying to look up via
 * cvm_oct_dev_for_port();
 */
struct octeon_ethernet *cvm_oct_by_pkind[64] __cacheline_aligned;

/*
 * cvm_oct_by_srio_mbox is indexed by the SRIO mailbox.
 */
struct octeon_ethernet *cvm_oct_by_srio_mbox[4][4];

/*
 * cvm_oct_list is a list of all cvm_oct_private_t created by this driver.
 */
LIST_HEAD(cvm_oct_list);

extern product_info_t autelan_product_info;

extern int oct_init(void);
extern int oct_cleanup(void);
extern int cvm_get_local_mac_addr(unsigned char *mac_addr);
static void cvm_oct_rx_refill_worker(struct work_struct *work);
static DECLARE_DELAYED_WORK(cvm_oct_rx_refill_work, cvm_oct_rx_refill_worker);
int netlink_sendto_se_agent(char *buff,unsigned int len)
{	
	struct sk_buff *skb=NULL;	
	struct nlmsghdr *nlh=NULL;
	control_cmd_t  *cmd_buf;
	pid_t se_agent_pid;
	int rval;
	if(buff==NULL)
	{
		printk("netlink_sendto_se_agent param error!\n");
		return -1;
	}
	cmd_buf=(control_cmd_t *)buff;
	se_agent_pid=cmd_buf->agent_pid;
	if(se_agent_pid <0)
	{
		printk("netlink_sendto_se_agent  se_agent_pid error\n");
		return -1;
	}
	if((skb=alloc_skb(NLMSG_SPACE(len),GFP_ATOMIC))==NULL)
	{ 	
		printk("netlink_sendto_se_agent alloc_skb failed.\n");		
		return -1;	
	}
	skb_get(skb);
	nlh=(struct nlmsghdr*)skb->data;
	nlh->nlmsg_len=NLMSG_SPACE(len);	
	nlh->nlmsg_pid=0;	
	nlh=(struct nlmsghdr*)skb_put(skb,NLMSG_SPACE(len));
	memcpy(NLMSG_DATA(nlh),cmd_buf,NLMSG_SPACE(len));
	NETLINK_CB(skb).pid=0;	
	NETLINK_CB(skb).dst_group=0;
	rval=netlink_unicast(se_agent_nl_sock,skb,se_agent_pid,MSG_DONTWAIT);
	if(rval<0)
	{
		printk("netlink_sendto_se_agent netlink unicast error\n");
	}
	kfree_skb(skb);
	return rval;
}

static void cvm_oct_rx_refill_worker(struct work_struct *work)
{
	/*
	 * FPA 0 may have been drained, try to refill it if we need
	 * more than num_packet_buffers / 2, otherwise normal receive
	 * processing will refill it.  If it were drained, no packets
	 * could be received so cvm_oct_napi_poll would never be
	 * invoked to do the refill.
	 */
	cvm_oct_rx_refill_pool(num_packet_buffers / 2);

	if (!atomic_read(&cvm_oct_poll_queue_stopping))
		queue_delayed_work(cvm_oct_poll_queue,
				   &cvm_oct_rx_refill_work, HZ);
}

static void cvm_oct_periodic_worker(struct work_struct *work)
{
	struct octeon_ethernet *priv = container_of(work,
						    struct octeon_ethernet,
						    port_periodic_work.work);

	if (priv->poll)
		priv->poll(priv->netdev);

	priv->netdev->netdev_ops->ndo_get_stats(priv->netdev);

	if (!atomic_read(&cvm_oct_poll_queue_stopping))
		queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
 }

static int cvm_oct_num_output_buffers;

static __init void cvm_oct_configure_common_hw(void)
{
	if(se_coexist_flag == SE_COEXIST)
	{
		cvmx_fau_atomic_write64(CVM_FAU_INIT_STATE_FLAG, FPA_INIT_WAIT);
		CVMX_SYNCWS;          
		while(cvmx_fau_fetch_and_add64(CVM_FAU_INIT_STATE_FLAG, 0) != FPA_INIT_OK); 
		printk(KERN_NOTICE "SE init fpa ok!\n");
	}
	else
	{
		/* Setup the FPA */
		cvmx_fpa_enable();
		cvm_oct_mem_fill_fpa(CVMX_FPA_PACKET_POOL, CVMX_FPA_PACKET_POOL_SIZE,
				     num_packet_buffers);
		cvm_oct_mem_fill_fpa(CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE,
				     num_packet_buffers);
		if (CVMX_FPA_OUTPUT_BUFFER_POOL != CVMX_FPA_PACKET_POOL) {
			cvm_oct_num_output_buffers = 4 * octeon_pko_get_total_queues();
			cvm_oct_mem_fill_fpa(CVMX_FPA_OUTPUT_BUFFER_POOL,
					     CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, cvm_oct_num_output_buffers);
		}

		if (USE_RED)
			cvmx_helper_setup_red(num_packet_buffers / 4,
					      num_packet_buffers / 8);
	}

}

/**
 * cvm_oct_register_callback -  Register a intercept callback for the named device.
 *
 * It returns the net_device structure for the ethernet port. Usign a
 * callback of NULL will remove the callback. Note that this callback
 * must not disturb scratch. It will be called with SYNCIOBDMAs in
 * progress and userspace may be using scratch. It also must not
 * disturb the group mask.
 *
 * @device_name: Device name to register for. (Example: "eth0")
 * @callback: Intercept callback to set.
 *
 * Returns the net_device structure for the ethernet port or NULL on failure.
 */
struct net_device *cvm_oct_register_callback(const char *device_name, cvm_oct_callback_t callback)
{
	struct octeon_ethernet *priv;

	list_for_each_entry(priv, &cvm_oct_list, list) {
		if (strcmp(device_name, priv->netdev->name) == 0) {
			priv->intercept_cb = callback;
			wmb();
			return priv->netdev;
		}
	}
	return NULL;
}
EXPORT_SYMBOL(cvm_oct_register_callback);

/**
 * cvm_oct_free_work- Free a work queue entry
 *
 * @work_queue_entry: Work queue entry to free
 *
 * Returns Zero on success, Negative on failure.
 */
int cvm_oct_free_work(void *work_queue_entry)
{
	cvmx_wqe_t *work = work_queue_entry;

	int segments = work->word2.s.bufs;
	union cvmx_buf_ptr segment_ptr = work->packet_ptr;

	while (segments--) {
		union cvmx_buf_ptr next_ptr = *(union cvmx_buf_ptr *)phys_to_virt(segment_ptr.s.addr - 8);
		if (unlikely(!segment_ptr.s.i))
			cvmx_fpa_free(cvm_oct_get_buffer_ptr(segment_ptr),
				      segment_ptr.s.pool,
				      DONT_WRITEBACK(CVMX_FPA_PACKET_POOL_SIZE / 128));
		segment_ptr = next_ptr;
	}
	cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));

	return 0;
}
EXPORT_SYMBOL(cvm_oct_free_work);

/* Lock to protect racy cvmx_pko_get_port_status() */
static DEFINE_SPINLOCK(cvm_oct_tx_stat_lock);





static void cvm_oct_common_show_stats
(
    struct net_device *dev,
    cvmx_pip_port_status_t rx_status,
    cvmx_pko_port_status_t tx_status
)
{
	
    cvm_stats[dev->ifindex].pip_stats.dropped_octets  += rx_status.dropped_octets;         /**< Inbound octets marked to be dropped by the IPD */
    cvm_stats[dev->ifindex].pip_stats.dropped_packets += rx_status.dropped_packets;        /**< Inbound packets marked to be dropped by the IPD */
    cvm_stats[dev->ifindex].pip_stats.pci_raw_packets += rx_status.pci_raw_packets;        /**< RAW PCI Packets received by PIP per port */
    cvm_stats[dev->ifindex].pip_stats.octets          += rx_status.octets;                 /**< Number of octets processed by PIP */
    cvm_stats[dev->ifindex].pip_stats.packets += rx_status.packets;                        /**< Number of packets processed by PIP */
    cvm_stats[dev->ifindex].pip_stats.multicast_packets += rx_status.multicast_packets;    /**< Number of indentified L2 multicast packets.
                                                                                                Does not include broadcast packets.
                                                                                                Only includes packets whose parse mode is
                                                                                                SKIP_TO_L2 */
    cvm_stats[dev->ifindex].pip_stats.broadcast_packets += rx_status.broadcast_packets;    /**< Number of indentified L2 broadcast packets.
                                                                                                Does not include multicast packets.
                                                                                                Only includes packets whose parse mode is
                                                                                                SKIP_TO_L2 */
    cvm_stats[dev->ifindex].pip_stats.len_64_packets += rx_status.len_64_packets;                /**< Number of 64B packets */
    cvm_stats[dev->ifindex].pip_stats.len_65_127_packets += rx_status.len_65_127_packets;        /**< Number of 65-127B packets */
    cvm_stats[dev->ifindex].pip_stats.len_128_255_packets += rx_status.len_128_255_packets;      /**< Number of 128-255B packets */
    cvm_stats[dev->ifindex].pip_stats.len_256_511_packets += rx_status.len_256_511_packets;      /**< Number of 256-511B packets */
    cvm_stats[dev->ifindex].pip_stats.len_512_1023_packets+= rx_status.len_512_1023_packets;     /**< Number of 512-1023B packets */
    cvm_stats[dev->ifindex].pip_stats.len_1024_1518_packets += rx_status.len_1024_1518_packets;  /**< Number of 1024-1518B packets */
    cvm_stats[dev->ifindex].pip_stats.len_1519_max_packets += rx_status.len_1519_max_packets;    /**< Number of 1519-max packets */
    cvm_stats[dev->ifindex].pip_stats.fcs_align_err_packets += rx_status.fcs_align_err_packets;  /**< Number of packets with FCS or Align opcode errors */
    cvm_stats[dev->ifindex].pip_stats.runt_packets += rx_status.runt_packets;                    /**< Number of packets with length < min */
    cvm_stats[dev->ifindex].pip_stats.runt_crc_packets += rx_status.runt_crc_packets;            /**< Number of packets with length < min and FCS error */
    cvm_stats[dev->ifindex].pip_stats.oversize_packets += rx_status.oversize_packets;            /**< Number of packets with length > max */
    cvm_stats[dev->ifindex].pip_stats.oversize_crc_packets += rx_status.oversize_crc_packets;    /**< Number of packets with length > max and FCS error */
    cvm_stats[dev->ifindex].pip_stats.inb_packets += rx_status.inb_packets;                      /**< Number of packets without GMX/SPX/PCI errors received by PIP */
    cvm_stats[dev->ifindex].pip_stats.inb_octets += rx_status.inb_octets;                        /**< Total number of octets from all packets received by PIP, including CRC */
    cvm_stats[dev->ifindex].pip_stats.inb_errors += rx_status.inb_errors;

	cvm_stats[dev->ifindex].pko_stats.packets += tx_status.packets;
	cvm_stats[dev->ifindex].pko_stats.octets += tx_status.octets;
	cvm_stats[dev->ifindex].pko_stats.doorbell += tx_status.doorbell;

	return;
}



/**
 * cvm_oct_common_get_stats - get the low level ethernet statistics
 * @dev:    Device to get the statistics from
 *
 * Returns Pointer to the statistics
 */
static struct net_device_stats *cvm_oct_common_get_stats(struct net_device *dev)
{
	unsigned long flags;
	cvmx_pip_port_status_t rx_status;
	cvmx_pko_port_status_t tx_status;
	u64 current_tx_octets;
	u32 current_tx_packets;
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (octeon_is_simulation()) {
		/* The simulator doesn't support statistics */
		memset(&rx_status, 0, sizeof(rx_status));
		memset(&tx_status, 0, sizeof(tx_status));
	} else {
		cvmx_pip_get_port_status(priv->ipd_port, 1, &rx_status);

		spin_lock_irqsave(&cvm_oct_tx_stat_lock, flags);
		cvmx_pko_get_port_status(priv->ipd_port, 0, &tx_status);
		current_tx_packets = tx_status.packets;
		current_tx_octets = tx_status.octets;
		/*
		 * The tx_packets counter is 32-bits as are all these
		 * variables.  No truncation necessary.
		 */
		tx_status.packets = current_tx_packets - priv->last_tx_packets;
		/*
		 * The tx_octets counter is only 48-bits, so we need
		 * to truncate in case there was a wrap-around
		 */
		tx_status.octets = (current_tx_octets - priv->last_tx_octets) & 0xffffffffffffull;
		priv->last_tx_packets = current_tx_packets;
		priv->last_tx_octets = current_tx_octets;
		spin_unlock_irqrestore(&cvm_oct_tx_stat_lock, flags);
	}

	dev->stats.rx_packets += rx_status.inb_packets;
	dev->stats.tx_packets += tx_status.packets;
	dev->stats.rx_bytes += rx_status.inb_octets;
	dev->stats.tx_bytes += tx_status.octets;
	dev->stats.multicast += rx_status.multicast_packets;
	dev->stats.rx_crc_errors += rx_status.inb_errors;
	dev->stats.rx_frame_errors += rx_status.fcs_align_err_packets;

	/*
	 * The drop counter must be incremented atomically since the
	 * RX tasklet also increments it.
	 */
	atomic64_add(rx_status.dropped_packets,
		     (atomic64_t *)&dev->stats.rx_dropped);
	cvm_oct_common_show_stats(dev, rx_status, tx_status);

	return &dev->stats;
}

/**
 * cvm_oct_common_change_mtu - change the link MTU
 * @dev:     Device to change
 * @new_mtu: The new MTU
 *
 * Returns Zero on success
 */
static int cvm_oct_common_change_mtu(struct net_device *dev, int new_mtu)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
	int vlan_bytes = 4;
#else
	int vlan_bytes = 0;
#endif
	int rpa_bytes = 18;

	/*
	 * Limit the MTU to make sure the ethernet packets are between
	 * 64 bytes and 65535 bytes.
	 */
	if ((new_mtu + 14 + 4 + vlan_bytes + rpa_bytes < 64)
	    || (new_mtu + 14 + 4 + vlan_bytes + rpa_bytes > 65392)) {
		pr_err("MTU must be between %d and %d.\n",
		       64 - 14 - 4 - vlan_bytes - rpa_bytes, 65392 - 14 - 4 - vlan_bytes - rpa_bytes);
		return -EINVAL;
	}
	dev->mtu = new_mtu;

	if (priv->has_gmx_regs) {
		/* Add ethernet header and FCS, and VLAN if configured. */
		int max_packet = new_mtu + 14 + 4 + vlan_bytes + rpa_bytes;

		if (OCTEON_IS_MODEL(OCTEON_CN3XXX)
		    || OCTEON_IS_MODEL(OCTEON_CN58XX)) {
			/* Signal errors on packets larger than the MTU */
			cvmx_write_csr(CVMX_GMXX_RXX_FRM_MAX(priv->interface_port, priv->interface),
				       max_packet);
		} else {
			/*
			 * Set the hardware to truncate packets larger
			 * than the MTU and smaller the 64 bytes.
			 */
			union cvmx_pip_frm_len_chkx frm_len_chk;
			frm_len_chk.u64 = 0;
			frm_len_chk.s.minlen = 64;
			frm_len_chk.s.maxlen = max_packet;
			cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(priv->interface), frm_len_chk.u64);
		}
		/*
		 * Set the hardware to truncate packets larger than
		 * the MTU. The jabber register must be set to a
		 * multiple of 8 bytes, so round up.
		 */
		/*
		cvmx_write_csr(CVMX_GMXX_RXX_JABBER(priv->interface_port, priv->interface),
			       (max_packet + 7) & ~7u);
		*/
		/* Set the larger frame len check on interface */
		cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(priv->interface), 0x6100040);
	}
	return 0;
}

/**
 * cvm_oct_common_set_multicast_list - set the multicast list
 * @dev:    Device to work on
 */
static void cvm_oct_common_set_multicast_list(struct net_device *dev)
{
	union cvmx_gmxx_prtx_cfg gmx_cfg;
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->has_gmx_regs) {
		union cvmx_gmxx_rxx_adr_ctl control;
		control.u64 = 0;
		control.s.bcst = 1;	/* Allow broadcast MAC addresses */

		if (dev->mc_list || (dev->flags & IFF_ALLMULTI) ||
		    (dev->flags & IFF_PROMISC))
			/* Force accept multicast packets */
			control.s.mcst = 2;
		else
			/* Force reject multicat packets */
			control.s.mcst = 1;

		if (dev->flags & IFF_PROMISC)
			/*
			 * Reject matches if promisc. Since CAM is
			 * shut off, should accept everything.
			 */
			control.s.cam_mode = 0;
		else
			/* Filter packets based on the CAM */
			control.s.cam_mode = 1;

		gmx_cfg.u64 =
		    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface));
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface),
			       gmx_cfg.u64 & ~1ull);

		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CTL(priv->interface_port, priv->interface),
			       control.u64);
		if (dev->flags & IFF_PROMISC)
			cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN
				       (priv->interface_port, priv->interface), 0);
		else
			cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN
				       (priv->interface_port, priv->interface), 1);

		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface),
			       gmx_cfg.u64);
	}
}

/**
 * cvm_oct_common_set_mac_address - set the hardware MAC address for a device
 * @dev:    The device in question.
 * @addr:   Address structure to change it too.

 * Returns Zero on success
 */
static int cvm_oct_common_set_mac_address(struct net_device *dev, void *addr)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	union cvmx_gmxx_prtx_cfg gmx_cfg;

	memcpy(dev->dev_addr, addr + 2, 6);

	if (priv->has_gmx_regs) {
		int i;
		u8 *ptr = addr;
		u64 mac = 0;
		for (i = 0; i < 6; i++)
			mac = (mac << 8) | (u64) (ptr[i + 2]);

		gmx_cfg.u64 =
		    cvmx_read_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface));
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface),
			       gmx_cfg.u64 & ~1ull);

		cvmx_write_csr(CVMX_GMXX_SMACX(priv->interface_port, priv->interface), mac);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM0(priv->interface_port, priv->interface), ptr[2]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM1(priv->interface_port, priv->interface), ptr[3]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM2(priv->interface_port, priv->interface), ptr[4]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM3(priv->interface_port, priv->interface), ptr[5]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM4(priv->interface_port, priv->interface), ptr[6]);
		cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM5(priv->interface_port, priv->interface), ptr[7]);
		cvm_oct_common_set_multicast_list(dev);
		cvmx_write_csr(CVMX_GMXX_PRTX_CFG(priv->interface_port, priv->interface), gmx_cfg.u64);
	}
	return 0;
}

/**
 * cvm_oct_common_init - per network device initialization
 * @dev:    Device to initialize
 *
 * Returns Zero on success
 */
int cvm_oct_common_init(struct net_device *dev)
{
	unsigned long flags;
	cvmx_pko_port_status_t tx_status;
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct sockaddr sa = {0};
	const u8 *mac = NULL;
	unsigned char mac_addr[6];
	unsigned int new_flags;

	if (autelan_product_info.distributed_product && (!(autelan_product_info.board_slot_id < 0))){
		if (!memcmp(dev->name, obc_name, 3)){
			obc_mac_base[5] = 0xf0;
			obc_mac_base[5] += (autelan_product_info.board_slot_id - 0x1);
			mac = (const u8 *)&obc_mac_base;
		}
		else if(0 == cvm_get_local_mac_addr(mac_addr)){
			mac = (const u8 *)&mac_addr;
		}
	}
	else if (priv->of_node)
		mac = of_get_mac_address(priv->of_node);

	if (mac)
		memcpy(sa.sa_data, mac, ETH_ALEN);
	else
		random_ether_addr(sa.sa_data);

	if (priv->num_tx_queues) {
		dev->features |= NETIF_F_SG | NETIF_F_FRAGLIST;
		if (USE_HW_TCPUDP_CHECKSUM)
			dev->features |= NETIF_F_IP_CSUM;
	}

	/* We do our own locking, Linux doesn't need to */
	dev->features |= NETIF_F_LLTX;
	SET_ETHTOOL_OPS(dev, &cvm_oct_ethtool_ops);
	new_flags = dev->flags | IFF_PROMISC;
	dev_change_flags(dev, new_flags);

	cvm_oct_phy_setup_device(dev);
	dev->netdev_ops->ndo_set_mac_address(dev, &sa);
	dev->netdev_ops->ndo_change_mtu(dev, dev->mtu);

	spin_lock_irqsave(&cvm_oct_tx_stat_lock, flags);
	cvmx_pko_get_port_status(priv->ipd_port, 0, &tx_status);
	priv->last_tx_packets = tx_status.packets;
	priv->last_tx_octets = tx_status.octets;
	/*
	 * Zero out stats for port so we won't mistakenly show
	 * counters from the bootloader.
	 */
	memset(&dev->stats, 0, sizeof(struct net_device_stats));
	spin_unlock_irqrestore(&cvm_oct_tx_stat_lock, flags);

	return 0;
}

void cvm_oct_common_uninit(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->phydev)
		phy_disconnect(priv->phydev);
}

static const struct net_device_ops cvm_oct_npi_netdev_ops = {
	.ndo_init		= cvm_oct_common_init,
	.ndo_uninit		= cvm_oct_common_uninit,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};

static const struct net_device_ops cvm_oct_xaui_netdev_ops = {
	.ndo_init		= cvm_oct_xaui_init,
	.ndo_uninit		= cvm_oct_xaui_uninit,
	.ndo_open		= cvm_oct_xaui_open,
	.ndo_stop		= cvm_oct_xaui_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_sgmii_netdev_ops = {
	.ndo_init		= cvm_oct_sgmii_init,
	.ndo_uninit		= cvm_oct_sgmii_uninit,
	.ndo_open		= cvm_oct_sgmii_open,
	.ndo_stop		= cvm_oct_sgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_spi_netdev_ops = {
	.ndo_init		= cvm_oct_spi_init,
	.ndo_uninit		= cvm_oct_spi_uninit,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_rgmii_netdev_ops = {
	.ndo_init		= cvm_oct_rgmii_init,
	.ndo_uninit		= cvm_oct_rgmii_uninit,
	.ndo_open		= cvm_oct_rgmii_open,
	.ndo_stop		= cvm_oct_rgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
static const struct net_device_ops cvm_oct_xaui_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_xaui_init,
	.ndo_uninit		= cvm_oct_xaui_uninit,
	.ndo_open		= cvm_oct_xaui_open,
	.ndo_stop		= cvm_oct_xaui_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_sgmii_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_sgmii_init,
	.ndo_uninit		= cvm_oct_sgmii_uninit,
	.ndo_open		= cvm_oct_sgmii_open,
	.ndo_stop		= cvm_oct_sgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_spi_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_spi_init,
	.ndo_uninit		= cvm_oct_spi_uninit,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
static const struct net_device_ops cvm_oct_rgmii_lockless_netdev_ops = {
	.ndo_init		= cvm_oct_rgmii_init,
	.ndo_uninit		= cvm_oct_rgmii_uninit,
	.ndo_open		= cvm_oct_rgmii_open,
	.ndo_stop		= cvm_oct_rgmii_stop,
	.ndo_start_xmit		= cvm_oct_xmit_lockless,
	.ndo_set_multicast_list	= cvm_oct_common_set_multicast_list,
	.ndo_set_mac_address	= cvm_oct_common_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_common_change_mtu,
	.ndo_get_stats		= cvm_oct_common_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#endif
#ifdef CONFIG_RAPIDIO
static const struct net_device_ops cvm_oct_srio_netdev_ops = {
	.ndo_init		= cvm_oct_srio_init,
	.ndo_start_xmit		= cvm_oct_xmit_srio,
	.ndo_set_mac_address	= cvm_oct_srio_set_mac_address,
	.ndo_do_ioctl		= cvm_oct_ioctl,
	.ndo_change_mtu		= cvm_oct_srio_change_mtu,
	.ndo_get_stats		= cvm_oct_srio_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= cvm_oct_poll_controller,
#endif
};
#endif

extern void octeon_mdiobus_force_mod_depencency(void);

static int num_devices_extra_wqe;
#define PER_DEVICE_EXTRA_WQE (MAX_OUT_QUEUE_DEPTH)

static void cvm_oct_override_pko_queue_priority(int pko_port, u64 priorities[16])
{
	int i;

	if (octeon_pko_lockless()) {
		for (i = 0; i < 16; i++)
			priorities[i] = 8;
	}
}

static struct rb_root cvm_oct_ipd_tree = RB_ROOT;

void cvm_oct_add_ipd_port(struct octeon_ethernet *port)
{
	struct rb_node **link = &cvm_oct_ipd_tree.rb_node;
	struct rb_node *parent = NULL;
	struct octeon_ethernet *n;
	int value = port->key;

	while (*link) {
		parent = *link;
		n = rb_entry(parent, struct octeon_ethernet, ipd_tree);

		if (value < n->key)
			link = &(*link)->rb_left;
		else if (value > n->key)
			link = &(*link)->rb_right;
		else
			BUG();
	}
	rb_link_node(&port->ipd_tree, parent, link);
	rb_insert_color(&port->ipd_tree, &cvm_oct_ipd_tree);
}

struct octeon_ethernet *cvm_oct_dev_for_port(int port_number)
{
	struct rb_node *n = cvm_oct_ipd_tree.rb_node;
	while (n) {
		struct octeon_ethernet *s = rb_entry(n, struct octeon_ethernet, ipd_tree);

		if (s->key > port_number)
			n = n->rb_left;
		else if (s->key < port_number)
			n = n->rb_left;
		else
			return s;
	}
	return NULL;
}

static struct device_node * __init cvm_oct_of_get_child(const struct device_node *parent,
							int reg_val)
{
	struct device_node *node = NULL;
	int size;
	const __be32 *addr;

	for (;;) {
		node = of_get_next_child(parent, node);
		if (!node)
			break;
		addr = of_get_property(node, "reg", &size);
		if (addr && (be32_to_cpu(*addr) == reg_val))
			break;
	}
	return node;
}

static struct device_node * __init cvm_oct_node_for_port(struct device_node *pip,
							 int interface, int port)
{
	struct device_node *ni, *np;

	ni = cvm_oct_of_get_child(pip, interface);
	if (!ni)
		return NULL;

	np = cvm_oct_of_get_child(ni, port);
	of_node_put(ni);

	return np;
}

/* Add for qos config */
static void oct_qos_queue_config(void)
{
	cvmx_pow_pp_grp_mskx_t grp_msk;		
	cvmx_ciu_fuse_t fuse;		
	uint64_t fuse_tmp = 0;	
	int core = 0;		
	int i = 0;

    /* add for TIPC qos watch. TIPC pkt goto grp15,qos0. add by zhaohan */
    {
    	cvmx_pip_qos_watchx_t qos_watch;		
        qos_watch.u64 = 0;
        qos_watch.cn56xx.match_type = 4;  /* match Ether type，CN58XX is not support */
        qos_watch.cn56xx.match_value = 0x88ca; /* TIPC mark */
        qos_watch.cn56xx.qos = 0; /* high priority */
        qos_watch.cn56xx.mask = 0x0000;
        cvmx_write_csr(CVMX_PIP_QOS_WATCHX(0), qos_watch.u64);  /* PIP_QOS_WATCH0 0x00011800A0000100 */
    }

    /* config qos vlan */
    {
        cvmx_pip_qos_vlanx_t qos_vlan;		
        qos_vlan.u64 = 0;
        qos_vlan.s.qos = 2;
        
        for(i = 0; i < 8; i++)
        {
            cvmx_write_csr(CVMX_PIP_QOS_VLANX(i), qos_vlan.u64);    /* PIP_QOS_VLAN0 0x00011800A00000C0 */
        }
    }

    /* setup RED by qos */
    {
		/* TIPC & fast_fwd */
        cvmx_helper_setup_red_queue(0, 200, 100);
        /* Untag packet */
		cvmx_helper_setup_red_queue(1, 200, 100);
        /* Tag packet */
        cvmx_helper_setup_red_queue(2, 500, 400);
    }

	/* set qos level to all cores */	
	fuse.u64 = cvmx_read_csr(CVMX_CIU_FUSE);
	fuse_tmp = fuse.s.fuse;
	do{
		/*优先级0最高，queue 1是报文对应的QOS队列，优先级低于控制报文的QOS队列*/
		grp_msk.u64 = cvmx_read_csr(CVMX_POW_PP_GRP_MSKX(core));
		grp_msk.s.qos1_pri = 1;
		grp_msk.s.qos2_pri = 2; /* SE => linux, send debug info, ues qos 2,  add by zhaohan*/
		cvmx_write_csr(CVMX_POW_PP_GRP_MSKX(core), grp_msk.u64);    /* POW_PP_GRP_MSK0 0x0001670000000000 */
		core = core+1;
		fuse_tmp = fuse_tmp >> 1;
	}while(fuse_tmp&0x1);

	/* set pow qos rnd */
	{
		/*配置SSO的QOS调度优先级，保证核间通信的控制消息优先调度
		  每一个rnd表示一次循环是否遍历该QOS队列的WORK，这里有八个位每个位表示一个QOS队列。
		  每个寄存器对应四个RND队列，一共有八个寄存器，所以对应了32个RND循环*/		
		cvmx_pow_qos_rndx_t qosmask;
		qosmask.s.rnd = 0x1;
		qosmask.s.rnd_p1 = 0x3;
		qosmask.s.rnd_p2 = 0x3;
		qosmask.s.rnd_p3 = 0x7;

		for (i = 0; i < 8; i++) 
		{
			cvmx_write_csr(CVMX_POW_QOS_RNDX(i),qosmask.u64);       /* POW_QOS_RND0 0x00016700000001C0 */
		}
	} 
}

/* set packet qos for port */
static void oct_qos_port_config(int port_index)
{
	cvmx_pip_port_cfg_t port_cfg;
	port_cfg.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(port_index));
    /* Have all packets goto POW queue 1, queue 0 is for the messages between the cores*/	
	port_cfg.s.qos = 1;       /* #define DEFAULT_PACKET_SSO_QUEUE 1 */
    port_cfg.s.qos_wat = 1;     /* enable qos_watcher0 qos */
    port_cfg.s.qos_vlan =1;     /* enable qos_vlan bit. qos set by vlan_qos */
	cvmx_write_csr(CVMX_PIP_PRT_CFGX(port_index), port_cfg.u64);	/* PIP_PRT_CFG0 0x00011800A0000200 */
}

static int __init cvm_oct_init_module(void)
{
	int num_interfaces;
	int interface;
	int fau = FAU_NUM_PACKET_BUFFERS_TO_FREE;
	int qos;
	struct device_node *pip;
	int i;
	int rv = 0;
	if(cvmx_fau_fetch_and_add64(CVM_FAU_SE_COEXIST_FLAG, 0) == SE_MAGIC_MUN)
	{
		se_coexist_flag = SE_COEXIST;
		use_skbuffs_in_hw_flag = 0;
	}
	base_local_slot_num = autelan_product_info.board_slot_id - 1;
	/* Init netlink socket in cavium octeon module */
	nl_sk = netlink_kernel_create(&init_net, NETLINK_OCTEON, 0, NULL, NULL, THIS_MODULE);   
	if(!nl_sk) {   
		pr_err("Failed to create netlink socket.\n");   
	}
	else {
		pr_info("Cavium Octeon create netlink socket OK.\n");
	}
	
	
	se_agent_nl_sock = netlink_kernel_create(&init_net, NETLINK_SE_AGENT, 0, NULL, NULL, THIS_MODULE);   
	if(!se_agent_nl_sock) {   
		pr_err("Failed to create netlink socket for se_agent.\n");   
	}

	octeon_mdiobus_force_mod_depencency();
	pr_notice("octeon-ethernet %s\n", OCTEON_ETHERNET_VERSION);
 /* for 12c salve cpu targe mode */
#ifdef ETHERPCI
	pr_notice("EtherPCI Enabled\n");
#endif

	pip = of_find_node_by_path("pip");
	if (!pip) {
		pr_err("Error: No 'pip' in /aliases\n");
		return -EINVAL;
	}

	cvmx_override_pko_queue_priority = cvm_oct_override_pko_queue_priority;

	cvm_oct_poll_queue = create_singlethread_workqueue("octeon-ethernet");
	if (cvm_oct_poll_queue == NULL) {
		pr_err("octeon-ethernet: Cannot create workqueue");
		rv = -ENOMEM;
		goto err_cleanup;
	}

	cvm_oct_proc_initialize();
	cvm_oct_configure_common_hw();

	cvmx_helper_initialize_packet_io_global();

    /* config qos queue for AC board */
	if(autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S)	
	{
	    if(se_coexist_flag != SE_COEXIST)
	    {
		    oct_qos_queue_config();
	    }
	}
	
 /* for 12c salve cpu targe mode */
#ifdef ETHERPCI
	{
		cvmx_wqe_t  wqe;
		cvmx_wqe_set_port(&wqe, txcomp_port);
		txcomp_port = cvmx_wqe_get_port(&wqe);
		CVMX_SYNCWS;
	}
#endif

	/* Change the input group for all ports before input is enabled */
	num_interfaces = cvmx_helper_get_number_of_interfaces();
	for (interface = 0; interface < num_interfaces; interface++) {
		int num_ports = cvmx_helper_ports_on_interface(interface);
		int port;
		for (port = 0; port < num_ports; port++) {
			union cvmx_pip_prt_tagx pip_prt_tagx;
			int pkind;

			if (octeon_has_feature(OCTEON_FEATURE_PKND))
				pkind = cvmx_helper_get_pknd(interface, port);
			else
				pkind = cvmx_helper_get_ipd_port(interface, port);

            /* Config qos prot for AC board */
			if(autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU || \
        		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C || \
        		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C || \
        		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X || \
        		autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || \
        		autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S)
            {
				if(se_coexist_flag != SE_COEXIST)
				{
				    oct_qos_port_config(pkind);
				}
            }
			pip_prt_tagx.u64 = cvmx_read_csr(CVMX_PIP_PRT_TAGX(pkind));
			pip_prt_tagx.s.grp = pow_receive_group;
			cvmx_write_csr(CVMX_PIP_PRT_TAGX(pkind), pip_prt_tagx.u64);
		}
	}

	cvmx_helper_ipd_and_packet_input_enable();
	if(se_coexist_flag == SE_COEXIST)
	{
		cvmx_ipd_disable();
		cvmx_fau_atomic_write64(CVM_FAU_INIT_STATE_FLAG, IPD_EN_OK);
		CVMX_SYNCWS;
	}
	
 /* for 12c salve cpu targe mode */
#if defined(ETHERPCI)
	if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2)) {
		uint64_t  val64 = cvmx_read_csr(CVMX_IOB_INT_ENB);
		pr_err("Disabling NP_DAT NP_SOP & NP_EOP for 56xx Pass2\n");
		val64 &= ~(0x13ULL);
		cvmx_write_csr(CVMX_IOB_INT_ENB, val64);
	}
	cvmx_dma_engine_initialize();
#endif
	/*
	 * Initialize the FAU used for counting packet buffers that
	 * need to be freed.
	 */
	cvmx_fau_atomic_write32(FAU_NUM_PACKET_BUFFERS_TO_FREE, 0);

	if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_SMU)
	{
		num_interfaces = 1;
		for (interface = 0; interface < num_interfaces; interface++) {
			cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
			int num_ports = cvmx_helper_ports_on_interface(interface);
			int interface_port;

			/* Only 3 sgmii ports are available on AX_81_SMU */
			num_ports = (num_ports > 3) ? 3 : num_ports;

			for (interface_port = 0; num_ports > 0;
			     interface_port++, num_ports--) {
				struct octeon_ethernet *priv;
				int base_queue;
				struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
				if (!dev) {
					pr_err("Failed to allocate ethernet device for port %d:%d\n", interface, interface_port);
					continue;
				}

				if (disable_core_queueing)
					dev->tx_queue_len = 0;

				/* Initialize the device private structure. */
				priv = netdev_priv(dev);
				dev->priv = priv;
				priv->of_node = cvm_oct_node_for_port(pip, interface, interface_port);
				RB_CLEAR_NODE(&priv->ipd_tree);
				priv->netdev = dev;
				priv->interface = interface;
				priv->interface_port = interface_port;

				INIT_DELAYED_WORK(&priv->port_periodic_work,
						  cvm_oct_periodic_worker);
				priv->imode = imode;

				priv->ipd_port = cvmx_helper_get_ipd_port(interface, interface_port);
				priv->key = priv->ipd_port;
				priv->pko_port = cvmx_helper_get_pko_port(interface, interface_port);
				base_queue = cvmx_pko_get_base_queue(priv->ipd_port);
				priv->num_tx_queues = cvmx_pko_get_num_queues(priv->ipd_port);
				
				if (octeon_has_feature(OCTEON_FEATURE_PKND))
					priv->ipd_pkind = cvmx_helper_get_pknd(interface, interface_port);
				else
					priv->ipd_pkind = priv->ipd_port;

				for (qos = 0; qos < priv->num_tx_queues; qos++) {
					priv->tx_queue[qos].queue = base_queue + qos;
					fau = fau - sizeof(u32);
					priv->tx_queue[qos].fau = fau;
					cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
				}

				switch (priv->imode) {

				/* These types don't support ports to IPD/PKO */
				case CVMX_HELPER_INTERFACE_MODE_DISABLED:
				case CVMX_HELPER_INTERFACE_MODE_PCIE:
				case CVMX_HELPER_INTERFACE_MODE_PICMG:
					break;

				case CVMX_HELPER_INTERFACE_MODE_NPI:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "npi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_XAUI:
				case CVMX_HELPER_INTERFACE_MODE_RXAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "xaui%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_LOOP:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "loop%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					if (priv->ipd_port == 0){
						sprintf(dev->name, "obc%d", priv->ipd_port);
					}else if (priv->ipd_port == 2){
						if (autelan_product_info.board_slot_id > 0)
						{
							sprintf(dev->name, "mng%d-%d", autelan_product_info.board_slot_id, priv->ipd_port - 1);
						}
						else
						{
							sprintf(dev->name, "mng%d", priv->ipd_port - 1);
						}
					}else {
						if (autelan_product_info.board_slot_id > 0)
						{
							sprintf(dev->name, "mng%d-%d", autelan_product_info.board_slot_id, priv->ipd_port + 1);
						}
						else
						{
							sprintf(dev->name, "mng%d", priv->ipd_port + 1);
						}
					}
					break;

				case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_spi_netdev_ops;
					strcpy(dev->name, "spi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_RGMII:
				case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;
#ifdef CONFIG_RAPIDIO
				case CVMX_HELPER_INTERFACE_MODE_SRIO:
					dev->netdev_ops = &cvm_oct_srio_netdev_ops;
					strcpy(dev->name, "rio%d");
					break;
#endif
				}

				if (!dev->netdev_ops) {
					free_netdev(dev);
				} else if (register_netdev(dev) < 0) {
					pr_err("Failed to register ethernet device for interface %d, port %d\n",
						 interface, priv->ipd_port);
					free_netdev(dev);
				} else {
					list_add_tail(&priv->list, &cvm_oct_list);
					if (cvm_oct_by_pkind[priv->ipd_pkind] == NULL)
						cvm_oct_by_pkind[priv->ipd_pkind] = priv;
					else
						cvm_oct_by_pkind[priv->ipd_pkind] = (void *)-1L;

					cvm_oct_add_ipd_port(priv);
					/*
					 * Each transmit queue will need its
					 * own MAX_OUT_QUEUE_DEPTH worth of
					 * WQE to track the transmit skbs.
					 */
					cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
					num_devices_extra_wqe++;

					queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
				}
			}
		}
	}
	else if (autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU)
	{
		num_interfaces = 2;
		
		for (interface = 0; interface < num_interfaces; interface++) {
			cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
			int num_ports = cvmx_helper_ports_on_interface(interface);
			int interface_port;
			int i = 1;

			for (interface_port = 0; num_ports > 0;
			     interface_port++, num_ports--) {
				struct octeon_ethernet *priv;
				int base_queue;
				struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
				if (!dev) {
					pr_err("Failed to allocate ethernet device for port %d:%d\n", interface, interface_port);
					continue;
				}

				if (disable_core_queueing)
					dev->tx_queue_len = 0;

				/* Initialize the device private structure. */
				priv = netdev_priv(dev);
				dev->priv = priv;
				priv->of_node = cvm_oct_node_for_port(pip, interface, interface_port);
				RB_CLEAR_NODE(&priv->ipd_tree);
				priv->netdev = dev;
				priv->interface = interface;
				priv->interface_port = interface_port;

				INIT_DELAYED_WORK(&priv->port_periodic_work,
						  cvm_oct_periodic_worker);
				priv->imode = imode;

				priv->ipd_port = cvmx_helper_get_ipd_port(interface, interface_port);
				priv->key = priv->ipd_port;
				priv->pko_port = cvmx_helper_get_pko_port(interface, interface_port);
				base_queue = cvmx_pko_get_base_queue(priv->ipd_port);
				priv->num_tx_queues = cvmx_pko_get_num_queues(priv->ipd_port);
				
				if (octeon_has_feature(OCTEON_FEATURE_PKND))
					priv->ipd_pkind = cvmx_helper_get_pknd(interface, interface_port);
				else
					priv->ipd_pkind = priv->ipd_port;

				for (qos = 0; qos < priv->num_tx_queues; qos++) {
					priv->tx_queue[qos].queue = base_queue + qos;
					fau = fau - sizeof(u32);
					priv->tx_queue[qos].fau = fau;
					cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
				}

				switch (priv->imode) {

				/* These types don't support ports to IPD/PKO */
				case CVMX_HELPER_INTERFACE_MODE_DISABLED:
				case CVMX_HELPER_INTERFACE_MODE_PCIE:
				case CVMX_HELPER_INTERFACE_MODE_PICMG:
					break;

				case CVMX_HELPER_INTERFACE_MODE_NPI:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "npi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_XAUI:
				case CVMX_HELPER_INTERFACE_MODE_RXAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "xaui%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_LOOP:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "loop%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_spi_netdev_ops;
					strcpy(dev->name, "obc0");
					break;

				case CVMX_HELPER_INTERFACE_MODE_RGMII:
				case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					if (!(autelan_product_info.board_slot_id < 0))
					{
						sprintf(dev->name, "eth%d-%d", autelan_product_info.board_slot_id, i++);
					}
					else
					{
						sprintf(dev->name, "eth0-%d", i++);
					}
					break;
#ifdef CONFIG_RAPIDIO
				case CVMX_HELPER_INTERFACE_MODE_SRIO:
					dev->netdev_ops = &cvm_oct_srio_netdev_ops;
					strcpy(dev->name, "rio%d");
					break;
#endif
				}

				if (!dev->netdev_ops) {
					free_netdev(dev);
				} else if (register_netdev(dev) < 0) {
					pr_err("Failed to register ethernet device for interface %d, port %d\n",
						 interface, priv->ipd_port);
					free_netdev(dev);
				} else {
					list_add_tail(&priv->list, &cvm_oct_list);
					if (cvm_oct_by_pkind[priv->ipd_pkind] == NULL)
						cvm_oct_by_pkind[priv->ipd_pkind] = priv;
					else
						cvm_oct_by_pkind[priv->ipd_pkind] = (void *)-1L;

					cvm_oct_add_ipd_port(priv);
					/*
					 * Each transmit queue will need its
					 * own MAX_OUT_QUEUE_DEPTH worth of
					 * WQE to track the transmit skbs.
					 */
					cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
					num_devices_extra_wqe++;

					queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
				}
			}
		}
	}
	else if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_2X12G12S || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX71_2X12G12S || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_12X)
	{
		num_interfaces = 1;
		for (interface = 0; interface < num_interfaces; interface++) {
			cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
			int num_ports = cvmx_helper_ports_on_interface(interface);
			int interface_port;

			/* Only 3 sgmii ports are available on AX71_2X12G12S */
			if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_12X) {
				num_ports = (num_ports > 2) ? 2 : num_ports;
			}
			else {
				num_ports = (num_ports > 3) ? 3 : num_ports;
			}

			for (interface_port = 0; num_ports > 0;
			     interface_port++, num_ports--) {
				struct octeon_ethernet *priv;
				int base_queue;
				struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
				if (!dev) {
					pr_err("Failed to allocate ethernet device for port %d:%d\n", interface, interface_port);
					continue;
				}

				if (disable_core_queueing)
					dev->tx_queue_len = 0;

				/* Initialize the device private structure. */
				priv = netdev_priv(dev);
				dev->priv = priv;
				priv->of_node = cvm_oct_node_for_port(pip, interface, interface_port);
				RB_CLEAR_NODE(&priv->ipd_tree);
				priv->netdev = dev;
				priv->interface = interface;
				priv->interface_port = interface_port;

				INIT_DELAYED_WORK(&priv->port_periodic_work,
						  cvm_oct_periodic_worker);
				priv->imode = imode;

				priv->ipd_port = cvmx_helper_get_ipd_port(interface, interface_port);
				priv->key = priv->ipd_port;
				priv->pko_port = cvmx_helper_get_pko_port(interface, interface_port);
				base_queue = cvmx_pko_get_base_queue(priv->ipd_port);
				priv->num_tx_queues = cvmx_pko_get_num_queues(priv->ipd_port);
				
				if (octeon_has_feature(OCTEON_FEATURE_PKND))
					priv->ipd_pkind = cvmx_helper_get_pknd(interface, interface_port);
				else
					priv->ipd_pkind = priv->ipd_port;

				for (qos = 0; qos < priv->num_tx_queues; qos++) {
					priv->tx_queue[qos].queue = base_queue + qos;
					fau = fau - sizeof(u32);
					priv->tx_queue[qos].fau = fau;
					cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
				}

				switch (priv->imode) {

				/* These types don't support ports to IPD/PKO */
				case CVMX_HELPER_INTERFACE_MODE_DISABLED:
				case CVMX_HELPER_INTERFACE_MODE_PCIE:
				case CVMX_HELPER_INTERFACE_MODE_PICMG:
					break;

				case CVMX_HELPER_INTERFACE_MODE_NPI:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "npi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_XAUI:
				case CVMX_HELPER_INTERFACE_MODE_RXAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "xaui%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_LOOP:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "loop%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					if (priv->ipd_port< 2) {
						sprintf(dev->name, "obc%d", priv->ipd_port);
					}else {
						sprintf(dev->name, "eth%d", priv->ipd_port + 1);
					}
					break;

				case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_spi_netdev_ops;
					strcpy(dev->name, "spi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_RGMII:
				case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;
#ifdef CONFIG_RAPIDIO
				case CVMX_HELPER_INTERFACE_MODE_SRIO:
					dev->netdev_ops = &cvm_oct_srio_netdev_ops;
					strcpy(dev->name, "rio%d");
					break;
#endif
				}

				if (!dev->netdev_ops) {
					free_netdev(dev);
				} else if (register_netdev(dev) < 0) {
					pr_err("Failed to register ethernet device for interface %d, port %d\n",
						 interface, priv->ipd_port);
					free_netdev(dev);
				} else {
					list_add_tail(&priv->list, &cvm_oct_list);
					if (cvm_oct_by_pkind[priv->ipd_pkind] == NULL)
						cvm_oct_by_pkind[priv->ipd_pkind] = priv;
					else
						cvm_oct_by_pkind[priv->ipd_pkind] = (void *)-1L;

					cvm_oct_add_ipd_port(priv);
					/*
					 * Each transmit queue will need its
					 * own MAX_OUT_QUEUE_DEPTH worth of
					 * WQE to track the transmit skbs.
					 */
					cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
					num_devices_extra_wqe++;

					queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
				}
			}
		}
	}
	else if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || \
		autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S)
	{
		num_interfaces = cvmx_helper_get_number_of_interfaces();
		/* for CN56XX cpu on AX81_AC8C, SMGII port is on interface 0, XAUI on interface 1 */
		num_interfaces = (num_interfaces > 2) ? 2 : num_interfaces;
		
		for (interface = 0; interface < num_interfaces; interface++) {
			cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
			int num_ports = cvmx_helper_ports_on_interface(interface);
			int interface_port;
			
 /* for 12c salve cpu targe mode */
#ifdef ETHERPCI
			if(autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C){
					if(imode == CVMX_HELPER_INTERFACE_MODE_NPI) {
						if (octeon_has_feature(OCTEON_FEATURE_PKND)) {
							cvmx_sli_tx_pipe_t	slitxpipe;
							slitxpipe.u64 = cvmx_read_csr(CVMX_PEXP_SLI_TX_PIPE);  
							slitxpipe.s.nump = 4; 
							cvmx_write_csr(CVMX_PEXP_SLI_TX_PIPE, slitxpipe.u64);  
						}
						num_ports = 4;
					}
				}
#endif			
			if ((autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || \
				autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X || \
				autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S) && 
				(interface == 0)) {
				num_ports = (num_ports > 2) ? 2 : num_ports;
			} 
			for (interface_port = 0; num_ports > 0;
			     interface_port++, num_ports--) {
				struct octeon_ethernet *priv;
				int base_queue;
				struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
				if (!dev) {
					pr_err("Failed to allocate ethernet device for port %d:%d\n", interface, interface_port);
					continue;
				}

				if (disable_core_queueing)
					dev->tx_queue_len = 0;

				/* Initialize the device private structure. */
				priv = netdev_priv(dev);
				dev->priv = priv;
				priv->of_node = cvm_oct_node_for_port(pip, interface, interface_port);
				RB_CLEAR_NODE(&priv->ipd_tree);
				priv->netdev = dev;
				priv->interface = interface;
				priv->interface_port = interface_port;

				INIT_DELAYED_WORK(&priv->port_periodic_work,
						  cvm_oct_periodic_worker);
				priv->imode = imode;

				if (imode == CVMX_HELPER_INTERFACE_MODE_SRIO) {
	                                int mbox = cvmx_helper_get_ipd_port(interface, interface_port) - cvmx_helper_get_ipd_port(interface, 0);
					cvmx_srio_tx_message_header_t tx_header;
					tx_header.u64 = 0;
					tx_header.s.tt = 0;
					tx_header.s.ssize = 0xe;
					tx_header.s.mbox = mbox;
					tx_header.s.lns = 1;
	                                tx_header.s.intr = 1;
					priv->srio_tx_header = tx_header.u64;
					priv->ipd_port = cvmx_helper_get_ipd_port(interface, mbox >> 1);
					priv->pko_port = priv->ipd_port;
					priv->key = priv->ipd_port + (0x10000 * mbox);
					base_queue = cvmx_pko_get_base_queue(priv->ipd_port) + (mbox & 1);
					priv->num_tx_queues = 1;
					cvm_oct_by_srio_mbox[interface - 4][mbox] = priv;
				} else {
					priv->ipd_port = cvmx_helper_get_ipd_port(interface, interface_port);
					priv->key = priv->ipd_port;
					priv->pko_port = cvmx_helper_get_pko_port(interface, interface_port);
					base_queue = cvmx_pko_get_base_queue(priv->ipd_port);
					priv->num_tx_queues = cvmx_pko_get_num_queues(priv->ipd_port);
				}
				if (octeon_has_feature(OCTEON_FEATURE_PKND))
					priv->ipd_pkind = cvmx_helper_get_pknd(interface, interface_port);
				else
					priv->ipd_pkind = priv->ipd_port;

				for (qos = 0; qos < priv->num_tx_queues; qos++) {
					priv->tx_queue[qos].queue = base_queue + qos;
					fau = fau - sizeof(u32);
					priv->tx_queue[qos].fau = fau;
					cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
				}

				switch (priv->imode) {

				/* These types don't support ports to IPD/PKO */
				case CVMX_HELPER_INTERFACE_MODE_DISABLED:
				case CVMX_HELPER_INTERFACE_MODE_PCIE:
				case CVMX_HELPER_INTERFACE_MODE_PICMG:
					break;

				case CVMX_HELPER_INTERFACE_MODE_NPI:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
 /* for 12c salve cpu targe mode */
#if defined(ETHERPCI)
					strcpy(dev->name, "pci%d");
#else
					strcpy(dev->name, "npi%d");
#endif
					break;

				case CVMX_HELPER_INTERFACE_MODE_XAUI:
				case CVMX_HELPER_INTERFACE_MODE_RXAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
					priv->has_gmx_regs = 1;
					/* modify the name of ve, [num_ports] should change later. zhangdi@autelan.com 2012-06-05 */
					sprintf(dev->name, "ve%02df%d", autelan_product_info.board_slot_id,num_ports);
					break;

				case CVMX_HELPER_INTERFACE_MODE_LOOP:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "loop%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C || 
						autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S ||
						autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S ||
						autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X)
					{
						if (priv->ipd_port < 2)
							sprintf(dev->name, "obc%d", priv->ipd_port);
						else
							sprintf(dev->name, "eth%d-%d", autelan_product_info.board_slot_id, priv->ipd_port + 3);
					}
					else
					{
						sprintf(dev->name, "eth%d-%d", autelan_product_info.board_slot_id, priv->ipd_port + 1);
					}
					break;

				case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_spi_netdev_ops;
					strcpy(dev->name, "spi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_RGMII:
				case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;
#ifdef CONFIG_RAPIDIO
				case CVMX_HELPER_INTERFACE_MODE_SRIO:
					dev->netdev_ops = &cvm_oct_srio_netdev_ops;
					strcpy(dev->name, "rio%d");
					break;
#endif
				}
 				/* for 12c salve cpu targe mode */
				if(priv->imode == CVMX_HELPER_INTERFACE_MODE_NPI) {
					netif_carrier_on(dev);
				}

				if (!dev->netdev_ops) {
					free_netdev(dev);
				} else if (register_netdev(dev) < 0) {
					pr_err("Failed to register ethernet device for interface %d, port %d\n",
						 interface, priv->ipd_port);
					free_netdev(dev);
				} else {
					list_add_tail(&priv->list, &cvm_oct_list);
					if (cvm_oct_by_pkind[priv->ipd_pkind] == NULL)
						cvm_oct_by_pkind[priv->ipd_pkind] = priv;
					else
						cvm_oct_by_pkind[priv->ipd_pkind] = (void *)-1L;

					cvm_oct_add_ipd_port(priv);
					/*
					 * Each transmit queue will need its
					 * own MAX_OUT_QUEUE_DEPTH worth of
					 * WQE to track the transmit skbs.
					 */
					cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
					num_devices_extra_wqe++;

					queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
				}
			}
		}
	}
	else
	{
		for (interface = 0; interface < num_interfaces; interface++) {
			cvmx_helper_interface_mode_t imode = cvmx_helper_interface_get_mode(interface);
			int num_ports = cvmx_helper_ports_on_interface(interface);
			int interface_port;

			if (imode == CVMX_HELPER_INTERFACE_MODE_SRIO)
				num_ports = 4;

			if (imode == CVMX_HELPER_INTERFACE_MODE_ILK)
				continue;

			for (interface_port = 0; num_ports > 0;
			     interface_port++, num_ports--) {
				struct octeon_ethernet *priv;
				int base_queue;
				struct net_device *dev = alloc_etherdev(sizeof(struct octeon_ethernet));
				if (!dev) {
					pr_err("Failed to allocate ethernet device for port %d:%d\n", interface, interface_port);
					continue;
				}

				if (disable_core_queueing)
					dev->tx_queue_len = 0;

				/* Initialize the device private structure. */
				priv = netdev_priv(dev);
				dev->priv = priv;
				priv->of_node = cvm_oct_node_for_port(pip, interface, interface_port);
				RB_CLEAR_NODE(&priv->ipd_tree);
				priv->netdev = dev;
				priv->interface = interface;
				priv->interface_port = interface_port;

				INIT_DELAYED_WORK(&priv->port_periodic_work,
						  cvm_oct_periodic_worker);
				priv->imode = imode;

				if (imode == CVMX_HELPER_INTERFACE_MODE_SRIO) {
	                                int mbox = cvmx_helper_get_ipd_port(interface, interface_port) - cvmx_helper_get_ipd_port(interface, 0);
					cvmx_srio_tx_message_header_t tx_header;
					tx_header.u64 = 0;
					tx_header.s.tt = 0;
					tx_header.s.ssize = 0xe;
					tx_header.s.mbox = mbox;
					tx_header.s.lns = 1;
	                                tx_header.s.intr = 1;
					priv->srio_tx_header = tx_header.u64;
					priv->ipd_port = cvmx_helper_get_ipd_port(interface, mbox >> 1);
					priv->pko_port = priv->ipd_port;
					priv->key = priv->ipd_port + (0x10000 * mbox);
					base_queue = cvmx_pko_get_base_queue(priv->ipd_port) + (mbox & 1);
					priv->num_tx_queues = 1;
					cvm_oct_by_srio_mbox[interface - 4][mbox] = priv;
				} else {
					priv->ipd_port = cvmx_helper_get_ipd_port(interface, interface_port);
					priv->key = priv->ipd_port;
					priv->pko_port = cvmx_helper_get_pko_port(interface, interface_port);
					base_queue = cvmx_pko_get_base_queue(priv->ipd_port);
					priv->num_tx_queues = cvmx_pko_get_num_queues(priv->ipd_port);
				}
				if (octeon_has_feature(OCTEON_FEATURE_PKND))
					priv->ipd_pkind = cvmx_helper_get_pknd(interface, interface_port);
				else
					priv->ipd_pkind = priv->ipd_port;

				for (qos = 0; qos < priv->num_tx_queues; qos++) {
					priv->tx_queue[qos].queue = base_queue + qos;
					fau = fau - sizeof(u32);
					priv->tx_queue[qos].fau = fau;
					cvmx_fau_atomic_write32(priv->tx_queue[qos].fau, 0);
				}

				switch (priv->imode) {

				/* These types don't support ports to IPD/PKO */
				case CVMX_HELPER_INTERFACE_MODE_DISABLED:
				case CVMX_HELPER_INTERFACE_MODE_PCIE:
				case CVMX_HELPER_INTERFACE_MODE_PICMG:
					break;

				case CVMX_HELPER_INTERFACE_MODE_NPI:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "npi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_XAUI:
				case CVMX_HELPER_INTERFACE_MODE_RXAUI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_xaui_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_xaui_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "xaui%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_LOOP:
					dev->netdev_ops = &cvm_oct_npi_netdev_ops;
					strcpy(dev->name, "loop%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SGMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_sgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_sgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_SPI:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_spi_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_spi_netdev_ops;
					strcpy(dev->name, "spi%d");
					break;

				case CVMX_HELPER_INTERFACE_MODE_RGMII:
				case CVMX_HELPER_INTERFACE_MODE_GMII:
#ifdef CONFIG_OCTEON_ETHERNET_LOCKLESS_IF_SUPPORTED
					if (octeon_pko_lockless()) {
						dev->netdev_ops = &cvm_oct_rgmii_lockless_netdev_ops;
						priv->tx_lockless = 1;
					} else
#endif
						dev->netdev_ops = &cvm_oct_rgmii_netdev_ops;
					priv->has_gmx_regs = 1;
					strcpy(dev->name, "eth%d");
					break;
#ifdef CONFIG_RAPIDIO
				case CVMX_HELPER_INTERFACE_MODE_SRIO:
					dev->netdev_ops = &cvm_oct_srio_netdev_ops;
					strcpy(dev->name, "rio%d");
					break;
#endif
				}

				if (!dev->netdev_ops) {
					free_netdev(dev);
				} else if (register_netdev(dev) < 0) {
					pr_err("Failed to register ethernet device for interface %d, port %d\n",
						 interface, priv->ipd_port);
					free_netdev(dev);
				} else {
					list_add_tail(&priv->list, &cvm_oct_list);
					if (cvm_oct_by_pkind[priv->ipd_pkind] == NULL)
						cvm_oct_by_pkind[priv->ipd_pkind] = priv;
					else
						cvm_oct_by_pkind[priv->ipd_pkind] = (void *)-1L;

					cvm_oct_add_ipd_port(priv);
					/*
					 * Each transmit queue will need its
					 * own MAX_OUT_QUEUE_DEPTH worth of
					 * WQE to track the transmit skbs.
					 */
					cvm_oct_mem_fill_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, PER_DEVICE_EXTRA_WQE);
					num_devices_extra_wqe++;

					queue_delayed_work(cvm_oct_poll_queue, &priv->port_periodic_work, HZ);
				}
			}
		}
	}

	/* Set collision locations to NULL */
	for (i = 0; i < ARRAY_SIZE(cvm_oct_by_pkind); i++)
		if (cvm_oct_by_pkind[i] == (void *)-1L)
			cvm_oct_by_pkind[i] = NULL;

	cvm_oct_rx_initialize(num_packet_buffers + num_devices_extra_wqe * PER_DEVICE_EXTRA_WQE);

	queue_delayed_work(cvm_oct_poll_queue, &cvm_oct_rx_refill_work, HZ);

	oct_init();
	return rv;
err_cleanup:
	cvm_oct_mem_cleanup();
	return rv;
}

static void __exit cvm_oct_cleanup_module(void)
{
	struct octeon_ethernet *priv;
	struct octeon_ethernet *tmp;

	/* release netlink socket in cavium octeon */
	if(nl_sk != NULL){
        sock_release(nl_sk->sk_socket);   
    }

	/*
	 * Put both taking the interface down and unregistering it
	 * under the lock.  That way the devices cannot be taken back
	 * up in the middle of everything.
	 */
	rtnl_lock();

	/*
	 * Take down all the interfaces, this disables the GMX and
	 * prevents it from getting into a Bad State when IPD is
	 * disabled.
	 */
	list_for_each_entry(priv, &cvm_oct_list, list) {
		unsigned int f = dev_get_flags(priv->netdev);
		dev_change_flags(priv->netdev, f & ~IFF_UP);
	}

	mdelay(10);

	cvmx_ipd_disable();

	mdelay(10);

	atomic_inc_return(&cvm_oct_poll_queue_stopping);
	cancel_delayed_work_sync(&cvm_oct_rx_refill_work);

	cvm_oct_rx_shutdown0();

	/* unregister the ethernet devices */
	list_for_each_entry(priv, &cvm_oct_list, list) {
		cancel_delayed_work_sync(&priv->port_periodic_work);
		unregister_netdevice(priv->netdev);
	}

	rtnl_unlock();

	/* Free the ethernet devices */
	list_for_each_entry_safe_reverse(priv, tmp, &cvm_oct_list, list) {
		list_del(&priv->list);
		free_netdev(priv->netdev);
	}

	cvmx_helper_shutdown_packet_io_global();

	cvm_oct_rx_shutdown1();

	destroy_workqueue(cvm_oct_poll_queue);

	cvm_oct_proc_shutdown();

	/* Free the HW pools */
	cvm_oct_mem_empty_fpa(CVMX_FPA_PACKET_POOL, CVMX_FPA_PACKET_POOL_SIZE, num_packet_buffers);
	cvm_oct_mem_empty_fpa(CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE, num_packet_buffers);
	cvm_oct_mem_empty_fpa(CVMX_FPA_TX_WQE_POOL, CVMX_FPA_TX_WQE_POOL_SIZE, num_devices_extra_wqe * PER_DEVICE_EXTRA_WQE);

	if (CVMX_FPA_OUTPUT_BUFFER_POOL != CVMX_FPA_PACKET_POOL)
		cvm_oct_mem_empty_fpa(CVMX_FPA_OUTPUT_BUFFER_POOL, CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, cvm_oct_num_output_buffers);

	/*
	 * Disable FPA, all buffers are free, not done by helper
	 * shutdown.  But don't do it on 68XX as this causes SSO to
	 * malfunction on subsequent initialization.
	 */
	if (!OCTEON_IS_MODEL(OCTEON_CN68XX))
		cvmx_fpa_disable();
	
	sock_release(se_agent_nl_sock->sk_socket);
	se_agent_nl_sock = NULL;
	oct_cleanup();
	cvm_oct_mem_cleanup();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon ethernet driver.");
#ifdef MODULE
module_init(cvm_oct_init_module);
#else
late_initcall(cvm_oct_init_module);
#endif
module_exit(cvm_oct_cleanup_module);
