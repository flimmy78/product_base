/*******************************************************************************
Copyright (C) Autelan Technology


This software file is owned and distributed by Autelan Technology 
********************************************************************************


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* rpa-ioctl.c
*
* CREATOR:
* autelan.software.bsp&boot. team
*
* DESCRIPTION:
* rpa-ethernet module receive packet
*
*******************************************************************************/

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <net/if_inet6.h>
#include <linux/vmalloc.h>
#include <linux/mii.h>
#include <linux/inetdevice.h>
#include <linux/sched.h>
#include <asm/atomic.h>
#include <asm/delay.h>

#include "rpa.h"
#include "rpa-includes.h"

unsigned int rpa_broadcast_mask;
unsigned int rpa_default_broadcast_mask;
unsigned int rpa_rcv_skb_data_offset = 0x0;
unsigned int rpa_rx_out_num = 0x0;

extern struct net_device *cvm_oct_device[];
extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
extern int rpa_rx_debug;
extern int rpa_tx_debug;
extern int rpa_rx_eth_debug;
extern int rpa_arp_eth_debug;
extern int rpa_local_slot_num;

extern uint32_t (*cvm_rpa_rx_hook) (struct sk_buff *);
extern int (*cvm_rpa_tx_hook)(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag);
extern void printPacketBuffer(unsigned char *buffer,unsigned long buffLen);


typedef struct  cvm_enet_ether_header {
	uint8_t  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
	uint16_t ether_type;
}cvm_eth_header;

typedef struct  rpa_vlan_ether_header {
	uint8_t  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
	uint16_t vlan_type;
	uint16_t vlan_tag;
	uint16_t ether_type;
}rpa_vlan_eth_header;

typedef struct rpa_ether_arp_header{
	unsigned short	ar_hrd;		/* format of hardware address	*/
	unsigned short	ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	unsigned short	ar_op;		/* ARP opcode (command)		*/
}rpa_arp_header;

#if 0
int rpa_remove_rpa_tag(struct sk_buff *skb, int flag)
{
	cvm_eth_header *original_head;
	cvm_eth_header *new_head;

	if (flag)
	{
		original_head = (cvm_eth_header *)(skb->data - 14);
		new_head = (cvm_eth_header *)(skb->data - 6);

		new_head->ether_type = original_head->ether_type;
		memset(new_head->ether_shost, 0, RPA_ENET_ETHER_ADDR_LEN);
		memcpy(new_head->ether_shost, original_head->ether_shost, RPA_ENET_ETHER_ADDR_LEN);
		memset(new_head->ether_dhost, 0, RPA_ENET_ETHER_ADDR_LEN);
		memcpy(new_head->ether_dhost, original_head->ether_dhost, RPA_ENET_ETHER_ADDR_LEN);
		
		skb->data = (unsigned char *)new_head + 14;
		skb->mac.raw = (unsigned char *)original_head + RPA_ENET_RPATAG_LEN;
		skb->len -= RPA_ENET_RPATAG_LEN;
	}
	else
	{
		
	}

	if (rpa_eth_debug)
	{
		printk("mode[%d] : after remove rpa tag\n", flag);
		printPacketBuffer(skb->data, skb->len);
		printPacketBuffer(skb->mac.raw, 14 + skb->len);
	}
	
	return 0;
}
#endif

/**
 * get local port number when send to another protocol stack on other board
 *
 * @param skb : the sk buff
 * @param slotNum : the slot number of this board
 */
int cvm_rpa_local_netdevnum_get(int ifindex)
{
	int netdevNum = -1;
	int i = 0;

	for (i = 0; i < RPA_NETDEV_NUM; i++)
	{
		if ((rpa_local_slot_num >= 0) && (cvm_rpa_dev_index[rpa_local_slot_num][i].ifindex == ifindex))
		{
			netdevNum = i;
			return netdevNum;
		}
	}

	return netdevNum;
}
EXPORT_SYMBOL(cvm_rpa_local_netdevnum_get);

/*
 * broadcast the neighbor advertisement packet for ipv6 function 
 * caojia , 20111012
 */
rpa_callback_result_t rpa_ipv6_neighbor_advertisement_broadcast(struct sk_buff *skb)
{
	struct ethhdr *ether_header;
	rpa_vlan_eth_header *vlan_ether_header;
	struct ipv6hdr *ipv6_header;
	struct icmp6hdr *icmp6_header;

	int netdevNum = 0;
	int slot = 0;
	struct sk_buff *skb_cp = NULL;
	int ret;

	if ((NULL == skb) || (NULL == skb->dev)) {
		return -1;
	}

	if (cvm_rpa_tx_hook == NULL) {
		return -1;
	}

	if (!rpa_broadcast_mask) {
		return 0;
	}

	if ((skb->dev->priv_flags & IFF_RPA) || (skb->dev->priv_flags & IFF_802_1Q_VLAN)) {
		return 0;
	}

	if (skb->mac_header) {
		ether_header = eth_hdr(skb);
		vlan_ether_header = (rpa_vlan_eth_header *)eth_hdr(skb);
	}
	else {
		return 0;
	}

	/* is ipv6 packet */
	if (likely(ETH_P_IPV6 == ntohs(ether_header->h_proto)))
	{
		ipv6_header = (struct ipv6hdr *)(skb->data);

		/* next header is icmpv6*/
		if (likely(0x6 == ipv6_header->version) && likely(0x3a == ipv6_header->nexthdr))
		{
			icmp6_header = (struct icmp6hdr *)(skb->data + 40);
		}
		else
		{
			return -1;
		}
	}
	else if ((ETH_P_8021Q == vlan_ether_header->vlan_type) && \
		(ETH_P_IPV6 == vlan_ether_header->ether_type))
	{
		ipv6_header = (struct ipv6hdr *)(skb->data + 4);

		/* next header is icmpv6*/
		if (likely(0x6 == ipv6_header->version) && likely(0x3a == ipv6_header->nexthdr))
		{
			icmp6_header = (struct icmp6hdr *)(skb->data + 40 + 4);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	/* icmpv6 type is neighbor advertisement */
	if (likely(0x88 == icmp6_header->icmp6_type))
	{
		netdevNum = cvm_rpa_local_netdevnum_get(skb->dev->ifindex);
		if (netdevNum < 0)
		{
			return 0;
		}
		for (slot = 0; slot < RPA_SLOT_NUM; slot++)
		{	
			if (slot == rpa_local_slot_num || !((rpa_broadcast_mask >> slot) & 0x1)) {
				continue;
			}
			
			if (rpa_arp_send_dev[slot].netdev)
			{
				skb_cp = skb_copy(skb, GFP_ATOMIC);/*copy the skb */
				if (!skb_cp) {
					rpa_warning("skb_copy failed in rpa_arp_broadcast!\n");
					return -1;
				}

				else
				{
					skb_cp->data = (unsigned char *)eth_hdr(skb_cp);
					skb_cp->len += RPA_ETHER_HEAD_LEN;
				}

				skb_cp->dev = rpa_arp_send_dev[slot].netdev;
				ret = cvm_rpa_tx_hook(skb_cp, skb_cp->dev, netdevNum, RPA_ENTER_STACK_ENABLE);
				if (ret != 0) {
/*					kfree_skb(skb_cp); skb has been freed in cvm_rpa_tx_hook.*/ 
					if (rpa_arp_eth_debug) {
						rpa_info("IPV6 Neighbor Advertisement send to slot %d by rpa failed !\n", slot);
					}
				}
				else {
					if (rpa_arp_eth_debug) {
						rpa_info("IPV6 Neighbor Advertisement send to slot %d by rpa sucess!\n", slot);
					}
				}
			}
			else {
				if (rpa_arp_eth_debug) {
					printk("rpa_arp_send_dev[%d].netdev not exist!\n", slot);
				}
			}
		}
	}
	else
	{
		return 0;
	}

	return 0;
}


rpa_callback_result_t rpa_arp_broadcast(struct sk_buff *skb)
{
	struct ethhdr *ether_header;
	rpa_arp_header *arp_header;
	rpa_vlan_eth_header *vlan_ether_header;
	int netdevNum = -1;
	int slot = 0;
	struct sk_buff *skb_cp = NULL;
	int ret;

	if ((NULL == skb) || (NULL == skb->dev)) {
		return -1;
	}

	if (cvm_rpa_tx_hook == NULL) {
		return -1;
	}

	if (!rpa_broadcast_mask) {
		return 0;
	}

	if ((skb->dev->priv_flags & IFF_RPA) || (skb->dev->priv_flags & IFF_802_1Q_VLAN)) {
		return 0;
	}

	if (skb->mac_header) {
		ether_header = eth_hdr(skb);
		vlan_ether_header = (rpa_vlan_eth_header *)eth_hdr(skb);
	}
	else {
		return 0;
	}

	if (ntohs(ether_header->h_proto) == ETH_P_ARP) {
		arp_header = (rpa_arp_header *)skb->data;
		//printk("ar_op : %x\n", arp_header->ar_op);
		//printk("dev name : %s\n", skb->dev->name);
	}
	else if ((ETH_P_8021Q == vlan_ether_header->vlan_type) && \
		(ETH_P_ARP == vlan_ether_header->ether_type)) {
		arp_header = (rpa_arp_header *)(skb->data + 4);
		//printk("V ar_op : %x\n", arp_header->ar_op);
		//printk("V dev name : %s\n", skb->dev->name);
	}
	else {
		return 0;
	}

	if (arp_header->ar_op == 0x2) {
		netdevNum = cvm_rpa_local_netdevnum_get(skb->dev->ifindex);
		if (netdevNum < 0) {
			return 0;
		}

		for (slot = 0; slot < RPA_SLOT_NUM; slot++) {
			if (slot == rpa_local_slot_num || !((rpa_broadcast_mask >> slot) & 0x1)) {
				continue;
			}

			if (rpa_arp_send_dev[slot].netdev) {
				skb_cp = skb_copy(skb, GFP_ATOMIC);/*copy the skb */
				if (!skb_cp) {
					rpa_warning("skb_copy failed in rpa_arp_broadcast!\n");
					return -1;
				}
				else {
					skb_cp->data = (unsigned char *)eth_hdr(skb_cp);
					skb_cp->len += RPA_ETHER_HEAD_LEN;
				}
				
				skb_cp->dev = rpa_arp_send_dev[slot].netdev;
				ret = cvm_rpa_tx_hook(skb_cp, skb_cp->dev, netdevNum, RPA_ENTER_STACK_ENABLE);
				if (ret != 0) {
/*					kfree_skb(skb_cp); skb has been freed in cvm_rpa_tx_hook.*/ 
					if (rpa_arp_eth_debug) {
						rpa_info("ARP Reply send to slot %d by rpa failed !\n", slot);
					}
				}
				else {
					if (rpa_arp_eth_debug) {
						rpa_info("Arp-reply send to slot %d by rpa sucess!\n", slot);
					}
				}
			}
			else {
				if (rpa_arp_eth_debug) {
					printk("rpa_arp_send_dev[%d].netdev not exist!\n", slot);
				}
			}
		}
	}
	else {
		return 0;
	}

	return 0;
}

/**
 * Description:
 *  rpa rx callback function.
 *
 * Parameter:
 *  dev: device pointer
 *  wqe: work queue pointer
 *  skb: packet skb
 *
 * Return:
 *  CVM_OCT_TAKE_OWNERSHIP_SKB: send by hard_start_xmit
 *
 */
unsigned int rpa_recv_cb(struct net_device *dev, struct sk_buff *skb, unsigned int flag)
{
	unsigned short dslot, dport;
	rpa_eth_header *rpa_head = NULL;
	int result=0;	
	//int cpu = smp_processor_id();
	
	if(rpa_rx_eth_debug) {
		printk("before remove rpa tag\n");
		printPacketBuffer(skb->data, skb->len);   
	}
	
	if (flag == NORMAL_RPA_PKT) {
		rpa_head = (rpa_eth_header *)(skb->data + rpa_rcv_skb_data_offset);
	}
	else if (flag == TO_PS_RPA_PKT) {
		if (rpa_rx_eth_debug) {
			printk("rpa rx send to protocol stack\n");
			printPacketBuffer(skb->data, skb->len); 
		}
		netif_receive_skb(skb);

		return CVM_OCT_TAKE_OWNERSHIP_SKB;
	}
	else {
		return CVM_OCT_DROP;
	}

	if ((long unsigned int)rpa_head & 0xff00000000000000) {
		dslot = (rpa_head->d_s_slotNum & RPA_DST_SLOT_MASK) >> RPA_DST_SLOT;
		dport = rpa_head->dnetdevNum;
		if (dport > RPA_NETDEV_NUM) {
			rpa_info("rpa receive packet dnetdevNum(%d) error!\n", dport);
			return CVM_OCT_DROP;
		}
	}
	else {
		rpa_info("skb->data invalid address : %p\n", rpa_head);
		return CVM_OCT_DROP;
	}

	/* to remove RPA_tag */
	skb_pull(skb, RPA_ENET_RPATAG_LEN + rpa_rcv_skb_data_offset);
	if (((long unsigned int)skb->head & 0xff00000000000000) && (skb->len >= RPA_ENET_RPATAG_LEN)) {
		skb->network_header = skb->data - skb->head + 14;
	}
	else {
		rpa_err("skb->head invalid address : %p\n", skb->head);
		rpa_err("skb->len(%d) less than rpa_head_len(18).\n", skb->len);
		return CVM_OCT_DROP;
	}

	if(rpa_rx_eth_debug) {
		printk("after remove rpa tag\n");
		printPacketBuffer(skb->data, skb->len);   
	}

	/* other packet send out by the relevant device */
#if 0
	if(skb->dev)
	{
		if (skb->dev->xmit_lock_owner != cpu){

			spin_lock(&skb->dev->xmit_lock);
			skb->dev->xmit_lock_owner = cpu;
			result = skb->dev->hard_start_xmit(skb, skb->dev);
			if (result == 0)
			{
				skb->dev->xmit_lock_owner = -1;
				spin_unlock(&skb->dev->xmit_lock);
				return CVM_OCT_TAKE_OWNERSHIP_SKB;
			}
			else
			{
				skb->dev->xmit_lock_owner = -1;
				spin_unlock(&skb->dev->xmit_lock);
				return CVM_OCT_DROP;
			}
		}
		else
		{
			printk(KERN_CRIT "RPA : Dead loop on virtual device "
				       "%s.\n", skb->dev->name);
			return CVM_OCT_DROP;
		}
	}
#else 
	if (!((rpa_rx_out_num++) % 100)) {
		touch_softlockup_watchdog();
	}
	if (rpa_rx_out_num == 0xffff)
			rpa_rx_out_num = 0x0;
	if(cvm_rpa_dev_index[dslot][dport].netdev && (cvm_rpa_dev_index[dslot][dport].netdev->flags & IFF_UP)) {
		if (cvm_rpa_dev_index[dslot][dport].netdev->netdev_ops && 
			cvm_rpa_dev_index[dslot][dport].netdev->netdev_ops->ndo_start_xmit) {
			const struct net_device_ops *ops = cvm_rpa_dev_index[dslot][dport].netdev->netdev_ops;
			skb->dev = cvm_rpa_dev_index[dslot][dport].netdev;
			//spin_lock(&skb->dev->xmit_lock);
			//skb->dev->xmit_lock_owner = cpu;
			result = ops->ndo_start_xmit(skb, skb->dev);
			if (result == 0) {
				//skb->dev->xmit_lock_owner = -1;
				//spin_unlock(&skb->dev->xmit_lock);
				return CVM_OCT_TAKE_OWNERSHIP_SKB;
			}
			else {
				//skb->dev->xmit_lock_owner = -1;
				//spin_unlock(&skb->dev->xmit_lock);
				return CVM_OCT_DROP;
			}
		}
		else {
			rpa_warning("The netdev[%d][%d] %s has no netdev_ops or no ndo_start_xmit!\n", dslot, dport, cvm_rpa_dev_index[dslot][dport].netdev->name);
			rpa_warning("netdev_ops[%p] , ndo_start_xmit[%p]\n", 
				cvm_rpa_dev_index[dslot][dport].netdev->netdev_ops, 
				cvm_rpa_dev_index[dslot][dport].netdev->netdev_ops->ndo_start_xmit);
			return CVM_OCT_DROP;
		}
	}
#endif 
	else {
		return CVM_OCT_DROP;
		//printk("Can not found net device!.\n");
	}
}

