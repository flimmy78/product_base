/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"
#ifndef __AX_PLATFORM__
#include <linux/dba.h>
#endif

#ifndef __AX_PLATFORM__
extern unsigned int dba_enable_flag;
extern int (*br_dhcp_broadcast_hook) (struct sk_buff **pskb, int *dhcp_flag);
#endif
/* Bridge group multicast address 802.1d (pg 51). */
const u8 br_group_address[ETH_ALEN] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };
/*************************hanhui upgrade*****************************/
//#define __AX_PLATFORM__
#ifndef __AX_PLATFORM__
/* ---------------------------------------------------------------- */
/*
 * Fill a rtnetlink message with our event data.
 * Note that we propage only the specified event and don't dump the
 * current wireless config. Dumping the wireless config is far too
 * expensive (for each parameter, the driver need to query the hardware).
 */

/*#define BR_NETLINK_DEBUG*/
 
#ifdef BR_NETLINK_DEBUG
#define debugPrintk(fmt, args...) \
	printk(KERN_DEBUG " %s() %d: " fmt, __FUNCTION__, __LINE__ , ## args )
#else
#define debugPrintk(fmt, args...)
#endif /* BR_NETLINK_DEBUG */
extern unsigned int br_user_mstp;/*1 is enabled 0 is not enable*/
extern struct sock *brnl;
#define BR_RTMSG(r) ((unsigned char *)(r) + NLMSG_ALIGN(sizeof(struct br_netlink_msg)))
struct br_netlink_msg
{
	int	br_ifindex;			/* bridge Link index	*/
	int	port_ifindex;		/* port Link index	*/
	unsigned int msg_len;			
};

void 
br_dump_packet_detail
(
	unsigned char *buffer,
	unsigned int buffLen
)
{
	unsigned int i = 0;
	unsigned char lineBuffer[64] = {0}, *bufPtr = NULL;
	unsigned int curLen = 0;

	if( !buffer) {
		return;
	}
	
	printk(KERN_DEBUG ".......................br debug....................... %d\n",buffLen);
	bufPtr = lineBuffer;
	curLen = 0;
	for(i = 0;i < buffLen ; i++)
	{
		curLen += sprintf(bufPtr,"%02x ",buffer[i]);
		bufPtr = lineBuffer + curLen;
		
		if(0==(i+1)%16) {
			printk(KERN_DEBUG "%s\n",lineBuffer);
			memset(lineBuffer,0,sizeof(lineBuffer));
			curLen = 0;
			bufPtr = lineBuffer;
		}
	}
	
	if((buffLen%16)!=0)
	{
		printk(KERN_DEBUG "%s\n",lineBuffer);
	}
	
	printk(KERN_DEBUG ".......................br debug.......................\n");
}

/*struct net_bridge_port *p  dev = p->dev*/
/**********************************************************************************
 *  br_netlink_send_bpdu
 *
 *	DESCRIPTION:
 * 		send bpdu packet
 *
 *	INPUT:		
 *		struct net_device *dev
 *		unsigned char *data 
 *		int length
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 **********************************************************************************/
void 
br_netlink_send_bpdu
(
	struct net_device *dev, 
	unsigned char *data, 
	int length
)
{
	struct sk_buff *skb = NULL;
	int size = 0;
	
	if (!dev || !data) {
		printk(KERN_ERR "br_netlink_send_bpdu:dev %s, data %s \n", dev ? "exist":"nil", data ? "exist":"nil");
		return 1;
	}

	size = length + 2*ETH_ALEN + 2;
	if (size < 60) {
		size = 60;
	}
	
	if ((skb = dev_alloc_skb(size)) == NULL) {
		printk(KERN_ERR "br: memory squeeze!\n");
		return;
	}

	skb->dev = dev;
	skb->protocol = htons(ETH_P_802_2);
	skb->mac_header = skb_put(skb, size);
	memcpy(skb->mac_header, br_group_address, ETH_ALEN);
	memcpy(skb->mac_header +ETH_ALEN, dev->dev_addr, ETH_ALEN);
	memset(skb->mac_header+(2*ETH_ALEN), 0 , 1);
	memset(skb->mac_header+(2*ETH_ALEN), length , 1);
	/*
	skb->mac_header[2*ETH_ALEN] = 0;
	skb->mac_header[2*ETH_ALEN+1] = length;
	*/
	skb->network_header = skb->mac_header + 2*ETH_ALEN + 2;
	memcpy(skb->network_header, data, length);
	memset(skb->network_header + length, 0xa5, size - length - ETH_HLEN);

	debugPrintk("to %s\n", dev->name);
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_OUT, skb, NULL, skb->dev,
		dev_queue_xmit);
}

/**********************************************************************************
 *  br_netlink_rcv_skb
 *
 *	DESCRIPTION:
 * 		receive netlink message from skb
 *
 *	INPUT:
 *		
 *		struct sk_buff *skb
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0	success
 *		1	error
 *
 **********************************************************************************/
int
br_netlink_rcv_skb
(
	struct sk_buff *skb
)
{
	struct nlmsghdr *nlh = NULL;
	unsigned char *ptr = NULL;
	struct br_netlink_msg *rtm = NULL;
	struct net_device *dev = NULL;
	
	if (!skb) {
		printk(KERN_ERR "br_netlink_rcv_skb:skb %s\n", skb ? "exist":"nil");
		return 1;
	}
	
	if (skb->len >= NLMSG_SPACE(0)) { /*NLMSG_SPACE(0) is 16 sizeof struct nlmsghdr*/
		nlh = (struct nlmsghdr *)skb->data;
		if (nlh->nlmsg_len < sizeof(struct nlmsghdr)
		    || skb->len < nlh->nlmsg_len) {
			return 0;
		}
		rtm = (struct br_netlink_msg *)NLMSG_DATA(nlh);
/*printk(KERN_ERR "brnetlink_rcv_msg: rtm->br_ifindex is %d, rtm->port_ifindex is %d, rtm->msg_len is %d\n", rtm->br_ifindex, rtm->port_ifindex, rtm->msg_len);	*/
		ptr = BR_RTMSG(rtm);
		
#ifdef BR_NETLINK_DEBUG
		br_dump_packet_detail(ptr, rtm->msg_len);
#endif		
		if ((dev = dev_get_by_index(dev_net(skb->dev), rtm->port_ifindex)) != NULL) {
			br_netlink_send_bpdu(dev, ptr, rtm->msg_len);
			dev_put(dev);
		}
		else {
			printk(KERN_ERR "brnetlink_rcv_msg: can not find dev for this port_ifindex %d\n", rtm->port_ifindex);
		}
	}
	else {
		printk(KERN_ERR "brnetlink_rcv_msg: skb->len is too small %d\n", skb->len);	
	}

	return 0;
}

/**********************************************************************************
 *  br_netlink_rcv
 *
 *	DESCRIPTION:
 * 		receive netlink message for sock queue
 *
 *	INPUT:
 *		
 *		struct sock *sk 
 *		int len
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 **********************************************************************************/
void 
br_netlink_rcv
(
	struct sock *sk, 
	int len
)
{	
	if (!sk || !br_user_mstp) {
		#if 0
		printk(KERN_ERR "br_netlink_rcv:sk %s and br user mstp is %s\n", sk ? "exist":"nil", br_user_mstp ? "enable":"disable");
        #endif
		return 1;
	}
	
	do {
		struct sk_buff *skb;
		
		while ((skb = skb_dequeue(&sk->sk_receive_queue)) != NULL) {			
			if (br_netlink_rcv_skb(skb)) {
				if (skb->len) {
					skb_queue_head(&sk->sk_receive_queue, skb);
				}
				else {
					kfree_skb(skb);
				}
				break;
			}
			kfree_skb(skb);
		}

	} while(brnl && brnl->sk_receive_queue.qlen);
}

/**********************************************************************************
 *  br_netlink_build_msg
 *
 *	DESCRIPTION:
 * 		biuld a netlink message
 *
 *	INPUT:
 *		
 * 		struct sk_buff *rt_skb
 *		int br_ifindex
 *		int port_ifindex
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		sk_buff		success
 *		NULL		fail
 *
 **********************************************************************************/
struct sk_buff *
br_netlink_build_msg
(
	struct sk_buff *rt_skb, 
	int br_ifindex,
	int port_ifindex
)
{
	struct sk_buff *skb = NULL;
	size_t size = 0;
	sk_buff_data_t old_tail = NULL;
	struct nlmsghdr *nlh = NULL;
	unsigned char *ptr = NULL;
	struct br_netlink_msg *rtm = NULL;
	
	if (!rt_skb) {
		printk(KERN_ERR "br_netlink_build_msg: rt_skb %s\n", rt_skb ? "exist":"nil");
		return NULL;
	}

	size = NLMSG_SPACE(rt_skb->len + rt_skb->mac_len);
	size += NLMSG_ALIGN(sizeof(struct br_netlink_msg));
	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_ERR "br_netlink_build_msg: alloc skb fail !!!");
		return NULL;
	}
/*
printk(KERN_ERR "brrmg_build_message: br index %d, port index %d, msg len %d, data len %d, mac len %d\n", br_ifindex, port_ifindex, rt_skb->len, rt_skb->data_len, rt_skb->mac_len);
printk(KERN_ERR "brrmg_build_message: size %d, sizeof(*nlh) %d\n", size, sizeof(*nlh));
	*/
	old_tail = skb->tail;
	nlh = NLMSG_PUT(skb, 0, 0, RTM_NEWLINK, size - sizeof(*nlh));
	rtm = (struct br_netlink_msg *)NLMSG_DATA(nlh);
	rtm->br_ifindex = br_ifindex;
	rtm->port_ifindex = port_ifindex;
	rtm->msg_len = rt_skb->len + rt_skb->mac_len;
	ptr = BR_RTMSG(rtm);
	/*memcpy(ptr, rt_skb->mac.raw, rt_skb->len + rt_skb->mac_len);need think about it*/	
	memcpy(ptr, rt_skb->mac_header, rt_skb->mac_len);
	memcpy((ptr + rt_skb->mac_len), rt_skb->data, rt_skb->len);
	nlh->nlmsg_len = skb->tail - old_tail;
	return skb;

nlmsg_failure:
	if (skb)
		kfree_skb(skb);
		printk(KERN_ERR "br_netlink_build_msg: error creating netlink message\n");
	return NULL;
}

/**********************************************************************************
 *  br_netlink_send_msg2user
 *
 *	DESCRIPTION:
 * 		send netlink memssage to user, bpdu packet is in messsage
 *
 *	INPUT:
 *		
 *		int br_ifindex
 *	 	int port_ifindex
 *		struct sk_buff *skb
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 **********************************************************************************/
void 
br_netlink_send_msg2user
(
	int br_ifindex,
	int port_ifindex,
	struct sk_buff *skb
)
{
	struct sk_buff *skb2 = NULL;
	
	if (!skb) {
		printk(KERN_ERR "br_netlink_send_msg2user:skb %s\n", skb ? "exist":"nil");
		return;
	}

	skb2 = br_netlink_build_msg(skb, br_ifindex, port_ifindex);
	if (skb2 == NULL) {
		return;
	}
	
	NETLINK_CB(skb2).dst_group = 1;
	netlink_broadcast(brnl, skb2, 0, 1, GFP_ATOMIC);
}

#endif
/******************************end***************************************/
static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
	struct net_device *indev, *brdev = br->dev;

	brdev->stats.rx_packets++;
	brdev->stats.rx_bytes += skb->len;

	indev = skb->dev;
	skb->dev = brdev;
	/* Bugfix AXSSZFI-1551, replace with netif_rx() for spinlock lockup on CPU. zhangdi@autelan.com 2012-04-12 */
#if 0
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
		netif_receive_skb);
#else
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
		netif_rx);
#endif
}

/* note: already called with rcu_read_lock (preempt_disabled) */
int br_handle_frame_finish(struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);
	struct net_bridge *br;
	struct net_bridge_fdb_entry *dst;
	struct sk_buff *skb2;

	if (!p || p->state == BR_STATE_DISABLED)
		goto drop;

	/* insert into forwarding database after filtering to avoid spoofing */
	br = p->br;
	if(!(!br->fdb_multicast_update && p->isuplink && is_multicast_ether_addr(dest))){
		br_fdb_update(br, p, eth_hdr(skb)->h_source);
	}

	if (p->state == BR_STATE_LEARNING)
		goto drop;

	/* The packet skb2 goes to the local host (NULL to skip). */
	skb2 = NULL;

	if (br->dev->flags & IFF_PROMISC)
		skb2 = skb;

	dst = NULL;

	if (is_multicast_ether_addr(dest)) {
		br->dev->stats.multicast++;
		skb2 = skb;
		/*************hanhui upgrade**********/
		if(br->multicast_isolation_enabled && !(p->isuplink)){
			//if multicast isolation enabled do not flood but frame up and forward to uplink ports
			int i = 0;
			struct sk_buff *skb1 = skb;
			struct net_bridge_port * tmpp = NULL;
			for(i = 0;i<UPLINK_MAX_COUNT;i++) {
				if(br->uplink_port[i]) {
					if(tmpp) {
						if((skb1 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
							br->dev->stats.tx_dropped++;
							kfree_skb(skb);
							return 0;
						}
						br_forward(tmpp, skb1);
					}
					tmpp = br->uplink_port[i]->br_port;
				}
			}

			if(tmpp) {
				// the last uplink port
				if((skb1 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return 0;
				}
				br_forward(tmpp, skb1);
				skb1 = NULL;
			}
			skb = NULL;
		}
		/******************end**************/
	} else if ((dst = __br_fdb_get(br, dest)) && dst->is_local) {
		skb2 = skb;
		/* Do not forward the packet since it's local. */
		skb = NULL;
	}
	/*************hanhui upgrade**********/
	else{
		if(br->isolation_enabled && !(p->isuplink)){
			//if unicast isolation enabled do not froward to uplink ports or drop it
			int i = 0;
			struct sk_buff *skb1 = skb;
			struct net_bridge_port * tmpp = NULL;
			for(i = 0;i<UPLINK_MAX_COUNT;i++) {
				if(br->uplink_port[i]) {
					if(tmpp) {
						if((skb1 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
							br->dev->stats.tx_dropped++;
							kfree_skb(skb);
							return 0;
						}
						br_forward(tmpp, skb1);
					}
					tmpp = br->uplink_port[i]->br_port;
				}
			}

			if(tmpp) {
				// the last uplink port, use skb
				br_forward(tmpp, skb);
				goto out;
			}
			else{
				// no uplink port
				goto drop;
			}
		}
		else if(br->unknown_unicast_drop && p->isuplink && (!dst)){
			//drop unknown unicast from uplink port
			goto drop;
		}
	}
	/******************end**************/
	if (skb2 == skb)
		skb2 = skb_clone(skb, GFP_ATOMIC);

	if (skb2)
		br_pass_frame_up(br, skb2);

	if (skb) {
		if (dst)
			br_forward(dst->dst, skb);
		else
			br_flood_forward(br, skb);
	}

out:
	return 0;
drop:
	kfree_skb(skb);
	goto out;
}

/* note: already called with rcu_read_lock (preempt_disabled) */
static int br_handle_local_finish(struct sk_buff *skb)
{
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);

	if (p)
		br_fdb_update(p->br, p, eth_hdr(skb)->h_source);
	return 0;	 /* process further */
}

/* Does address match the link local multicast address.
 * 01:80:c2:00:00:0X
 */
static inline int is_link_local(const unsigned char *dest)
{
	__be16 *a = (__be16 *)dest;
	static const __be16 *b = (const __be16 *)br_group_address;
	static const __be16 m = cpu_to_be16(0xfff0);

	return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | ((a[2] ^ b[2]) & m)) == 0;
}

/*
 * Called via br_handle_frame_hook.
 * Return NULL if skb is handled
 * note: already called with rcu_read_lock (preempt_disabled)
 */
struct sk_buff *br_handle_frame(struct net_bridge_port *p, struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	int (*rhook)(struct sk_buff *skb);
/**************************hanhui upgrade***************/
	int br_ifindex = 0, port_ifindex = 0;
/*******************************end********************/
	if (!is_valid_ether_addr(eth_hdr(skb)->h_source))
		goto drop;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return NULL;

	if (unlikely(is_link_local(dest))) {
		/* Pause frames shouldn't be passed up by driver anyway */
		if (skb->protocol == htons(ETH_P_PAUSE))
			goto drop;

		/* If STP is turned off, then forward */
		if (p->br->stp_enabled == BR_NO_STP && dest[5] == 0)
			goto forward;

		if (NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, skb->dev,
			    NULL, br_handle_local_finish))
			return NULL;	/* frame consumed by filter */
		else
			return skb;	/* continue processing */
	}

forward:
/*****************hanhui upgrade for stp****************/
#ifndef __AX_PLATFORM__	
	if (br_user_mstp && p->dev && p->br->dev &&
		!memcmp(dest, br_group_address, 5) && !(dest[5] & 0xF0)) {
		br_ifindex = p->br->dev->ifindex;
		port_ifindex = p->dev->ifindex;
		br_netlink_send_msg2user(br_ifindex, port_ifindex, skb);
		
		kfree_skb(skb);
		return NULL;
	}
#endif
/****************************end*********************/
	switch (p->state) {
	case BR_STATE_FORWARDING:
		rhook = rcu_dereference(br_should_route_hook);
		if (rhook != NULL) {
			if (rhook(skb))
				return skb;
			dest = eth_hdr(skb)->h_dest;
		}
		/* fall through */
	case BR_STATE_LEARNING:
		if (!compare_ether_addr(p->br->dev->dev_addr, dest))
			skb->pkt_type = PACKET_HOST;

		NF_HOOK(PF_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL,
			br_handle_frame_finish);
		break;
	default:
drop:
		kfree_skb(skb);
	}
	return NULL;
}
