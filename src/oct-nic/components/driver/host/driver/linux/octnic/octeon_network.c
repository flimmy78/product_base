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


#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "octeon_network.h"
#include "octeon_macros.h"
#include "octeon_nic.h"
#include "octeon_rpa.h"
#include <linux/autelan_product.h>


extern struct  octdev_props_t  *octprops[MAX_OCTEON_DEVICES];
extern product_info_t autelan_product_info;
extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern uint32_t (*cvm_rpa_rx_hook)(struct net_device *dev, struct sk_buff *skb, unsigned int flag);
extern int oct_debug;

#define OCT_NIC_TX_OK     NETDEV_TX_OK
#define OCT_NIC_TX_BUSY   NETDEV_TX_BUSY






void oct_net_setup_if(octeon_recv_info_t  *recv_info, void *buf);
void octeon_network_free_tx_buf(octeon_req_status_t status, void *arg);







void octnet_napi_enable(octnet_priv_t  *priv);
void octnet_napi_disable(octnet_priv_t  *priv);

oct_poll_fn_status_t octnet_check_txq_status(void *oct,unsigned long props_ptr);







void printPacketBuffer(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;

	if(!buffer)
		return;
	cavium_error(":::::::::::::::::::::::1::::::::::::::::::::::::\n");
	
	for(i = 0;i < buffLen ; i++)
	{
		cavium_print_msg("%02x ",buffer[i]);
		if(0==(i+1)%16) {
			cavium_print_msg("\n");
		}
	}
	if((buffLen%16)!=0)
	{
		cavium_print_msg("\n");
	}
	cavium_error("::::::::::::::::::::::::1:::::::::::::::::::::::\n");
}



static inline void
__octnet_stop_txqueue(octnet_os_devptr_t  *pndev)
{
	octnet_priv_t        *priv = GET_NETDEV_PRIV(pndev);

	netif_stop_queue(pndev);
	OCTNET_IFSTATE_RESET(priv, OCT_NIC_IFSTATE_TXENABLED);
}







static inline int
__octnet_start_txqueue(octnet_os_devptr_t  *pndev, int restart)
{
	octnet_priv_t  *priv = GET_NETDEV_PRIV(pndev);

	if(OCTNET_IFSTATE_CHECK(priv, OCT_NIC_IFSTATE_TXENABLED))
		return 0;

	if(priv->linfo.link.s.status) {
		if(restart) {
			netif_wake_queue(pndev);
		} else {
			netif_start_queue(pndev);
		}
		OCTNET_IFSTATE_SET(priv, OCT_NIC_IFSTATE_TXENABLED);
		return 0;
	}

	return 1;
}






void
octnet_stop_txqueue(octnet_os_devptr_t  *pndev)
{
	__octnet_stop_txqueue(pndev);
}



void
octnet_start_txqueue(octnet_os_devptr_t  *pndev)
{
	__octnet_start_txqueue((pndev), 0);
}


void
octnet_restart_txqueue(octnet_os_devptr_t  *pndev)
{
	__octnet_start_txqueue((pndev), 1);
}





static inline int
__check_txq_state(octnet_priv_t  *priv)
{
	if(OCTNET_IFSTATE_CHECK(priv, OCT_NIC_IFSTATE_TXENABLED))
		return 0;
	
	if(octnet_iq_is_full(priv->oct_dev, priv->txq))
		return 0;
	
	if(octnet_iq_bp_on(priv->oct_dev, priv->txq))
		return 0;
	
	OCTNET_IFSTATE_SET(priv, OCT_NIC_IFSTATE_TXENABLED);
	netif_wake_queue(priv->pndev);

	return 1;
}




oct_poll_fn_status_t
octnet_poll_check_txq_status(void            *oct,
                             unsigned long    ul_priv)
{
	if(!OCTNET_IFSTATE_CHECK((octnet_priv_t *)ul_priv, OCT_NIC_IFSTATE_RUNNING))
		return OCT_POLL_FN_FINISHED;

	__check_txq_state((octnet_priv_t *)ul_priv);

	return OCT_POLL_FN_CONTINUE;
}





void
__setup_tx_poll_fn(octnet_os_devptr_t *pndev)
{
	octeon_poll_ops_t    poll_ops;
	octnet_priv_t       *priv = GET_NETDEV_PRIV(pndev);

	poll_ops.fn     = octnet_poll_check_txq_status;
	poll_ops.fn_arg = (unsigned long)priv;
	poll_ops.ticks  = 1;
	poll_ops.rsvd   = 0xff;
	octeon_register_poll_fn(get_octeon_device_id(priv->oct_dev), &poll_ops);
}




/* Net device open */
int
octnet_open(struct net_device *pndev)
{
	octnet_priv_t      *priv = GET_NETDEV_PRIV(pndev);

	netif_start_queue(pndev);

	OCTNET_IFSTATE_SET(priv, OCT_NIC_IFSTATE_RUNNING);

	__setup_tx_poll_fn(pndev);

	CVM_MOD_INC_USE_COUNT;

	return 0;
}




/* Net device close */
int
octnet_stop(struct net_device *pndev)
{
	octnet_priv_t  *priv = GET_NETDEV_PRIV(pndev);

	OCTNET_IFSTATE_RESET(priv, OCT_NIC_IFSTATE_RUNNING);
	/* This is a hack that allows DHCP to continue working. */
	if(OCT_NIC_USE_NAPI) {
		set_bit(__LINK_STATE_START, &priv->pndev->state);
	}
	netif_stop_queue(pndev);

	CVM_MOD_DEC_USE_COUNT;

	return 0;
}







/*
   This routine is called by the callback function when a ctrl pkt sent to
   core app completes. The nctrl_ptr contains a copy of the command type
   and data sent to the core app. This routine is only called if the ctrl
   pkt was sent successfully to the core app.
*/
void
octnet_link_ctrl_cmd_completion(void  *nctrl_ptr)
{
	octnic_ctrl_pkt_t   *nctrl = (octnic_ctrl_pkt_t  *)nctrl_ptr;
	octnet_os_devptr_t  *pndev;
	octnet_priv_t       *priv;

	pndev = (octnet_os_devptr_t *)nctrl->netpndev;
	priv  = GET_NETDEV_PRIV(pndev);

	switch(nctrl->ncmd.s.cmd) {
		case OCTNET_CMD_CHANGE_DEVFLAGS:
			/* Save a copy of the flags sent to core app in the private area. */
			priv->core_flags = nctrl->udd[0];
			break;

		case OCTNET_CMD_CHANGE_MACADDR:
			/* If command is successful, change the MACADDR for net device. */
			cavium_print_msg("OCTNIC: %s MACAddr changed to 0x%llx\n",
			                octnet_get_devname(pndev),
			                CVM_CAST64(nctrl->udd[0]));
			cavium_memcpy(pndev->dev_addr, ((uint8_t *)&nctrl->udd[0]) + 2, ETH_ALEN);
			break;

		case OCTNET_CMD_CHANGE_MTU:
			/* If command is successful, change the MTU for net device. */
			cavium_print_msg("OCTNIC: %s MTU Changed from %d to %d\n",
			                 octnet_get_devname(pndev), pndev->mtu,
			                 nctrl->ncmd.s.param2);
			pndev->mtu = nctrl->ncmd.s.param2;
			break;

		default:
			cavium_error("OCTNIC: %s Unknown cmd: %d\n", __CVM_FUNCTION__,
			             nctrl->ncmd.s.cmd);
	}

}






/* This routine generates a octnet_ifflags_t mask from the net device flags
   received from the OS. */
static inline octnet_ifflags_t
octnet_get_new_flags(octnet_os_devptr_t  *pndev)
{
	octnet_ifflags_t     f = 0;

	if(pndev->flags & IFF_PROMISC) {
		f |= OCTNET_IFFLAG_PROMISC;
	}

	if(pndev->flags & IFF_ALLMULTI) {
		f |= OCTNET_IFFLAG_ALLMULTI;
	}

	if(pndev->flags & IFF_MULTICAST) {
		f |= OCTNET_IFFLAG_MULTICAST;
	}

	return f;
}








/* Net device set_multicast_list */
void
octnet_set_mcast_list(octnet_os_devptr_t  *pndev)
{
	octnet_priv_t       *priv = GET_NETDEV_PRIV(pndev);
	octnic_ctrl_pkt_t    nctrl;
	int                  ret;

	if(pndev->flags == priv->pndev_flags)
		return;

	/* Save the OS net device flags. */
	priv->pndev_flags  = pndev->flags;

	/* Create a ctrl pkt command to be sent to core app. */
	nctrl.ncmd.u64      = 0;
	nctrl.ncmd.s.cmd    = OCTNET_CMD_CHANGE_DEVFLAGS;
	nctrl.ncmd.s.param1 = priv->linfo.ifidx;
	nctrl.ncmd.s.param2 = 0;
	nctrl.ncmd.s.more   = 1;
	nctrl.netpndev      = (unsigned long)pndev;
	nctrl.cb_fn         = octnet_link_ctrl_cmd_completion;

	nctrl.udd[0]        = (uint64_t)octnet_get_new_flags(pndev);
	octeon_swap_8B_data(&nctrl.udd[0], 1);

	/* Apparently, any activity in this call from the kernel has to
	   be atomic. So we won't wait for response. */
	nctrl.wait_time     = 0;

#if !defined(ETHERPCI)

	ret = octnet_send_nic_ctrl_pkt(priv->oct_dev, &nctrl);
	if(ret) {
		cavium_error("OCTNIC: DevFlags change failed in core (ret: 0x%x)\n",
	             ret);
	}

#else
	octnet_link_ctrl_cmd_completion( (void  *)&nctrl);
	ret = 0;
#endif
}









/* Net device set_mac_address */
int
octnet_set_mac(struct net_device *pndev, void *addr)
{
	int                  ret = 0;
	octnet_priv_t       *priv;
	struct sockaddr     *p_sockaddr = (struct sockaddr *)addr;
	octnic_ctrl_pkt_t    nctrl;

	priv = GET_NETDEV_PRIV(pndev);

	nctrl.ncmd.u64      = 0;
	nctrl.ncmd.s.cmd    = OCTNET_CMD_CHANGE_MACADDR;
	nctrl.ncmd.s.param1 = priv->linfo.ifidx;
	nctrl.ncmd.s.param2 = 0;
	nctrl.ncmd.s.more   = 1;
	nctrl.netpndev      = (unsigned long)pndev;
	nctrl.cb_fn         = octnet_link_ctrl_cmd_completion;
	/*wangchong@autelan.com 2013-2-23 for ves* can`t get MAC on AX8606*/
	nctrl.wait_time     = 0;

	nctrl.udd[0] = 0;
	/* The MAC Address is presented in network byte order. */
	cavium_memcpy((uint8_t *)&nctrl.udd[0] + 2, p_sockaddr->sa_data, ETH_ALEN);

#if  !defined(ETHERPCI)
	ret = octnet_send_nic_ctrl_pkt(priv->oct_dev, &nctrl);
	if(ret) {
		cavium_error("OCTNIC: MAC Address change failed\n");
		ret = -EBUSY;
	}
#else
	octnet_link_ctrl_cmd_completion( (void  *)&nctrl);
	ret = 0;
#endif

	return ret;
}





/* Net device change_mtu */
int
octnet_change_mtu(struct net_device *pndev, int new_mtu)
{
	octnet_priv_t       *priv;
	octnic_ctrl_pkt_t    nctrl;
	int                  ret = 0, max_frm_size = new_mtu + 18;

	cavium_print(PRINT_FLOW,"OCTNIC: %s called\n", __CVM_FUNCTION__);

	/* Limit the MTU to make sure the ethernet packets are between 64 bytes
	   and 65535 bytes */
	if ( (max_frm_size < OCTNET_MIN_FRM_SIZE)
		 || (max_frm_size > OCTNET_MAX_FRM_SIZE))  {
		cavium_error("OCTNIC: Invalid MTU: %d (Valid values are between %d and %d)\n",
		             new_mtu, (OCTNET_MIN_FRM_SIZE - 18),
		             (OCTNET_MAX_FRM_SIZE - 18));
		return -EINVAL;
	}


	priv = GET_NETDEV_PRIV(pndev);

#if 0
	if(octeon_reset_oq_bufsize(get_octeon_device_id(priv->oct_dev), priv->rxq, new_mtu)) {
		cavium_error("Error changing output queue buffer size\n");
		ret = -EINVAL;
		goto change_mtu_finish;
	}
#endif

	nctrl.ncmd.u64      = 0;
	nctrl.ncmd.s.cmd    = OCTNET_CMD_CHANGE_MTU;
	nctrl.ncmd.s.param1 = priv->linfo.ifidx;
	nctrl.ncmd.s.param2 = new_mtu;
	nctrl.wait_time     = 100;
	nctrl.netpndev      = (unsigned long)pndev;
	nctrl.cb_fn         = octnet_link_ctrl_cmd_completion;

#if  !defined(ETHERPCI)
	ret = octnet_send_nic_ctrl_pkt(priv->oct_dev, &nctrl);
	if(ret) {
		cavium_error("OCTNIC: Failed to set MTU\n");
		ret = -EINVAL;
	}
#else
	octnet_link_ctrl_cmd_completion( (void  *)&nctrl);
	ret = 0;
#endif

	return ret;
}



/** Routine to push packets arriving on Octeon interface upto network layer.
  * @param oct_dev  - pointer to octeon device.
  * @param skbuff   - skbuff struct to be passed to network layer.
  * @param len      - size of total data received.
  * @param port     - Octeon port on which data was received.
  */
void
octnet_push_packet(int                  octeon_id,
                   void                *skbuff,
                   uint32_t             len,
                   octeon_resp_hdr_t   *resp_hdr)
{
	struct sk_buff     *skb   = (struct sk_buff *)skbuff;
	octnet_os_devptr_t *pndev = (octnet_os_devptr_t *)octprops[octeon_id]->pndev[resp_hdr->dest_qport];
    rpa_packet_flag rpa_pkt_flag = NOT_RPA_PKT;
	unsigned int rpa_type_position;
    unsigned int base_local_slot_num;
	//int oct_debug = 0;
	enum cvm_oct_callback_result callback_result;

	if (oct_debug) {
		cavium_error("Octeon NIC before remove rpa tag.\n");
		printPacketBuffer(skb->data, skb->len);
	}
	
	if (autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU)
		rpa_type_position = 0x10;
	else
		rpa_type_position = 0xc;

	base_local_slot_num = autelan_product_info.board_slot_id - 1;

	if(pndev) {
		octnet_priv_t  *priv  = GET_NETDEV_PRIV(pndev);

		rpa_pkt_flag = NOT_RPA_PKT;
		if (*((unsigned short*)(skb->data + rpa_type_position)) == RPA_COOKIE) {
			rpa_eth_header *rpa_head = (rpa_eth_header *)(skb->data + (rpa_type_position - 0xc));
			unsigned char slotNum = (rpa_head->d_s_slotNum >> 4) & 0x0f;
			if ((unsigned int)slotNum == base_local_slot_num) {
    			rpa_pkt_flag = NORMAL_RPA_PKT;

    			/* need send to protocol stack? */
    			if ((rpa_head->type & 0x0f) == 0x01){
					int rpa_slotNum = (rpa_head->d_s_slotNum) & 0x0f;
					int rpa_netdevNum = rpa_head->snetdevNum;
					/* get right device which send the arp-reply to kernel */
					if (cvm_rpa_dev_index[rpa_slotNum][rpa_netdevNum].netdev) {
						skb->dev = cvm_rpa_dev_index[rpa_slotNum][rpa_netdevNum].netdev;
						//cavium_error("huangjing##dev->name : %s\n", skb->dev->name);
						skb_pull(skb, rpa_type_position + 0x6);
						rpa_pkt_flag = TO_PS_RPA_PKT;
					}else {
            			free_recv_buffer(skb);
            			priv->stats.rx_dropped++;
            			return;
					}
				}
	        }else{
                free_recv_buffer(skb);
				priv->stats.rx_dropped++;
				return;
			}
		}

	
		/* Do not proceed if the interface is not in RUNNING state. */
		if( !(cavium_atomic_read(&priv->ifstate) & OCT_NIC_IFSTATE_RUNNING)) {
			free_recv_buffer(skb);
			priv->stats.rx_dropped++;
			return;
		}

		/* if received skb by rpa(not a arp-reply), not parse the L2 head */
    	if(rpa_pkt_flag == NOT_RPA_PKT) {
        	skb->dev       = pndev;
            skb->protocol  = eth_type_trans(skb, skb->dev);
        	skb->ip_summed = CHECKSUM_NONE;

        	if(netif_rx(skb) != NET_RX_DROP) {
        		priv->stats.rx_bytes += len;
        		priv->stats.rx_packets++;
        		pndev->last_rx  = jiffies;
        	} else {
        		priv->stats.rx_dropped++;
        	}
    	}else if (rpa_pkt_flag == TO_PS_RPA_PKT) {
			skb->protocol = eth_type_trans(skb, skb->dev);
			skb->protocol  = eth_type_trans(skb, skb->dev);
        	skb->ip_summed = CHECKSUM_NONE;

        	if(netif_rx(skb) != NET_RX_DROP) {
        		priv->stats.rx_bytes += len;
        		priv->stats.rx_packets++;
        		pndev->last_rx  = jiffies;
        	} else {
        		priv->stats.rx_dropped++;
        	}
		}else if (rpa_pkt_flag == NORMAL_RPA_PKT) {
        	if (cvm_rpa_rx_hook) {
            	callback_result = cvm_rpa_rx_hook(skb->dev, skb, rpa_pkt_flag);
            }
			else {
				callback_result = CVM_OCT_DROP;
			}
			if(callback_result != CVM_OCT_TAKE_OWNERSHIP_SKB)
			{
                free_recv_buffer(skb);
			}
		}
		
	} else  {

		free_recv_buffer(skb);
	    }

}










void
octnic_free_netbuf(void *buf)
{
	struct sk_buff               *skb;
	struct octnet_buf_free_info  *finfo;
	octnet_priv_t                *priv;

	finfo = (struct octnet_buf_free_info  *)buf;
	skb   = finfo->skb;
	priv  = finfo->priv;

	octeon_unmap_single_buffer(get_octeon_device_id(priv->oct_dev),
	                           finfo->dptr, skb->len,
	                           CAVIUM_PCI_DMA_TODEVICE);
	free_recv_buffer((cavium_netbuf_t *)skb);

	__check_txq_state(priv);
}




void
octnic_free_netsgbuf(void *buf)
{
	struct octnet_buf_free_info  *finfo;
	struct sk_buff               *skb;
	octnet_priv_t                *priv;
	struct octnic_gather         *g;
	int                           i, frags;


	finfo    = (struct octnet_buf_free_info *)buf;
	skb      = finfo->skb;
	priv     = finfo->priv;
	g        = finfo->g;
	frags    = skb_shinfo(skb)->nr_frags;


	octeon_unmap_single_buffer(get_octeon_device_id(priv->oct_dev),
	                           g->sg[0].ptr[0], (skb->len - skb->data_len),
	                           CAVIUM_PCI_DMA_TODEVICE);

	i = 1;
	while(frags--) {
		struct skb_frag_struct  *frag = &skb_shinfo(skb)->frags[i-1];

		octeon_unmap_page(get_octeon_device_id(priv->oct_dev),
		                  g->sg[(i >> 2)].ptr[(i&3)],
		                  frag->size, CAVIUM_PCI_DMA_TODEVICE);
		i++;
	}

	octeon_unmap_single_buffer(get_octeon_device_id(priv->oct_dev),
	                           finfo->dptr, g->sg_size,
	                           CAVIUM_PCI_DMA_TODEVICE);

	cavium_spin_lock(&priv->lock);
	cavium_list_add_tail(&g->list, &priv->glist);
	cavium_spin_unlock(&priv->lock);

	free_recv_buffer((cavium_netbuf_t *)skb);

	__check_txq_state(priv);
}






void
print_ip_header(struct iphdr *ip)
{
	cavium_print_msg("ip: tos: %x tot_len: %x id: %x frag_off: %x ttl: %x proto: %x\n", ip->tos, ip->tot_len, ip->id, ip->frag_off, ip->ttl, ip->protocol);
}




static inline int
is_ipv4(struct sk_buff  *skb)
{
	return  ( (skb->protocol == htons(ETH_P_IP))
			 && (cvm_ip_hdr(skb)->version == 4)
			 && (cvm_ip_hdr(skb)->ihl == 5));
}



static inline int
is_ip_fragmented(struct sk_buff  *skb)
{
	/* The Don't fragment and Reserved flag fields are ignored.
	   IP is fragmented if
	   -  the More fragments bit is set (indicating this IP is a fragment with
		  more to follow; the current offset could be 0 ). 
 	   -  ths offset field is non-zero. */
	return ( htons(cvm_ip_hdr(skb)->frag_off) & 0x3fff);
}




static inline int
is_ipv6(struct sk_buff  *skb)
{
	return  ( (skb->protocol == htons(ETH_P_IPV6))
			 && (cvm_ip6_hdr(skb)->version == 6));
}




static inline int
is_wo_extn_hdr(struct sk_buff  *skb)
{
	return  ( (cvm_ip6_hdr(skb)->nexthdr == IPPROTO_TCP) ||
			(cvm_ip6_hdr(skb)->nexthdr == IPPROTO_UDP));
}



static inline int
is_tcpudp(struct sk_buff  *skb)
{
	return    ( (cvm_ip_hdr(skb)->protocol == IPPROTO_TCP)
			 || (cvm_ip_hdr(skb)->protocol == IPPROTO_UDP));
}


/* ipfwd_learn add dev_type to identify the caller of different net_device driver 20121127*/ 
extern int (*cvm_ipfwd_learn_tx_hook) (struct sk_buff *,uint8_t dev_type);


int
octnet_xmit(struct sk_buff *skb, struct net_device *pndev)
{
	octnet_priv_t                 *priv;
	struct octnet_buf_free_info   *finfo;
	octnic_cmd_setup_t             cmdsetup;
	octnic_data_pkt_t              ndata;
	int                            status = 0,maclen = 12;
	uint8_t dev_type = 2; /* 2 is pcie octnic driver. defined in ipfwd_learn.h */

	cavium_print(PRINT_FLOW,"OCTNIC: network xmit called\n");

    /*wangchong@autelan.com   drop packets without vlan&rpa for ve*s*  */
	priv = GET_NETDEV_PRIV(pndev);
	priv->stats.tx_packets++;
	
    if( (autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X)||\
		((autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C)&&(priv->ifidx == 4)) )
    {
    	if((*((unsigned short*)(skb->data + maclen)) != RPA_COOKIE)&&\
    		(*((unsigned short*)(skb->data + maclen)) != VLAN_ETHER_TYPE))
    	{
    		goto oct_xmit_failed;
    	}
    }

    /* call learn function form the ipfwd_learn module */
	/* ipfwd_learn add dev_type to identify the caller of different net_device driver 20121127*/ 
    if(cvm_ipfwd_learn_tx_hook)
    {   
		cvm_ipfwd_learn_tx_hook(skb, dev_type);
    }
	
	if(!OCTNET_IFSTATE_CHECK(priv, OCT_NIC_IFSTATE_TXENABLED)) {
		return OCT_NIC_TX_BUSY;
	}

	/* Check for all conditions in which the current packet cannot be
	   transmitted. */
	if( !(cavium_atomic_read(&priv->ifstate) &  OCT_NIC_IFSTATE_RUNNING)
		|| (!priv->linfo.link.s.status)
		|| (octnet_iq_is_full(priv->oct_dev, priv->txq))
		|| (skb->len <= 0) ) {
		goto oct_xmit_failed;
	}

	/* Use space in skb->cb to store info used to unmap and free the buffers. */
	finfo       = (struct octnet_buf_free_info *)skb->cb;
	finfo->priv = priv;
	finfo->skb  = skb;

	/* Prepare the attributes for the data to be passed to OSI. */
	ndata.buf       = (void *)finfo;
	ndata.q_no      = priv->txq;
	ndata.datasize  = skb->len;

	cmdsetup.u64       = 0;
	cmdsetup.s.ifidx = priv->linfo.ifidx;


	if( (is_ipv4(skb) && !is_ip_fragmented(skb) && is_tcpudp(skb)) ||
		(is_ipv6(skb) && is_wo_extn_hdr(skb)) ) {
			cmdsetup.s.cksum_offset = sizeof(struct ethhdr) + 1;
	}




	if(skb_shinfo(skb)->nr_frags == 0) {

		cmdsetup.s.u.datasize = skb->len;

		octnet_prepare_pci_cmd(&(ndata.cmd), &cmdsetup);

		/* Offload checksum calculation for TCP/UDP packets */
		ndata.cmd.dptr =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
		                          skb->data, skb->len, CAVIUM_PCI_DMA_TODEVICE);

		finfo->dptr     = ndata.cmd.dptr;

		ndata.buftype   = NORESP_BUFTYPE_NET;

	} else {
		int                     i, frags;
		struct skb_frag_struct *frag;
		struct octnic_gather   *g;

		cavium_spin_lock(&priv->lock);
		g = (struct octnic_gather *)cavium_list_delete_head(&priv->glist);
		cavium_spin_unlock(&priv->lock);

		if(g == NULL)
			goto oct_xmit_failed;

		cmdsetup.s.gather       = 1;
		cmdsetup.s.u.gatherptrs = (skb_shinfo(skb)->nr_frags + 1);
		octnet_prepare_pci_cmd(&(ndata.cmd), &cmdsetup);

		memset(g->sg, 0, g->sg_size);

		g->sg[0].ptr[0] =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
 		                             skb->data, (skb->len - skb->data_len),
		                             CAVIUM_PCI_DMA_TODEVICE);
		CAVIUM_ADD_SG_SIZE(&(g->sg[0]), (skb->len - skb->data_len), 0);


		frags = skb_shinfo(skb)->nr_frags;
		i = 1;
		while(frags--) {
			frag = &skb_shinfo(skb)->frags[i-1];

			g->sg[(i >> 2)].ptr[(i&3)] =
					 octeon_map_page(get_octeon_device_id(priv->oct_dev), 			                             frag->page, frag->page_offset,
			                         frag->size,CAVIUM_PCI_DMA_TODEVICE);
			CAVIUM_ADD_SG_SIZE(&(g->sg[(i >> 2)]), frag->size, (i&3));
			i++;
		}


		ndata.cmd.dptr =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
		                           g->sg, g->sg_size, CAVIUM_PCI_DMA_TODEVICE);

		finfo->dptr      = ndata.cmd.dptr;
		finfo->g         = g;

		ndata.buftype   = NORESP_BUFTYPE_NET_SG;
	}

	status = octnet_send_nic_data_pkt(priv->oct_dev, &ndata);
	if(status == NORESP_SEND_FAILED)
		goto oct_xmit_failed;

	if(status == NORESP_SEND_STOP)  {
		netif_stop_queue(pndev);
		OCTNET_IFSTATE_RESET(priv, OCT_NIC_IFSTATE_TXENABLED);
	}

	pndev->trans_start = jiffies;

	return OCT_NIC_TX_OK;

oct_xmit_failed:
	priv->stats.tx_dropped++;
	free_recv_buffer(skb);
	return OCT_NIC_TX_OK;
}




int
octnet_fccp_xmit(struct sk_buff *skb, struct net_device *pndev)
{
	octnet_priv_t                 *priv;
	struct octnet_buf_free_info   *finfo;
	octnic_cmd_setup_t             cmdsetup;
	octnic_data_pkt_t              ndata;
	int                            status = 0;


	priv = GET_NETDEV_PRIV(pndev);
	priv->stats.tx_packets++;

	if(!OCTNET_IFSTATE_CHECK(priv, OCT_NIC_IFSTATE_TXENABLED)) {
		return OCT_NIC_TX_BUSY;
	}

	/* Check for all conditions in which the current packet cannot be
	   transmitted. */
	if( !(cavium_atomic_read(&priv->ifstate) &  OCT_NIC_IFSTATE_RUNNING)
		|| (!priv->linfo.link.s.status)
		|| (octnet_iq_is_full(priv->oct_dev, priv->txq))
		|| (skb->len <= 0) ) {
		goto oct_xmit_failed;
	}

	/* Use space in skb->cb to store info used to unmap and free the buffers. */
	finfo       = (struct octnet_buf_free_info *)skb->cb;
	finfo->priv = priv;
	finfo->skb  = skb;

	/* Prepare the attributes for the data to be passed to OSI. */
	ndata.buf       = (void *)finfo;
	ndata.q_no      = priv->txq;
	ndata.datasize  = skb->len;

	cmdsetup.u64       = 0;
	cmdsetup.s.ifidx = priv->linfo.ifidx;


	if( (is_ipv4(skb) && !is_ip_fragmented(skb) && is_tcpudp(skb)) ||
		(is_ipv6(skb) && is_wo_extn_hdr(skb)) ) {
			cmdsetup.s.cksum_offset = sizeof(struct ethhdr) + 1;
	}




	if(skb_shinfo(skb)->nr_frags == 0) {

		cmdsetup.s.u.datasize = skb->len;

		octnet_prepare_pci_cmd(&(ndata.cmd), &cmdsetup);

		/* Offload checksum calculation for TCP/UDP packets */
		ndata.cmd.dptr =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
		                          skb->data, skb->len, CAVIUM_PCI_DMA_TODEVICE);

		finfo->dptr     = ndata.cmd.dptr;

		ndata.buftype   = NORESP_BUFTYPE_NET;

	} else {
		int                     i, frags;
		struct skb_frag_struct *frag;
		struct octnic_gather   *g;

		cavium_spin_lock(&priv->lock);
		g = (struct octnic_gather *)cavium_list_delete_head(&priv->glist);
		cavium_spin_unlock(&priv->lock);

		if(g == NULL)
			goto oct_xmit_failed;

		cmdsetup.s.gather       = 1;
		cmdsetup.s.u.gatherptrs = (skb_shinfo(skb)->nr_frags + 1);
		octnet_prepare_pci_cmd(&(ndata.cmd), &cmdsetup);

		memset(g->sg, 0, g->sg_size);

		g->sg[0].ptr[0] =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
 		                             skb->data, (skb->len - skb->data_len),
		                             CAVIUM_PCI_DMA_TODEVICE);
		CAVIUM_ADD_SG_SIZE(&(g->sg[0]), (skb->len - skb->data_len), 0);


		frags = skb_shinfo(skb)->nr_frags;
		i = 1;
		while(frags--) {
			frag = &skb_shinfo(skb)->frags[i-1];

			g->sg[(i >> 2)].ptr[(i&3)] =
					 octeon_map_page(get_octeon_device_id(priv->oct_dev), 			                             frag->page, frag->page_offset,
			                         frag->size,CAVIUM_PCI_DMA_TODEVICE);
			CAVIUM_ADD_SG_SIZE(&(g->sg[(i >> 2)]), frag->size, (i&3));
			i++;
		}


		ndata.cmd.dptr =
		          octeon_map_single_buffer(get_octeon_device_id(priv->oct_dev),
		                           g->sg, g->sg_size, CAVIUM_PCI_DMA_TODEVICE);

		finfo->dptr      = ndata.cmd.dptr;
		finfo->g         = g;

		ndata.buftype   = NORESP_BUFTYPE_NET_SG;
	}

	status = octnet_send_nic_data_pkt(priv->oct_dev, &ndata);
	if(status == NORESP_SEND_FAILED)
		goto oct_xmit_failed;


	if(status == NORESP_SEND_STOP)  {
		netif_stop_queue(pndev);
		OCTNET_IFSTATE_RESET(priv, OCT_NIC_IFSTATE_TXENABLED);
	}

	pndev->trans_start = jiffies;

	return OCT_NIC_TX_OK;

oct_xmit_failed:
	priv->stats.tx_dropped++;
	free_recv_buffer(skb);
	return OCT_NIC_TX_OK;
}
EXPORT_SYMBOL(octnet_fccp_xmit);



void
octnet_napi_enable(octnet_priv_t  *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	napi_enable(&priv->napi);
#else
	netif_poll_enable(priv->pndev);
#endif
}


void
octnet_napi_disable(octnet_priv_t  *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	napi_disable(&priv->napi);
#else
	netif_poll_disable(priv->pndev);
#endif
}




void
octnet_notify_napi_complete(octnet_priv_t  *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
	napi_complete(&priv->napi);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	netif_rx_complete(priv->pndev, &priv->napi);
#else
	netif_rx_complete(priv->pndev);
#endif
}


void
octnet_notify_napi_start(octnet_priv_t  *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
	napi_schedule(&priv->napi);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	netif_rx_schedule(priv->pndev, &priv->napi);
#else
	netif_rx_schedule(priv->pndev);
#endif
}





void
octnet_napi_drv_callback(int oct_id, int oq_no, int event)
{
	octnet_priv_t  *priv = GET_NETDEV_PRIV(octprops[oct_id]->pndev[oq_no]);
	if(event == POLL_EVENT_INTR_ARRIVED) {
		octnet_notify_napi_start(priv);
	}
}




int
octnet_napi_do_rx(octnet_priv_t  *priv, int budget)
{
	int   work_done, oct_id;

	oct_id = get_octeon_device_id(priv->oct_dev);

	work_done = octeon_process_droq_poll_cmd(oct_id, priv->rxq,
	                            POLL_EVENT_PROCESS_PKTS, budget);
	if(work_done < 0) {
		cavium_error("\n %s: CHECK THE OCTEON DEVICE ID OR DROQ NUMBER\n", __FUNCTION__);
		goto octnet_napi_finish;
	}

	if(work_done > budget) {
		cavium_error(">>>> %s work_done: %d budget: %d\n", __FUNCTION__, work_done,
		        budget);
	}

	return work_done;

octnet_napi_finish:
	octnet_notify_napi_complete(priv);
	octeon_process_droq_poll_cmd(oct_id, priv->rxq, POLL_EVENT_ENABLE_INTR, 0);
	return 0;
}






#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

int
octnet_napi_poll(struct napi_struct *napi, int budget)
{
	struct net_device  *pndev  = napi->dev;
	octnet_priv_t      *priv   = GET_NETDEV_PRIV(pndev);
	int                 work_done;

	work_done = octnet_napi_do_rx(priv, budget);

	if(work_done < budget) {
		int oct_id = get_octeon_device_id(priv->oct_dev);
		octnet_notify_napi_complete(priv);
		octeon_process_droq_poll_cmd(oct_id, priv->rxq, POLL_EVENT_ENABLE_INTR, 0);
		return 0;
	}

	return work_done;
}

#else

int
octnet_napi_poll(struct net_device  *pndev, int  *budget)
{
	int              work_done = 0;
	octnet_priv_t   *priv   = GET_NETDEV_PRIV(pndev);

	work_done = octnet_napi_do_rx(priv, *budget);

	*budget      -= work_done;
	pndev->quota -= work_done;

	if(work_done < *budget) {
		int oct_id = get_octeon_device_id(priv->oct_dev);
		octnet_notify_napi_complete(priv);
		octeon_process_droq_poll_cmd(oct_id, priv->rxq, POLL_EVENT_ENABLE_INTR, 0);
		return 0;
	}

	return 1;
}

#endif





struct net_device_stats *
octnet_stats(struct net_device *pndev)
{
	cavium_print(PRINT_FLOW,"octnet_stats: network stats called\n");
	return &(GET_NETDEV_PRIV(pndev)->stats);
}





void
octnet_tx_timeout(struct net_device *pndev)
{
	cavium_error("OCTNIC: tx timeout for %s\n", octnet_get_devname(pndev));
	pndev->trans_start = jiffies;
	netif_wake_queue(pndev);
}


oct_link_info_t *octnet_get_netdev_linfo(octnet_os_devptr_t *pndev)
{
    octnet_priv_t        *priv = GET_NETDEV_PRIV(pndev);

    return &(priv->linfo);
}

uint16_t octnet_get_netdev_gmxport(octnet_os_devptr_t *pndev)
{
    octnet_priv_t        *priv = GET_NETDEV_PRIV(pndev);

    return (priv->linfo.gmxport);
}
EXPORT_SYMBOL(octnet_get_netdev_gmxport);



int octnet_get_nic_ifcount(void)
{
    return octprops[0]->ifcount;
}
EXPORT_SYMBOL(octnet_get_nic_ifcount);

octnet_os_devptr_t** octnet_get_nic_netdevs(void)
{
    return octprops[0]->pndev;
}
EXPORT_SYMBOL(octnet_get_nic_netdevs);

/* $Id: octeon_network.c 62414 2011-07-15 23:06:22Z panicker $ */ 
