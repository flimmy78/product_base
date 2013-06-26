/**********************************************************************
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
**********************************************************************/
#include <linux/autelan_product.h>
#include "ethernet-rpa.h"
/* error: dereferencing pointer to incomplete type, 2012-10-01 */
#include <linux/udp.h>
#include <linux/ip.h>

#undef CVM_OCT_NAPI_POLL
#undef CVM_OCT_NAPI_HAS_CN68XX_SSO

#ifdef CVM_OCT_NAPI_68
#define CVM_OCT_NAPI_POLL cvm_oct_napi_poll_68
#define CVM_OCT_NAPI_HAS_CN68XX_SSO 1
#else
#define CVM_OCT_NAPI_POLL cvm_oct_napi_poll_38
#define CVM_OCT_NAPI_HAS_CN68XX_SSO 0
#endif

 /* for 12c salve cpu targe mode */
#if defined(ETHERPCI)
#define PACKET_FROM_PCI(work)  (IS_PACKET_FROM_PCI(work, CVM_OCT_NAPI_HAS_CN68XX_SSO))
extern int txcomp_port;
#endif

//int rxmax_in_an_interrupt = 18;
//module_param(rxmax_in_an_interrupt, int, 0644);
extern int rx_napi_weight;
extern product_info_t autelan_product_info;
extern unsigned int base_local_slot_num;
extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern unsigned int rpa_type_position;
extern int oct_rpa_debug;
extern int oct_length_debug;
extern int pend_cvm;

extern uint32_t (*cvm_rpa_rx_hook)(struct net_device *dev, struct sk_buff *skb, unsigned int flag);

extern void printPacketBuffer(unsigned char *buffer,unsigned long buffLen);
extern struct sock *se_agent_nl_sock ;
extern int netlink_sendto_se_agent(char *buff,unsigned int len);

extern int (* rcv_rate_limit_hook)(unsigned int *,int);

/**
 * cvm_oct_napi_poll - the NAPI poll function.
 * @napi: The NAPI instance, or null if called from cvm_oct_poll_controller
 * @budget: Maximum number of packets to receive.
 *
 * Returns the number of packets processed.
 */
static int CVM_OCT_NAPI_POLL(struct napi_struct *napi, int budget)
{
	const int	coreid = cvmx_get_core_num();
	u64		old_group_mask;
	u64		old_scratch;
	int		rx_count = 0;
	bool		did_work_request = false;
	bool		packet_copied;
	rpa_packet_flag rpa_pkt_flag = NOT_RPA_PKT;

	char		*p = (char *)cvm_oct_by_pkind;
	unsigned char * ptr_data = NULL;
    unsigned char * temp_data = NULL;
	struct udphdr *uh = NULL;
	struct iphdr *ip = NULL;
    unsigned short ip_type =0;
    unsigned short vlan_type =0;
	
	if (coreid == 0) {
		//pr_crit("Warning : the Core 0 run a NAPI poll unexpectedly!\n");
		goto napi_over;
	}
	
	/* Prefetch cvm_oct_device since we know we need it soon */
	prefetch(&p[0]);
	prefetch(&p[SMP_CACHE_BYTES]);
	prefetch(&p[2 * SMP_CACHE_BYTES]);

	if (USE_ASYNC_IOBDMA) {
		/* Save scratch in case userspace is using it */
		CVMX_SYNCIOBDMA;
		old_scratch = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
	}

	/* Only allow work for our group (and preserve priorities) */
	if (CVM_OCT_NAPI_HAS_CN68XX_SSO) {
		old_group_mask = cvmx_read_csr(CVMX_SSO_PPX_GRP_MSK(coreid));
		cvmx_write_csr(CVMX_SSO_PPX_GRP_MSK(coreid),
			       1ull << pow_receive_group);
		/* Read it back so it takes effect before we request work */
		cvmx_read_csr(CVMX_SSO_PPX_GRP_MSK(coreid));
	} else {
		old_group_mask = cvmx_read_csr(CVMX_POW_PP_GRP_MSKX(coreid));
		cvmx_write_csr(CVMX_POW_PP_GRP_MSKX(coreid),
			       (old_group_mask & ~0xFFFFull) | 1 << pow_receive_group);
	}

	if (USE_ASYNC_IOBDMA) {
		cvmx_pow_work_request_async(CVMX_SCR_SCRATCH, CVMX_POW_NO_WAIT);
		did_work_request = true;
	}

	if (rx_napi_weight < budget){
		budget = rx_napi_weight;
	}
	
	while (rx_count < budget) {
		
		struct sk_buff *skb = NULL;
		struct sk_buff **pskb = NULL;
		struct octeon_ethernet *priv;
		enum cvm_oct_callback_result callback_result;
		bool skb_in_hw;
		cvmx_wqe_t *work;
		int port;
		unsigned int segments;
		unsigned int fragment;		
		int packets_to_replace = 0;
		unsigned int packet_len;
		union cvmx_buf_ptr  packet_ptr;

		if (USE_ASYNC_IOBDMA && did_work_request)
			work = cvmx_pow_work_response_async(CVMX_SCR_SCRATCH);
		else
			work = cvmx_pow_work_request_sync(CVMX_POW_NO_WAIT);

		prefetch(work);
		did_work_request = false;

		if (unlikely(work == NULL))
			break;
		
 /* for 12c salve cpu targe mode */
#ifdef ETHERPCI
		if (PACKET_FROM_PCI(work)){
			cvmx_wqe_set_len(work, cvmx_wqe_get_len(work) - 24);
			work->packet_ptr.s.addr += 24;
			work->packet_ptr.s.size -= 24;
		}
#endif
		packet_ptr = work->packet_ptr;
		pskb = cvm_oct_packet_to_skb(cvm_oct_get_buffer_ptr(packet_ptr));
		prefetch(pskb);

		if (USE_ASYNC_IOBDMA && rx_count < (budget - 1)) {
			cvmx_pow_work_request_async_nocheck(CVMX_SCR_SCRATCH, CVMX_POW_NO_WAIT);
			did_work_request = true;
		}

		if (rx_count == 0) {
			/*
			 * First time through, see if there is enough
			 * work waiting to merit waking another
			 * CPU.
			 */
			int backlog;
			int cores_in_use = core_state.active_cores;
			if (CVM_OCT_NAPI_HAS_CN68XX_SSO) {
				union cvmx_sso_wq_int_cntx counts;
				counts.u64 = cvmx_read_csr(CVMX_SSO_WQ_INT_CNTX(pow_receive_group));
				backlog = counts.s.iq_cnt + counts.s.ds_cnt;
			} else {
				union cvmx_pow_wq_int_cntx counts;
				counts.u64 = cvmx_read_csr(CVMX_POW_WQ_INT_CNTX(pow_receive_group));
				backlog = counts.s.iq_cnt + counts.s.ds_cnt;
			}
			if (backlog > budget * cores_in_use &&
			    napi != NULL &&
			    cores_in_use < (core_state.baseline_cores - 1))
				cvm_oct_enable_one_cpu();
		}
 /* for 12c salve cpu targe mode */
#if defined(ETHERPCI)
		/*
		 * If WORD2[SOFTWARE] & ipprt = -1 then this WQE is a complete for
		 * a TX packet. Normal PCI Rx packets will have port # between 32-35.
		 */
		if ( (work->word2.s.software) && (cvmx_wqe_get_port(work) == txcomp_port)) {
#else
		/*
		 * If WORD2[SOFTWARE] then this WQE is a complete for
		 * a TX packet.
		 */
		if (work->word2.s.software) {
#endif
			struct octeon_ethernet *priv;
			union skb_shared_tx *shtx;
			int packet_qos = work->word0.raw.unused;
			skb = (struct sk_buff *)packet_ptr.u64;
			priv = netdev_priv(skb->dev);
			if (!netif_running(skb->dev))
				netif_wake_queue(skb->dev);
			shtx = skb_tx(skb);
			if (unlikely(shtx->hardware)) {
				if (priv->tx_timestamp_hw) {
					u64 ns = *(u64 *)work->packet_data;
					struct skb_shared_hwtstamps ts;
					ts.syststamp = cvm_oct_ptp_to_ktime(ns);
					ts.hwtstamp = ns_to_ktime(ns);
					skb_tstamp_tx(skb, &ts);
				}
				if (priv->tx_timestamp_sw) {
					u64 ns = *(u64 *)work->packet_data;
					struct skb_shared_hwtstamps ts;
					ts.syststamp = ns_to_ktime(ns);
					ts.hwtstamp = ns_to_ktime(0);
					skb_tstamp_tx(skb, &ts);
				}
			}

			dev_kfree_skb_any(skb);

			cvmx_fpa_free(work, CVMX_FPA_TX_WQE_POOL, DONT_WRITEBACK(1));

			/*
			 * We are done with this one, adjust the queue
			 * depth.
			 */
			cvmx_fau_atomic_add32(priv->tx_queue[packet_qos].fau, -1);
			continue;
		}
		segments = work->word2.s.bufs;
		/* add to check if the packet is fragment 2012-10-01 */
		fragment = work->word2.s.is_frag;
		/*printk(KERN_NOTICE "DEBUG haodi: work->word2.s.bufs = [%d], work->word2.s.is_frag = [%d] \n", work->word2.s.bufs, work->word2.s.is_frag );*/
		
		//skb_in_hw = USE_SKBUFFS_IN_HW && segments > 0;
		skb_in_hw = use_skbuffs_in_hw_flag && segments > 0;
		if (likely(skb_in_hw)) {
			skb = *pskb;
			prefetch(&skb->head);
			prefetch(&skb->len);
		}

		if (CVM_OCT_NAPI_HAS_CN68XX_SSO)
			port = work->word0.pip.cn68xx.pknd;
		else
			port = work->word1.cn38xx.ipprt;

		prefetch(cvm_oct_by_pkind[port]);

		if (port == 0 && (autelan_product_info.board_type==AUTELAN_BOARD_AX81_SMU ||
			autelan_product_info.board_type==AUTELAN_BOARD_AX71_CRSMU || autelan_product_info.board_type == AUTELAN_BOARD_AX81_SMUE))
		{
			/*do nothing for obc0 recieve packets from other boards*/
		}
		else if ((port == 0 || port == 1) && (autelan_product_info.board_type == AUTELAN_BOARD_AX71_2X12G12S ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_2X12G12S ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_12X ||
			autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X))
		{
			/*do nothing for obc0 and obc1 recieve packets from other boards*/
		}
		else if(pend_cvm)
		{
			/* drop any packet if pend_cvm is set.*/
			cvm_oct_free_work(work);
			continue;
		}

		/* Immediately throw away all packets with receive errors */
		if (unlikely(work->word2.snoip.rcv_error)) {
			if (cvm_oct_check_rcv_error(work))
				continue;
		}

		/*
		 * We can only use the zero copy path if skbuffs are
		 * in the FPA pool and the packet fits in a single
		 * buffer.
		 */
		packet_len = work->word1.len;
		if (likely(skb_in_hw)) {
    		/*printk(KERN_NOTICE "DEBUG haodi: likely(skb_in_hw) = [%d] \n", skb_in_hw);*/			
            if(oct_length_debug)
            {
                /* This calculation was changed in case the skb header is using a
                    different address aliasing type than the buffer. It doesn't make
                    any differnece now, but the new one is more correct 2012-10-01 */
    			ptr_data = phys_to_virt(packet_ptr.s.addr);
        		if((((uint64_t)skb->data) & (~(0xffull))) != (uint64_t)((uint64_t)ptr_data & (~(0xffull))))
        		{
        				printk(KERN_NOTICE "ALIGN error: skb->data = [%p], work->packet_ptr.s.addr = [%p]. drp!\n", skb->data, ptr_data);
        				cvm_oct_free_work(work);
        				continue;
        		}
            }
			skb->data = phys_to_virt(packet_ptr.s.addr);			
			prefetch(skb->data);
			skb->len = packet_len;
			packets_to_replace = segments;
			if (likely(segments == 1)) {
				skb_set_tail_pointer(skb, skb->len);

                /* Add to check packet length of UDP packet[single buffer and not fragment], 2012-10-01 */
            	if((oct_length_debug)&&(0==fragment))
            	{
                    /* now to the mac header */
                	temp_data = skb->data;
                    vlan_type = (uint16_t)(*((unsigned short*)(temp_data + 12)));
                    if(vlan_type == 0x8100)     /* vlan tag 802.1Q */
                    {						
                        ip_type = (uint16_t)(*((unsigned short*)(temp_data + 16)));
                		
                        if(ip_type == 0x0800)   /* IP packets */
                        {
                            /* to the ip header */    
                        	ip = (struct iphdr*)((uint32_t *)(temp_data + 18));

                            if(ip->protocol == 0x11)  /* IP_PROTOCOL_UDP */
                        	{
            					/*printk(KERN_INFO "ip->protocol:0x%x \n",ip->protocol);*/
                        		uh = (struct udphdr*)(((uint32_t *)ip + ip->ihl));
                      			/*printk(KERN_INFO "skb uh->len[%d] .... skb->len[%d] \n", uh->len, skb->len);*/
                        		if (unlikely((uh->len > skb->len) || (uh->len < sizeof(struct udphdr)))) 
                        		{
                            		/*printPacketBuffer(skb->data, skb->len);*/
                        			printk(KERN_INFO "UDP error: uh->len[%d] > skb->len[%d] error, drop.\n", uh->len, skb->len);
                                   	if(use_skbuffs_in_hw_flag == 0) 
                                   	{
                                        dev_kfree_skb_irq(skb);
                                    }
                                    cvm_oct_free_work(work);
                                    continue;
                        		}
                        	}
                        }
                    }	
            	}				
			} else {
				struct sk_buff *current_skb = skb;
				struct sk_buff *next_skb = NULL;
				unsigned int segment_size;
				bool first_frag = true;

				skb_frag_list_init(skb);
				/* Multi-segment packet. */
				for (;;) {
					/*
					 * Octeon Errata PKI-100: The segment size is
					 * wrong. Until it is fixed, calculate the
					 * segment size based on the packet pool
					 * buffer size. When it is fixed, the
					 * following line should be replaced with this
					 * one: int segment_size =
					 * segment_ptr.s.size;
					 */
					segment_size = CVMX_FPA_PACKET_POOL_SIZE -
						(packet_ptr.s.addr - (((packet_ptr.s.addr >> 7) - packet_ptr.s.back) << 7));
					if (segment_size > packet_len)
						segment_size = packet_len;
					if (!first_frag)
						current_skb->len = segment_size;
					skb_set_tail_pointer(current_skb, segment_size);
					packet_len -= segment_size;
					segments--;
					if (segments == 0)
						break;
					packet_ptr = *(union cvmx_buf_ptr *)phys_to_virt(packet_ptr.s.addr - 8);
					next_skb = *cvm_oct_packet_to_skb(cvm_oct_get_buffer_ptr(packet_ptr));
					if (first_frag) {
						skb_frag_add_head(current_skb, next_skb);
					} else {
						current_skb->next = next_skb;
						next_skb->next = NULL;
					}
					current_skb = next_skb;
					first_frag = false;
					current_skb->data = phys_to_virt(packet_ptr.s.addr);
				}

			}
			packet_copied = false;
		} else {
			/*
			 * We have to copy the packet. First allocate
			 * an skbuff for it.
			 */
			skb = dev_alloc_skb(packet_len + CVMX_HELPER_FIRST_MBUFF_SKIP); //modify by fastfwd module
			if (!skb) {
				DEBUGPRINT("Port %d failed to allocate skbuff, packet dropped\n",
					   port);
				cvm_oct_free_work(work);
				continue;
			}
			
			skb_reserve(skb, CVMX_HELPER_FIRST_MBUFF_SKIP);  // add by fastfwd module
			/*
			 * Check if we've received a packet that was
			 * entirely stored in the work entry.
			 */
			if (unlikely(segments == 0)) {
				u8 *ptr = work->packet_data;

				if (likely(!work->word2.s.not_IP)) {
					/*
					 * The beginning of the packet
					 * moves for IP packets.
					 */
					if (work->word2.s.is_v6)
						ptr += 2;
					else
						ptr += 6;
				}
				memcpy(skb_put(skb, packet_len), ptr, packet_len);
				/* No packet buffers to free */
			} else {
				union cvmx_buf_ptr segment_ptr = work->packet_ptr;

				while (segments--) {
					union cvmx_buf_ptr  next_ptr;
					int segment_size;

					next_ptr = *(union cvmx_buf_ptr *)phys_to_virt(segment_ptr.s.addr - 8);

			/*
			 * Octeon Errata PKI-100: The segment size is
			 * wrong. Until it is fixed, calculate the
			 * segment size based on the packet pool
			 * buffer size. When it is fixed, the
			 * following line should be replaced with this
			 * one: int segment_size =
			 * segment_ptr.s.size;
			 */
					segment_size = CVMX_FPA_PACKET_POOL_SIZE -
						(segment_ptr.s.addr - (((segment_ptr.s.addr >> 7) - segment_ptr.s.back) << 7));
					/*
					 * Don't copy more than what
					 * is left in the packet.
					 */
					if (segment_size > packet_len)
						segment_size = packet_len;
					/* Copy the data into the packet */
					memcpy(skb_put(skb, segment_size),
					       phys_to_virt(segment_ptr.s.addr),
					       segment_size);
					packet_len -= segment_size;
					segment_ptr = next_ptr;
				}
			}
			packet_copied = true;
		}

		if (CVM_OCT_NAPI_HAS_CN68XX_SSO) {
			if (unlikely(cvm_oct_by_pkind[port] == NULL))
				priv = cvm_oct_dev_for_port(work->word2.s_cn68xx.port);
			else
				priv = cvm_oct_by_pkind[port];
		} else {
                        /* srio priv is based on mbox, not port */
			if (port == 40 || port == 41) {
				const cvmx_srio_rx_message_header_t *rx_header =
					(const cvmx_srio_rx_message_header_t *)skb->data;
				priv = cvm_oct_by_srio_mbox[(port - 40) >> 1][rx_header->word0.s.mbox];
			} else {
				priv = cvm_oct_by_pkind[port];
			}
		}

	if(rcv_rate_limit_hook && (! skb->data_len))
	{
		if(rcv_rate_limit_hook((unsigned int *)(skb->data), skb->len))
	   	{
	   		//struct ethhdr *eth = (struct ethhdr *)skb->data;
	   		if(!use_skbuffs_in_hw_flag){
				dev_kfree_skb(skb);
			}
	   		cvm_oct_free_work(work);
            continue;
	   	}
	}
	
    /* Add to check the ptr address, 2012-10-01 */
    if(oct_length_debug)
    {
        /*if((skb->head > skb->data)||(skb->tail < &(skb->data)))*/
        if(skb->head > skb->data)
    	{   	
            printk(KERN_INFO "SKB error: skb->head [%p]> skb->data [%p], drop!\n", skb->head,skb->data);
           	if(use_skbuffs_in_hw_flag == 0) 
           	{
                dev_kfree_skb_irq(skb);
            }
            cvm_oct_free_work(work);
            continue;
        }
    }
		if (likely(priv)) {
#ifdef CONFIG_RAPIDIO
			if (unlikely(priv->imode == CVMX_HELPER_INTERFACE_MODE_SRIO)) {
                                __skb_pull(skb, sizeof(cvmx_srio_rx_message_header_t));

				atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_packets);
				atomic64_add(skb->len, (atomic64_t *)&priv->netdev->stats.rx_bytes);
			}
#endif
			if (1) {
				//printk("receive data...netdev : %s\n", priv->netdev->name);
				//printPacketBuffer(skb->data, skb->len);
			}
			/* 
			 *is or isn't a rpa packet ?
			 *is or isn't a arp reply packet with RPA tag?
			 *change the device in skb
			 */
			rpa_pkt_flag = NOT_RPA_PKT;
			if (*((unsigned short*)(skb->data + rpa_type_position)) == RPA_COOKIE) {
				rpa_eth_header *rpa_head = (rpa_eth_header *)(skb->data + (rpa_type_position - 0xc));
				unsigned char slotNum = (rpa_head->d_s_slotNum >> 4) & 0x0f;

				if ((unsigned int)slotNum == base_local_slot_num) {
					rpa_pkt_flag = NORMAL_RPA_PKT;

					if (oct_rpa_debug) {
						printk("CVM_RPA before remove rpa tag.\n");
						printPacketBuffer(skb->data, skb->len);
					}

					/* need send to protocol stack? */
					if ((rpa_head->type & 0x0f) == 0x01) {
						int rpa_slotNum = (rpa_head->d_s_slotNum) & 0x0f;
						int rpa_netdevNum = rpa_head->snetdevNum;
						/* get right device which send the arp-reply to kernel */
						if (cvm_rpa_dev_index[rpa_slotNum][rpa_netdevNum].netdev) {
							skb->dev = cvm_rpa_dev_index[rpa_slotNum][rpa_netdevNum].netdev;
							skb_pull(skb, rpa_type_position + 0x6);
							rpa_pkt_flag = TO_PS_RPA_PKT;
							if (oct_rpa_debug) {
								printk("CVM_RPA after remove rpa tag(to protocol stack).\n");
								printPacketBuffer(skb->data, skb->len);
								pr_info("dev->name : %s\n", skb->dev->name);
							}
						}
						else {
							dev_kfree_skb_irq(skb);
							callback_result = CVM_OCT_DROP;
							goto skb_over;
						}
					}
				}
				else {
					dev_kfree_skb_irq(skb);
					callback_result = CVM_OCT_DROP;
					goto skb_over;
				}
			}
			
			/*
			 * Only accept packets for devices that are
			 * currently up.
			 */
			if (likely(priv->netdev->flags & IFF_UP)) {
				if (priv->rx_timestamp_sw) {
					struct skb_shared_hwtstamps *ts;
					ts = skb_hwtstamps(skb);
					ts->syststamp = ktime_get_real();
					ts->hwtstamp = ns_to_ktime(0);
				}
				if (priv->rx_timestamp_hw) {
					/* The first 8 bytes are the timestamp */
					u64 ns = *(u64 *)skb->data;
					struct skb_shared_hwtstamps *ts;
					ts = skb_hwtstamps(skb);
					ts->hwtstamp = ns_to_ktime(ns);
					ts->syststamp = cvm_oct_ptp_to_ktime(ns);
					__skb_pull(skb, 8);
				}
				/* if received skb by rpa(not a arp-reply), not parse the L2 head */
				if(rpa_pkt_flag == NOT_RPA_PKT) {
					skb->protocol = eth_type_trans(skb, priv->netdev);
					skb->dev = priv->netdev;
				}
				else if (rpa_pkt_flag == TO_PS_RPA_PKT) {
					skb->protocol = eth_type_trans(skb, skb->dev);
				}

				skb->cvm_cookie = CVM_COOKIE;
				if (unlikely(work->word2.s.not_IP || work->word2.s.IP_exc || work->word2.s.L4_error))
					skb->ip_summed = CHECKSUM_NONE;
				else
					skb->ip_summed = CHECKSUM_UNNECESSARY;

				/* Increment RX stats for virtual ports */
				if (port >= CVMX_PIP_NUM_INPUT_PORTS) {
					atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_packets);
					atomic64_add(skb->len, (atomic64_t *)&priv->netdev->stats.rx_bytes);
				}
				if (priv->intercept_cb  || rpa_pkt_flag) {
					/* rpa packet deliver the rpa module */
		            if (rpa_pkt_flag) {
		            	if (cvm_rpa_rx_hook) {
			            	callback_result = cvm_rpa_rx_hook(skb->dev, skb, rpa_pkt_flag);
		                }
						else {
							callback_result = CVM_OCT_DROP;
						}
		            }
					else {
						callback_result = priv->intercept_cb(priv->netdev, work, skb);
					}
					
					switch (callback_result) {
					case CVM_OCT_PASS:
						netif_receive_skb(skb);
						rx_count++;
						break;
					case CVM_OCT_DROP:
						dev_kfree_skb_any(skb);
						atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_dropped);
						break;
					case CVM_OCT_TAKE_OWNERSHIP_WORK:
						/*
						 * Interceptor took
						 * our work, but we
						 * need to free the
						 * skbuff
						 */
						//if (USE_SKBUFFS_IN_HW && likely(!packet_copied)) 
						if (use_skbuffs_in_hw_flag && likely(!packet_copied)) 
						{
							/*
							 * We can't free the skbuff since its data is
							 * the same as the work. In this case we don't
							 * do anything
							 */
						} else {
							dev_kfree_skb_any(skb);
						}
						break;
					case CVM_OCT_TAKE_OWNERSHIP_SKB:
						 rx_count++;
						/* Interceptor took our packet */
						break;
					}
				} else {
					netif_receive_skb(skb);
					callback_result = CVM_OCT_PASS;
					rx_count++;
				}
			} else {
				/* Drop any packet received for a device that isn't up */
				/*
				DEBUGPRINT("%s: Device not up, packet dropped\n",
					   dev->name);
				*/
				atomic64_add(1, (atomic64_t *)&priv->netdev->stats.rx_dropped);
				dev_kfree_skb_any(skb);
				callback_result = CVM_OCT_DROP;
			}
		} else {
			/*
			 * Drop any packet received for a device that
			 * doesn't exist.
			 */
			DEBUGPRINT("Port %d not controlled by Linux, packet dropped\n",
				   port);
			dev_kfree_skb_any(skb);
			callback_result = CVM_OCT_DROP;
		}
skb_over:
		/* We only need to free the work if the interceptor didn't
		   take over ownership of it */
		if (callback_result != CVM_OCT_TAKE_OWNERSHIP_WORK) {
			/*
			 * Check to see if the skbuff and work share
			 * the same packet buffer.
			 */
			//if (USE_SKBUFFS_IN_HW && likely(!packet_copied)) 
			if (use_skbuffs_in_hw_flag && likely(!packet_copied))
			{
				/*
				 * This buffer needs to be replaced,
				 * increment the number of buffers we
				 * need to free by one.
				 */
				cvmx_fau_atomic_add32(FAU_NUM_PACKET_BUFFERS_TO_FREE, packets_to_replace);
				cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, DONT_WRITEBACK(1));
			} else {
				cvm_oct_free_work(work);
			}
		}
	}
	/* Restore the original POW group mask */
	if (CVM_OCT_NAPI_HAS_CN68XX_SSO) {
		cvmx_write_csr(CVMX_SSO_PPX_GRP_MSK(coreid), old_group_mask);
		/* Read it back so it takes effect before ?? */
		cvmx_read_csr(CVMX_SSO_PPX_GRP_MSK(coreid));
	} else {
		cvmx_write_csr(CVMX_POW_PP_GRP_MSKX(coreid), old_group_mask);
	}
	if (USE_ASYNC_IOBDMA) {
		/* Restore the scratch area */
		cvmx_scratch_write64(CVMX_SCR_SCRATCH, old_scratch);
	}
	cvm_oct_rx_refill_pool(0);

napi_over:
	if (rx_count < budget && napi != NULL) {
		/* No more work */
		napi_complete(napi);
		cvm_oct_no_more_work(napi);
	}
	return rx_count;
}
