
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/dst.h>
#include <net/checksum.h>
#include <net/xfrm.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/autelan_product.h>

#include "rpa.h"
#include "pfm.h"





extern product_info_t autelan_product_info;
static struct sock *netlink_sock;
static int exit_flag = 0;
static int log_enable = 0;
/*zhaocg add fot show pfm table*/
static int pfm_pid = 0;
static int pfm_pro_id = 0;
int pfm_record_num_limit = 0;
netlink_transport_info pfm_table_info = {0};

ifindex_dest_ip_tree* forwarding_table[MAX_PROTOCOL][MAX_PORT];
/*pfm_entry*				pro_table[0xff]; */
extern int (*cvm_pfm_rx_hook)(struct sk_buff*);
extern rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
extern int (*cvm_rpa_tx_hook)(struct sk_buff *skb,struct net_device *netdev,unsigned int portNum,int flag);
extern int cvm_rpa_local_netdevnum_get(int ifindex);
int list_all_elem(struct date *table,struct entry_info* entry,int is_root);
int send_value_to_entry(struct entry_info* entry,int date);
int show_entry_info(struct entry_info* entry);
int get_slot_from_table(struct date** temp_date,struct entry_info* temp_entry);
int init_entry_info(entry_info** temp);
int insert_entry(entry_info* entry);
int init_proto_table(void);

/*int init_proto_table();*/
rwlock_t pfm_table_rwlock=RW_LOCK_UNLOCKED;


#define HAVE_RPA (!(autelan_product_info.board_type == AUTELAN_BOARD_AX71_2X12G12S \
				|| autelan_product_info.board_type == AUTELAN_BOARD_AX81_2X12G12S \
				|| autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_12X))


static u_char maskbit[] = {0x00, 0x80, 0xc0, 0xe0, 0xf0,
			         0xf8, 0xfc, 0xfe, 0xff};
char* maskbitstr[] = {"0x00", "0x80", "0xc0", "0xe0", "0xf0",
			         "0xf8", "0xfc", "0xfe", "0xff"};




#if NEW_PFM_STRUCT

struct date* proto_table[0x1fe];
long mem_use = 0;

#endif


/*int forwarding_table_init();*/
/*void test_forwarding_table_init_state();*/
int del_dest_ip_in_ifindex_tree(ifindex_dest_ip_tree** root,\
								int ifindex,\
								int	opt,\
								int opt_para,\
								_u32 dest_ip);
int copy_sk_buff_to_rpa(struct sk_buff* skb,int slot);
int add_mask_to_proto_table(struct date** temp_date,struct entry_info* temp_entry);
int get_slot_from_mask_table(struct date** temp_date,struct entry_info* temp_entry);


int print_log(char* format,... )
{
	if(unlikely(log_enable == 1))
	{
		
		va_list list;
		va_start(list,format);
		vprintk(format,list);
		va_end(list);
	}
	return 0;
}
int print_table_log(char* format,... )
{

	va_list list;
	va_start(list,format);
	vprintk(format,list);
	va_end(list);
	return 0;
}

int print_ip(int mask)
{
	
	u_char pnt[4];

	memcpy(pnt,&mask,4);
	

	print_log("%2x.%2x.%2x.%02x\n",pnt[0],pnt[1],pnt[2],pnt[3]);
	#if 0
	for(bit=0;bit<1;bit++)
	{
		for(offset=0;offset<=8;offset++)
		{
			//printf("pnt= %d\n",pnt[0]);
		//	if(maskbit[offset] == pnt[bit])
			{
				printf(" %s",maskbitstr[offset]);
		//		break;
			}
		}
		printf(":");
	}
	#endif
	return 1;
		
	}
int print_ip_o(int mask)
{
	
	u_char pnt[4];

	memcpy(pnt,&mask,4);
	

	print_log(KERN_INFO"%d.%d.%d.%d\n",pnt[0],pnt[1],pnt[2],pnt[3]);
	#if 0
	for(bit=0;bit<1;bit++)
	{
		for(offset=0;offset<=8;offset++)
		{
			//printf("pnt= %d\n",pnt[0]);
		//	if(maskbit[offset] == pnt[bit])
			{
				printf(" %s",maskbitstr[offset]);
		//		break;
			}
		}
		printf(":");
	}
	#endif
	return 1;
		
	}
int print_ip_o_table(int mask)
{
	
	u_char pnt[4];

	memcpy(pnt,&mask,4);
	

	print_table_log(KERN_INFO"%d.%d.%d.%d\n",pnt[0],pnt[1],pnt[2],pnt[3]);
	return 1;
		
	}


void
apply_mask_ipv4 (int* ip,int mask)
{
  u_char *pnt;
  int index;
  int offset;
  int prefix;

  index = mask / 8;

  if (index < 4)
    {
		pnt = (u_char *) ip;
      offset =mask % 8;

      pnt[index] &= maskbit[offset];
      index++;

      while (index < 4)
	pnt[index++] = 0;
    }
}


int
prefix_same (int ipadd1,int ipadd2,int mask)
{
	apply_mask_ipv4(&ipadd1,mask);
	apply_mask_ipv4(&ipadd2,mask);
	print_ip_o(ipadd1);
	print_ip_o(ipadd2);
	if(ipadd1 == ipadd2)
		return 1;
	return 0;
}
/*zhaocg add for show pfm table*/
static int
send_netlink_msg(int pid, netlink_transport_info *msg)
{
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlhdr = NULL;
	sk_buff_data_t old_tail;
	int ret = 0;
	int size = 0;

	size = NLMSG_SPACE(sizeof(*msg));
	skb = alloc_skb(size, GFP_ATOMIC);
	if(!skb) 
	{
		print_log(KERN_ERR,
					"alloc skb failed!\n");
		return -1;
	}
	old_tail = skb->tail;

	nlhdr = NLMSG_PUT(skb, 0, 0, 1, size - sizeof(*nlhdr));
	memcpy(NLMSG_DATA(nlhdr), msg, sizeof(*msg));
	nlhdr->nlmsg_len = skb->tail - old_tail;

	NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).creds.pid = pid;
	NETLINK_CB(skb).dst_group = 0;

	ret = netlink_unicast(netlink_sock, skb, pid, MSG_DONTWAIT);
	return ret;

nlmsg_failure:
	if(skb)
		kfree_skb(skb);
	return -1;
}


#if 1
int copy_sk_buff_to_rpa(struct sk_buff* skb,int _slot)
{
	
	struct sk_buff* skb_cp = NULL;
	int ret;
	int portnum;
	int slot;
	slot = _slot -1;
	print_log(KERN_INFO"PFM_KERN:copy_sk_buff_to_rpa now\n");
	#if 0
	if (rpa_arp_send_dev[slot].netdev)
	{
		char* temp = NULL;
		skb_cp = skb_copy(skb, GFP_ATOMIC);/*copy the skb */
	
		if (!skb_cp)
		{
			printk(KERN_ERR"PFM_KERN:skb_copy error!\n");
			return -1;
			
		}

		
		portnum = cvm_rpa_local_netdevnum_get(skb_cp->dev->ifindex);
		skb_cp->dev = rpa_arp_send_dev[slot].netdev;

		temp = skb_push(skb_cp,sizeof(struct ethhdr));
		memcpy(temp,skb_cp -> mac.raw,sizeof(struct ethhdr));

		if(cvm_rpa_tx_hook)
		{
			ret = cvm_rpa_tx_hook(skb_cp,skb_cp->dev,portnum,1);
		}else{

			printk(KERN_ERR"PFM====cvm_rpa_tx_hook is NULL\n");
			
			ret = -1;
		}
		if (skb_cp)
		{
			//kfree_skb(skb_cp);
		}
		
	/*	if (skb)
		{
			kfree_skb(skb);
		} */
		if (ret < 0)
		{
			printk(KERN_ERR"PFM_KERN:send rpa error!\n");
			return -1;
		}else{
			print_log(KERN_INFO"PFM_KERN:send rpa ok\n");
		}
		return 0;
	}else{
		//printk(KERN_ERR"PFM_KERN:rpa_arp_send_dev[%d].netdev is NULL\n",slot);
		
		/*if (skb)
		{
			kfree_skb(skb);
		} */
		return -1;
	}
#else
	portnum = cvm_rpa_local_netdevnum_get(skb->dev->ifindex);
	if (portnum < 0)
	{
		return -1;
	}

	if (cvm_rpa_tx_hook == NULL)
	{
		return -1;
	}
	
	if (rpa_arp_send_dev[slot].netdev)
	{
		char* temp = NULL;
		skb_cp = skb_copy(skb, GFP_ATOMIC);/*copy the skb */

		if (!skb_cp)
		{
			printk(KERN_ERR"PMF_KERN: skb_copy error!\n");
			return -1; /* add by dingkang coverity cid:11019*/
		}

		skb_cp->dev = rpa_arp_send_dev[slot].netdev;

		
		temp = skb_push(skb_cp,sizeof(struct ethhdr));
		memcpy(temp,(eth_hdr(skb)),sizeof(struct ethhdr));
		ret = cvm_rpa_tx_hook(skb_cp, skb_cp->dev, portnum, RPA_ENTER_STACK_ENABLE);
		if (ret != 0)
		{
	/*		kfree_skb(skb_cp);*/
			print_log(KERN_ERR"PMF_KERN: send rpa error!\n");
			return -1;
		}
		print_log(KERN_INFO"PFM_KERN:send rpa OK\n");
		return 1;
	}
	else
	{
		
		print_log(KERN_ERR"PMF_KERN:rpa_arp_send_dev [%d].netdev is NULL!\n",slot);
		return -1;
	}
#endif

}

#endif

int catch_packet(struct sk_buff* packet)
{
	struct ethhdr* machdr;
	entry_info entry;
	int ret;
	int forward_slot;

	
	if(unlikely(NULL == packet))
	{
		
		printk(KERN_ERR"PFM_KERN:packet was NULL\n");
		return 1;
	}

	if (packet->mac_header){
		machdr = eth_hdr(packet);		
	}else{        /* add by dingkang because coverity cid:14326*/
		return 0;
	}
/*	machdr = (struct ethhdr*)((packet -> data)+ packet ->mac_header);  */
	memset(&entry,0,sizeof(entry_info));
	
	/*print_log(KERN_INFO"PFM_KERN:catch a packet\n");*/
	if( likely(ETH_P_IP == ntohs(machdr -> h_proto))) /*add by dingkang because coverity cid:14326*/
	{
		struct iphdr *iphead = NULL;
		int forward_slot = -1;
		/*print_log(KERN_INFO"PFM_KERN:ip packet\n");*/
		iphead = (struct iphdr*)packet -> data;

		if((iphead -> protocol) == 6)
		{
			struct tcphdr *tcphead = NULL;
			/*print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");*/
			print_log(KERN_INFO"\n\n\nrecv ipv4 packet\n");

			tcphead = (struct tcphdr*)((packet -> data) + sizeof(struct iphdr));
		/*	print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
			
			print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
			print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);*/

		/*	forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][(tcphead -> dest)],
																packet -> dev -> ifindex,\iphead->daddr); */  
			set_count(&entry,1);
			if(packet -> dev == NULL)
				return 1;
			else
			set_ifindex(&entry,packet -> dev -> ifindex);
			set_proto(&entry,6);
			set_d_addr(&entry,iphead->daddr);
			set_s_addr(&entry,iphead->saddr);
			set_d_port(&entry,tcphead->dest);	
			set_s_port(&entry,tcphead->source);
			show_entry_info(&entry);
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
			read_unlock(&pfm_table_rwlock);

			if(0 != forward_slot)
			{
	/*			print_log(KERN_INFO"PFM====no forward needed\n");*/
				return 1;
			}else{
			
			forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");
				print_log(KERN_INFO"PFM====forward to slot:%d\n",forward_slot);
				print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);	
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				
				if(packet -> dev == NULL)
					return 1;
				else
				print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);
				if(unlikely(-1 == copy_sk_buff_to_rpa(packet,forward_slot)))
				{
					print_log(KERN_ERR"PFM====send rpa error!\n");
					return 1;
				}
				return 0;
			}

			return 1;
		}
		if((iphead -> protocol) == 17)
		{
			struct udphdr *udphead = NULL;

	/*		print_log(KERN_INFO"PFM====protocol is %s\n","UDP");*/

			udphead = (struct udphdr*)((packet -> data) + sizeof(struct iphdr));
	/*		print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);
			print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr);
			print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);*/


			/*
			forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[1][(udphead -> dest)],\
																packet -> dev -> ifindex,\
																iphead -> daddr);
																					*/
			set_count(&entry,1);
			set_proto(&entry,17);
			
			if(packet -> dev == NULL)
				return 1;
			else
			set_ifindex(&entry,packet -> dev -> ifindex);
			set_d_addr(&entry,iphead->daddr);
			set_s_addr(&entry,iphead->saddr);
			set_d_port(&entry,udphead->dest);
			set_s_port(&entry,udphead->source);
			
			show_entry_info(&entry);
			
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);	
			read_unlock(&pfm_table_rwlock);
			if(0 != forward_slot)
			{
		/*		print_log(KERN_INFO"PFM====no forward need\n");*/
				return 1;
			}else{
				forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","UDP");
				print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
				print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);	
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				
				if(packet -> dev == NULL)
					return 1;
				else
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);

				ret = copy_sk_buff_to_rpa(packet,forward_slot);
			/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
				if(unlikely(-1 == ret))
				{
					print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
					return 1;
				}
				return 0;
			}
			return 1;
		}
/*another proto*/
		set_count(&entry,1);
		set_proto(&entry,iphead -> protocol);
		set_d_addr(&entry,iphead->daddr);
		set_s_addr(&entry,iphead->saddr);
		set_ifindex(&entry,packet->dev->ifindex);
		
		show_entry_info(&entry);
		read_lock(&pfm_table_rwlock);
		forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);	
		read_unlock(&pfm_table_rwlock);
		if(0 != forward_slot)
		{
	/*		print_log(KERN_INFO"PFM====no forward need\n");*/
			return 1;
		}else{
		
		forward_slot = get_flag(&entry);
			print_log(KERN_INFO"PFM_KERN:==========================\n");
			print_log(KERN_INFO"PFM_KERN:protocol is %s\n","other");
			print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
			print_log(KERN_INFO"PFM_KERN:port is %x\n",iphead -> saddr);	
			print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 

/* delete by dingkang because coverity cid:13863 #1
			if(packet -> dev == NULL)
				return 1;
			else
			print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);
*/
			ret = copy_sk_buff_to_rpa(packet,forward_slot);
		/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
			if(unlikely(-1 == ret))
			{
				print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
				return 1 ;
			}
			return 0;
		}

		
		
			
	}

	
	if( likely(ETH_P_IPV6 == ntohs(machdr -> h_proto)))
	{
		struct ipv6hdr *ip6head = NULL;
		int forward_slot = -1;
		/*print_log(KERN_INFO"PFM_KERN:ip packet\n");*/
		ip6head = (struct ipv6hdr*)packet -> data;
		print_log(KERN_INFO"recv ipv6 packet\n");

		if((ip6head -> nexthdr) == 6)
		{
			struct tcphdr *tcphead = NULL;
			print_log(KERN_INFO"ipv6 tcp packet\n");
			/*print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");*/

			tcphead = (struct tcphdr*)((packet -> data) + sizeof(struct ipv6hdr));
			print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
			
			print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",tcphead -> source); 
			
			if(packet -> dev == NULL)
				return 1;
			else
			print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);

		/*	forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][(tcphead -> dest)],
																packet -> dev -> ifindex,\iphead->daddr); */  
			set_count(&entry,1);
			set_ifindex(&entry,packet -> dev -> ifindex);
			set_proto(&entry,(6+0xff));
			//not support ipv6 addr
			set_d_addr(&entry,0);
			set_s_addr(&entry,0);
			set_d_port(&entry,tcphead->dest);	
			set_s_port(&entry,tcphead->source);
			show_entry_info(&entry);
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
			read_unlock(&pfm_table_rwlock);

			if(0 != forward_slot)
			{
	/*			print_log(KERN_INFO"PFM====no forward needed\n");*/
				return 1;
			}else{
			
			forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");
				print_log(KERN_INFO"PFM====forward to slot:%d\n",forward_slot);
				print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
				print_log(KERN_INFO"PFM_KERN:Sport is %d\n",tcphead ->source);
				/*print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); */
				
				if(packet -> dev == NULL)
					return 1;
				else
				print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);
				if(unlikely(-1 == copy_sk_buff_to_rpa(packet,forward_slot)))
				{
					print_log(KERN_ERR"PFM====send rpa error!\n");
					return 1;
				}
				return 0;
			}

			return 1;
		}
		if((ip6head -> nexthdr) == 17)
		{
			struct udphdr *udphead = NULL;

	/*		print_log(KERN_INFO"PFM====protocol is %s\n","UDP");*/
	print_log(KERN_INFO"ipv6 udp packet\n");

			udphead = (struct udphdr*)((packet -> data) + sizeof(struct ipv6hdr));
			print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);
			print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",udphead -> source);
			
			if(packet -> dev == NULL)
				return 1;
			else
			print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);


			/*
			forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[1][(udphead -> dest)],\
																packet -> dev -> ifindex,\
																iphead -> daddr);
																					*/
			set_count(&entry,1);
			set_proto(&entry,(17+0xff));
			
			if(packet -> dev == NULL)
				return 1;
			else
			set_ifindex(&entry,packet -> dev -> ifindex);
			set_d_addr(&entry,0);
			set_s_addr(&entry,0);
			set_d_port(&entry,udphead->dest);
			set_s_port(&entry,udphead->source);
			show_entry_info(&entry);
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
			read_unlock(&pfm_table_rwlock);
			if(0 != forward_slot)
			{
		/*		print_log(KERN_INFO"PFM====no forward need\n");*/
				return 1;
			}else{
			
			forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","UDP");
				print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
				print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);	
				/*print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",ip6head -> daddr); */				
				if(packet -> dev == NULL)
					return 1;
				else
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);

				ret = copy_sk_buff_to_rpa(packet,forward_slot);
			/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
				if(unlikely(-1 == ret))
				{
					print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
					return 1;
				}
				return 0;
			}
			return 1;
		}
/*another proto*/
		set_count(&entry,1);
		set_proto(&entry,((ip6head -> nexthdr)+0xff));
		set_d_addr(&entry,0);
		set_s_addr(&entry,0);
		set_ifindex(&entry,packet->dev->ifindex);
		
		show_entry_info(&entry);
		read_lock(&pfm_table_rwlock);
		forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);	
		read_unlock(&pfm_table_rwlock);
		
		if(0 != forward_slot)
		
		{
	/*		print_log(KERN_INFO"PFM====no forward need\n");*/
			return 1;
		}else{
		
		forward_slot = get_flag(&entry);
			print_log(KERN_INFO"PFM_KERN:==========================\n");
			print_log(KERN_INFO"PFM_KERN:protocol is %s\n",ip6head -> nexthdr);
			print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
			/*print_log(KERN_INFO"PFM_KERN:port is %x\n",iphead -> saddr);	*/
			/*print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); */
			
/*	delete by dingkang because coverity cid:13863 #2
			if(packet -> dev == NULL)
				return 1;
			else
			print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);

			
*/
			ret = copy_sk_buff_to_rpa(packet,forward_slot);
		/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
			if(unlikely(-1 == ret))
			{
				print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
				return 1;
			}
			return 0;
		}

		
		
			
	}
	if( likely(ETH_P_8021Q == ntohs(machdr -> h_proto)))
	{
	struct vlan_ethhdr *vlan_hd;
	int ifindex = 0;
	int vid = 0;
	vlan_hd = vlan_eth_hdr(packet);
	print_log(KERN_INFO"\nrecv vlan packet %x\n",vlan_hd -> h_vlan_proto);
	if(vlan_hd -> h_vlan_proto == ETH_P_8021Q)
	{
		/*
		print_log(KERN_INFO"PFM_KERN:proto is %x\n",vlan_hd -> h_vlan_proto);
		print_log(KERN_INFO"PFM_KERN:type is %ud\n",vlan_hd -> h_vlan_encapsulated_proto);
		*/
		vid = vlan_hd -> h_vlan_TCI;
		/*
		print_log(KERN_INFO"PFM_KERN: TCI is %d\n",vid);
		*/
		vid &= ~(0xf000);
		/*
		print_log(KERN_INFO"PFM_KERN: TCI is %x\n",vid);

		print_log(KERN_INFO"PFM_KERN:vid is %d\n",vid);
		*/
		if(packet -> dev == NULL)
		{
			print_log(KERN_ERR"dev is NULL\n");
			return 0;
		}
		else{
		ifindex = (packet -> dev -> ifindex);
		ifindex += vid << 8;}
/*
		print_log(KERN_INFO"PFM_KERN: zhao ifindex is %d\n",ifindex);
*/
		if( likely(ETH_P_IP == ntohs(vlan_hd -> h_vlan_encapsulated_proto)))
		{
			struct iphdr *iphead = NULL;
/*			
			struct iphdr *iphead_vlan = NULL;
			print_log(KERN_INFO"PFM_KERN:ip packet\n");
*/
			iphead = (struct iphdr*)((packet -> data)+4);
/*			
			print_log(KERN_INFO"PFM_KERN: protocol is %d\n",iphead -> protocol);
*/			


			if((iphead -> protocol) == 6)
			{
				struct tcphdr *tcphead = NULL;
				int forward_slot = -1;
/*				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");*/

				tcphead = (struct tcphdr*)(((packet -> data) + sizeof(struct iphdr)) + 4);/*
				print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
				
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",ifindex);

				forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][(tcphead -> dest)],\
																	ifindex,\
																	iphead -> daddr);
*/
				set_count(&entry,1);
				set_proto(&entry,6);
				set_ifindex(&entry,ifindex);
				set_d_addr(&entry,iphead->daddr);
				set_d_port(&entry,tcphead->dest);
				set_s_port(&entry,tcphead->source);
				set_s_addr(&entry,iphead->saddr);
				show_entry_info(&entry);
				read_lock(&pfm_table_rwlock);
				forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
				read_unlock(&pfm_table_rwlock);

				
				if(0 != forward_slot)
				
				{
/*					print_log(KERN_INFO"PFM_KERN:no forward needed\n");*/
					return 1;
				}else{
				
				forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				
				print_log(KERN_INFO"PFM_KERN:proto is %x\n",vlan_hd -> h_vlan_proto);
				print_log(KERN_INFO"PFM_KERN:type is %ud\n",vlan_hd -> h_vlan_encapsulated_proto);
				print_log(KERN_INFO"PFM_KERN:TCI is %x\n",vid);
				print_log(KERN_INFO"PFM_KERN:vid is %d\n",vid);
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");
				print_log(KERN_INFO"PFM_KERN:ip packet\n");
				
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",ifindex);
				print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
					print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
					if(unlikely(-1 == copy_sk_buff_to_rpa(packet,forward_slot)))
					{
						print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
						return 1;
					}
					return 0;
				}

				return 1;
			}
			if((iphead -> protocol) == 17)
			{
				struct udphdr *udphead = NULL;
				int forward_slot;
				int ret;
//				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","UDP");

				udphead = (struct udphdr*)((packet -> data) + sizeof(struct iphdr) + 4);
//				print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);
//				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr);
//				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",ifindex);


/*				
				forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[1][(udphead -> dest)],\
																	ifindex,\
																	iphead -> daddr);
				*/

				set_count(&entry,1);
				set_proto(&entry,17);
				set_ifindex(&entry,ifindex);
				set_d_addr(&entry,iphead->daddr);
				set_d_port(&entry,udphead->dest);
				set_s_addr(&entry,iphead->saddr);
				set_s_port(&entry,udphead->source);
				show_entry_info(&entry);
				read_lock(&pfm_table_rwlock);
				forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
				read_unlock(&pfm_table_rwlock);

				if(0 != forward_slot)
				{
//					print_log(KERN_INFO"PFM====no forward need\n");
					return 1;
				}else{
				
				forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				
				print_log(KERN_INFO"PFM_KERN:proto is %x\n",vlan_hd -> h_vlan_proto);
				print_log(KERN_INFO"PFM_KERN:type is %ud\n",vlan_hd -> h_vlan_encapsulated_proto);
				print_log(KERN_INFO"PFM_KERN:TCI is %x\n",vid);
				print_log(KERN_INFO"PFM_KERN:vid is %d\n",vid);
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","UDP");
				
				print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr);
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",ifindex);
					print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
					print_log(KERN_INFO"PFM====forward to slot:%d\n",forward_slot);

					ret = copy_sk_buff_to_rpa(packet,forward_slot);
					
					print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);
					if(unlikely(-1 == ret))
					{
						print_log(KERN_ERR"PFM====send rpa error!\n");
						return 1;
					}
					return 0;
				}
				return 1;
			}
			/*another proto*/
			set_count(&entry,1);
			set_proto(&entry,iphead -> protocol);
			set_d_addr(&entry,iphead->daddr);
			set_s_addr(&entry,iphead->saddr);


			set_ifindex(&entry,ifindex);

			
			show_entry_info(&entry);
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry); 	
			read_unlock(&pfm_table_rwlock);
			if(0 != forward_slot)
			{
		/*		print_log(KERN_INFO"PFM====no forward need\n");*/
				return 1;
			}else{
			
			forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n","other");
				print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
				print_log(KERN_INFO"PFM_KERN:port is %x\n",iphead -> saddr);	
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				
				if(packet -> dev == NULL)
					{
						print_log(KERN_ERR"dev is NULL\n");
					return 1;
					}
				else
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);
	
				ret = copy_sk_buff_to_rpa(packet,forward_slot);
			/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
				if(unlikely(-1 == ret))
				{
					print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
					return 1;
				}
				return 0;

			
				
		}
	}
		if( likely(ETH_P_IPV6 == ntohs(vlan_hd -> h_vlan_encapsulated_proto)))
		{
			struct ipv6hdr *ip6head = NULL;
			int forward_slot = -1;
			/*print_log(KERN_INFO"PFM_KERN:ip packet\n");*/
			ip6head = ((struct ipv6hdr*)(packet -> data + 4));
			print_log(KERN_INFO"\n\n\nrecv ipv6 packet with vlan and nexthdr is :%d\n",ip6head->nexthdr);

			if((ip6head -> nexthdr) == 6)
			{
				struct tcphdr *tcphead = NULL;
				/*print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");*/

				tcphead = (struct tcphdr*)((packet -> data) + sizeof(struct ipv6hdr) +4);
			/*	print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
				
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
				print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);*/

			/*	forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][(tcphead -> dest)],
																	packet -> dev -> ifindex,\iphead->daddr); */  
				set_count(&entry,1);
			
				if(packet -> dev == NULL)
					{
						print_log(KERN_ERR"dev is NULL\n");
					return 1;
					}
				else
				set_ifindex(&entry,ifindex);
				set_proto(&entry,(6+0xff));
				/*not support ipv6 addr*/
				set_d_addr(&entry,0);
				set_s_addr(&entry,0);
				set_d_port(&entry,tcphead->dest);	
				set_s_port(&entry,tcphead->source);
				show_entry_info(&entry);
				read_lock(&pfm_table_rwlock);
				forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
				read_unlock(&pfm_table_rwlock);

				if(0 != forward_slot)
				{
		/*			print_log(KERN_INFO"PFM====no forward needed\n");*/
					return 1;
				}else{
				
				forward_slot = get_flag(&entry);
					print_log(KERN_INFO"PFM_KERN:protocol is %s\n","TCP");
					print_log(KERN_INFO"PFM====forward to slot:%d\n",forward_slot);
					print_log(KERN_INFO"PFM_KERN:port is %d\n",tcphead -> dest);
					print_log(KERN_INFO"PFM_KERN:Sport is %d\n",tcphead ->source);
					//print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); 
					
					if(packet -> dev == NULL)
						{
							print_log(KERN_ERR"dev is NULL\n");
						return 1;
						}
					else
					print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);
					if(unlikely(-1 == copy_sk_buff_to_rpa(packet,forward_slot)))
					{
						print_log(KERN_ERR"PFM====send rpa error!\n");
						return 1;
					}
					return 0;
				}
				

				return 1;
			}

			if((ip6head -> nexthdr) == 17)
			{
				struct udphdr *udphead = NULL;

		/*		print_log(KERN_INFO"PFM====protocol is %s\n","UDP");*/

				udphead = (struct udphdr*)((packet -> data) + sizeof(struct ipv6hdr) + 4);
		/*		print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);
				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr);
				print_log(KERN_INFO"PFM====ifindex is %d\n",packet -> dev -> ifindex);*/


				/*
				forward_slot = look_for_forward_slot_at_ifindex_tree(forwarding_table[1][(udphead -> dest)],\
																	packet -> dev -> ifindex,\
																	iphead -> daddr);
																						*/
				set_count(&entry,1);
				set_proto(&entry,(17+0xff));
				
				if(packet -> dev == NULL)
					{
						print_log(KERN_ERR"dev is NULL\n");
					return 1;
					}
				else
				set_ifindex(&entry,ifindex);
				set_d_addr(&entry,0);
				set_s_addr(&entry,0);
				set_d_port(&entry,udphead->dest);
				set_s_port(&entry,udphead->source);
				show_entry_info(&entry);
				read_lock(&pfm_table_rwlock);
				forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);
				read_unlock(&pfm_table_rwlock);
				if(0 != forward_slot)
				{
					print_log(KERN_INFO"PFM====no forward need\n");
					return 1;
				}else{
				
				forward_slot = get_flag(&entry);
					print_log(KERN_INFO"PFM_KERN:==========================\n");
					print_log(KERN_INFO"PFM_KERN:protocol is %s\n","UDP");
					print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
					print_log(KERN_INFO"PFM_KERN:port is %d\n",udphead -> dest);	
/*				print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); */
					if(packet -> dev == NULL)
						{
							print_log(KERN_ERR"dev is NULL\n");
						return 1;
						}
					else
					print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);

					ret = copy_sk_buff_to_rpa(packet,forward_slot);
				/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
					if(unlikely(-1 == ret))
					{
						print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
						return 1;
					}
					return 0;
				}
				return 1;
			}
	/*another proto*/

			set_count(&entry,1);
			set_proto(&entry,((ip6head -> nexthdr)+0xff));
			set_d_addr(&entry,0);
			set_s_addr(&entry,0);
			
			if(packet -> dev == NULL)
				{
					print_log(KERN_ERR"dev is NULL\n");
				return 1;
				}
			else
			set_ifindex(&entry,ifindex);
			
			show_entry_info(&entry);
			read_lock(&pfm_table_rwlock);
			forward_slot = get_slot_from_table(&(proto_table[(get_proto(&entry))]),&entry);	
			read_unlock(&pfm_table_rwlock);
			if(0 != forward_slot)
			{
		/*		print_log(KERN_INFO"PFM====no forward need\n");*/
				return 1;
			}else{
			
			forward_slot = get_flag(&entry);
				print_log(KERN_INFO"PFM_KERN:==========================\n");
				print_log(KERN_INFO"PFM_KERN:protocol is %s\n",ip6head -> nexthdr);
				print_log(KERN_INFO"PFM_KERN:forward to slot:%d\n",forward_slot);
				/*print_log(KERN_INFO"PFM_KERN:port is %x\n",iphead -> saddr);	*/
				/*print_log(KERN_INFO"PFM_KERN:dest ip is %x\n",iphead -> daddr); */
				
				if(packet -> dev == NULL)
					{
						print_log(KERN_ERR"dev is NULL\n");
					return 1;
					}
				else
				print_log(KERN_INFO"PFM_KERN:ifindex is %d\n",packet -> dev -> ifindex);

				ret = copy_sk_buff_to_rpa(packet,forward_slot);
			/*	print_log(KERN_INFO"PFM====copy_sk_buff_to_rpa ret is %d",ret);*/
				if(unlikely(-1 == ret))
				{
					print_log(KERN_ERR"PFM_KERN:send rpa error!\n");
					return 1;
				}
				return 0;
			
			}
			

	}
	
	}
	
	}
#if 0 
	if((int)(packet -> nh.iph -> protocol) == 6)
	{
		print_log(KERN_INFO"PFM_KERN:catch a tcp packet,port is %d",(int)(packet -> h.th -> dest));
		if(forwarding_table[0][(int)(packet -> h.th -> dest)] == NULL)
		{
	//		kfree_skbmem(packet);
			return 0; 
		}
		return 1;
	}
	if((int)(packet -> nh.iph -> protocol) == 17)
	{
		print_log(KERN_INFO"PFM_KERN:chtch a udp packet,port is %d",(int)(packet -> h.uh -> dest));
		if(forwarding_table[1][(int)(packet -> h.uh -> dest)] == NULL)
		{
			
	//		kfree_skbmem(packet);
			return 0;
		}
		return 1;
	}
	#endif
	return 1;
}


/*************************************************************
*
*
*
*                      data structure func
*
*
*
**************************************************************/

/************************show func***************************/

int list_all_ifindex_dest_ip_tree(ifindex_dest_ip_tree* root)
{
	if(NULL != root)
	{
		print_log(KERN_INFO"ifindex is %d\n",root -> ifindex);
		list_all_ifindex_dest_ip_tree(GET_LIFT_CHILD_IFINDEX(root));
		list_all_ifindex_dest_ip_tree(GET_RIGHT_CHILD_IFINDEX(root));
	}else{

		return 0 ;

	}
	return 0 ;
}




int list_all_dest_ip_tree(dest_ip_tree* root)
{
	if(NULL != root)
	{
		print_log(KERN_INFO"ip addr is %d\n",root -> dest_ip);
		print_log(KERN_INFO"slot_id is %d\n",root -> forward_slot);
		print_log(KERN_INFO"\n");
		list_all_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root));
		list_all_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root));
	}else{

		return 0 ;

	}
	return 0 ;
}


	



/************************test func*********************************/

/*
void test_forwarding_table_init_state()
{
	int i,j;
	for(i=0;i<MAX_PROTOCOL;i++)
	{
		for(j=0;j<MAX_PORT;j++)
		{
			if(NULL  != forwarding_table[i][j])
				print_log(KERN_INFO"forwarding_table[%d][%d] is not NULL\n",i,j);
		}
	}
}

*/

/******************************init func**************************/



int init_tree_elem(tree_elem** root,tree_elem* parent)
{
	if(unlikely(NULL != (*root)))
	{
		printk(KERN_ERR"init_tree_elem ,root is not NULL\n");
		return -1;
	}

	(*root) = (tree_elem*)kmalloc(TREE_ELEM_LEN,GFP_KERNEL);

	if(unlikely(NULL == (*root)))
	{
		printk(KERN_ERR"init_tree_elem,malloc error\n");
		return -1;
	}
	memset((*root),0,TREE_ELEM_LEN);

	
	(*root) -> lc 		= NULL;
	(*root) -> rc		= NULL;
	(*root) -> parent	= parent;
	(*root) -> deep		= 0;

	return 0;
}
	

int init_dest_ip_tree(dest_ip_tree** root,\
						_u32		 ip_addr,\
						int 		 slot_id,\
					  dest_ip_tree*	 parent)
{
	if(unlikely(NULL != (*root)))
	{
		printk(KERN_ERR"init_dest_ip_tree is not NULL\n");
		return -1;
	}
	
	(*root) = (dest_ip_tree*)kmalloc(DEST_IP_TREE_LEN,GFP_KERNEL);

	if(unlikely(NULL == (*root)))
	{
		printk(KERN_ERR"in init_dest_ip_tree,kmalloc error\n");
		return -1;
	}
	
	memset((*root),0,DEST_IP_TREE_LEN);
	
	if(unlikely(-1 == init_tree_elem(&((*root) -> tree),(tree_elem*)(parent))))
	{
		print_log(KERN_ERR"in init_dest_ip_tree,init_tree_elem \n");
		return -1;
	}
	
	(*root) -> dest_ip 			= ip_addr;
	(*root) -> forward_slot 	= slot_id;

	return 0;
}


int init_ifindex_dest_ip_tree(ifindex_dest_ip_tree** 	root,\
								int 					ifindex,\
								int 					forward_opt,\
								int 					forward_opt_para,\
								_u32 					ip_addr,\
								int 					slot_id,\
								ifindex_dest_ip_tree*	parent)
{
	if(unlikely(NULL != (*root)))
	{
		printk(KERN_ERR"init_ifindex_dest_ip_tree ,root is not NULL \n");
		return -1 ;
	}
	
	(*root) = kmalloc(IFINDEX_DEST_IP_TREE_LEN,GFP_KERNEL);
	
	if(unlikely(NULL == (*root)))
	{
		printk(KERN_ERR"init_ifindex_dest_ip_tree , malloc error\n");
		return -1;
	}
	memset((*root),0,IFINDEX_DEST_IP_TREE_LEN);

	
	if(unlikely(-1 == init_tree_elem(&((*root) -> tree),(tree_elem*)(parent))))
	{
		print_log(KERN_ERR"in init_ifindex_dest_ip_tree ,tree_elem_new return NULL \n");
		return -1;
	}
	if(unlikely(-1 == init_dest_ip_tree(&((*root) -> ip_tree),ip_addr,slot_id,NULL)))
	{
		print_log(KERN_ERR"in init_ifindex_dest_ip_tree ,init_dest_ip_tree error\n");
		return -1;
	}
	
	(*root) -> ifindex 			= 	ifindex;
	(*root) -> forward_opt		=	forward_opt;
	(*root) -> forward_opt_para	=	forward_opt_para;

	return 0;
}

/*
int forwarding_table_init()
{	
	
	memset(forwarding_table,0,sizeof(forwarding_table) );

	test_forwarding_table_init_state();
	return 0;

}
*/

/*************************insert func*************************/



int insert_dest_ip_tree(dest_ip_tree** 	root,\
					_u32 			ip_addr,\
					int 			slot_id,\
					dest_ip_tree*	parent)
{
	if(NULL == (*root))
	{
		if(unlikely(-1 == init_dest_ip_tree(root,ip_addr,slot_id,parent)))
		{
			printk(KERN_ERR"inster_dest_ip_tree,init_dest_ip_tree() error\n");
			return -1;
		}
		
	}else{

		if(unlikely(ip_addr == (*root) -> dest_ip))
		{
			print_log(KERN_ERR"has same dest_ip");
			return -1;
		}else if(ip_addr > (*root) -> dest_ip){

			if(unlikely(-1 == insert_dest_ip_tree((dest_ip_tree**)(&(((*root) -> tree -> rc))),\
										ip_addr,\
										slot_id,\
										(*root))))
			{
				print_log(KERN_ERR"inster_dest_ip_tree error\n");
				return -1;
			}
			
			return 0;
			
		}else if(ip_addr < (*root) -> dest_ip){
		
			if(unlikely(-1 == insert_dest_ip_tree((dest_ip_tree**)(&(((*root) -> tree -> lc))),\
										ip_addr,\
										slot_id,\
										(*root))))
			{
				print_log(KERN_ERR"inster_dest_ip_tree error\n");
				return -1;
			}
			
			return 0;
			
		}
	}
	return 0;
}
			
			

int insert_ifindex_dest_ip_tree(ifindex_dest_ip_tree** 	root,\
								int 					ifindex,\
								int 					forward_opt,\
								int 					forward_opt_para,\
								_u32 					ip_addr,\
								int 					slot_id,\
								ifindex_dest_ip_tree*	parent)
{
	if(NULL == (*root))
	{
		
		if(unlikely(-1 == init_ifindex_dest_ip_tree( root,\
											ifindex,\
											forward_opt,\
											forward_opt_para,\
											ip_addr,\
											slot_id,\
											parent)))
		{
			print_log(KERN_ERR"In inster_ifindex_dest_ip_tree ,init_ifindex_dest_ip_tree () error\n");
			return -1;
		}
		return 0;
	}else{

		if(ifindex == ((*root)->ifindex))
		{
			if(unlikely(-1 == insert_dest_ip_tree(&((*root) -> ip_tree),ip_addr,slot_id,NULL)))
			{
				print_log(KERN_ERR"In inster-ifindex_dest_ip_tree, inster_dest_ip_tree error\n");
				return -1;
			}
			return 0;
		}
		else if(ifindex > ((*root) -> ifindex)){

			if(unlikely(-1 == insert_ifindex_dest_ip_tree((ifindex_dest_ip_tree**)(&((*root) -> tree -> rc)),\
													ifindex,\
													forward_opt,\
													forward_opt_para,\
													ip_addr,\
													slot_id,\
													(*root))))
			{
				print_log(KERN_ERR"inster_ifindex_dest_ip_tree error\n");
				return -1;
			}
			return 0;
		}
		else if(ifindex < ((*root) -> ifindex)){

			if(unlikely(-1 == insert_ifindex_dest_ip_tree((ifindex_dest_ip_tree**)(&((*root) -> tree -> lc)),\
													ifindex,\
													forward_opt,\
													forward_opt_para,\
													ip_addr,\
													slot_id,\
													(*root))))
			{
				print_log(KERN_ERR"inster-ifindex_dest_ip_tree error\n");
				return -1;
			}
		return 0;
		}
	}
	return 0;
}
		
	

		
		
/************************search func***********************/



int look_for_ipaddr_at_dest_ip_tree(dest_ip_tree* root,int ip_addr)
{
	if(NULL == root)
	{
		print_log(KERN_INFO"look for ip_add ,root was NULL \n");
		return -1;
	}

	if(ip_addr == root -> dest_ip)
	{
		return (root -> forward_slot);
	}

	if(ip_addr > root -> dest_ip)
	{
		return (look_for_ipaddr_at_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root),ip_addr));
	}

	if(ip_addr < root -> dest_ip)
	{
		return (look_for_ipaddr_at_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root),ip_addr));
	}
	return 0;

}

/*for extend */

int hook_of_forward_opt(ifindex_dest_ip_tree* root)
{
	if( NULL == root )
	{
		printk(KERN_ERR"hook_of_forward_opt func root is NULL\n");
		return NOTHING;
	}
	if( FOWD_OPT_FOWD_ALL_IP == root -> forward_opt )
	{
		return root -> forward_opt_para;
	}
		
	return NOTHING;
}



int look_for_forward_slot_at_ifindex_tree(ifindex_dest_ip_tree* root,\
										int ifindex,\
										int ip_addr)
{
	if(NULL == root)
	{
	/*	print_log(KERN_INFO"look for ifindex, root was NULL \n"); */
		return -1;
	}

	if(ifindex == root -> ifindex)
	{
		int i;
		int forward_ip;
		i = hook_of_forward_opt(root);
		if(NOTHING != i)
		{
			return i;

		}

		forward_ip = look_for_ipaddr_at_dest_ip_tree(root -> ip_tree,ip_addr);

		return forward_ip;
	}else if(ifindex > root -> ifindex){
	int temp;
	temp = look_for_forward_slot_at_ifindex_tree(GET_RIGHT_CHILD_IFINDEX(root),\
										ifindex,\
										ip_addr);
	return temp;

	}else if(ifindex < root -> ifindex){
	int temp;
	temp = look_for_forward_slot_at_ifindex_tree(GET_LIFT_CHILD_IFINDEX(root),\
										ifindex,\
										ip_addr);
	return temp;

	}
	return 0;
}

/********************release func***********************/


int release_dest_ip_tree(dest_ip_tree* root)
{

	if(NULL == root)
	{
		return 0;
	}else{
	
		if(-1 == release_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root)))
		{
			printk(KERN_ERR"release error\n");
			return -1;
		}

		if(-1 == release_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root)))
		{
			printk(KERN_ERR"release error\n");
			return -1;
		}


		//release self

		kfree(root -> tree);
		kfree(root);

		return 0;
	}
}

int release_ifindex_dest_ip_tree(ifindex_dest_ip_tree* root)
{
	if(NULL == root)
	{
		return 0;
	}else{

		if(unlikely(-1 == release_ifindex_dest_ip_tree(GET_RIGHT_CHILD_IFINDEX(root))))
		{
			printk(KERN_ERR"release error\n");
			return -1;
		}

		if(unlikely(-1 == release_ifindex_dest_ip_tree(GET_LIFT_CHILD_IFINDEX(root))))
		{
			printk(KERN_ERR"release error\n");
			return -1;
		}

		//release slef

		if(unlikely(-1 == release_dest_ip_tree(root -> ip_tree)))
			return -1;
			
		kfree(root -> tree);
		kfree(root);

		return 0;
	}
}

	




/******************************************************
*
*
*				netlink_func
*
*
*******************************************************/

int show_netlink_transport_info(netlink_transport_info* data)
{
	print_log(KERN_INFO"=============get netlink info=================\n");
	print_log(KERN_INFO"netlink_transport_info opt \t\t\t%d\n",data ->opt);
	print_log(KERN_INFO"netlink_transport_info opt_para \t\t%d\n",data ->opt_para);
	print_log(KERN_INFO"netlink_transport_info port \t\t\t%d\n",data ->src_port);
	print_log(KERN_INFO"netlink_transport_info port \t\t\t%d\n",data ->dest_port);
	print_log(KERN_INFO"netlink_transport_info protocol \t\t%d\n",data ->protocol);
	print_log(KERN_INFO"netlink_transport_info ifindex \t\t\t%d\n",data ->ifindex);
	print_log(KERN_INFO"netlink_transport_info ipaddr \t\t\t%d\n",data ->src_ipaddr);
	print_log(KERN_INFO"netlink_transport_info ipaddr \t\t\t%d\n",data ->dest_ipaddr);
	print_log(KERN_INFO"netlink_transport_info slot \t\t\t%d\n",data ->forward_slot);
	print_log(KERN_INFO"=============netlink info over===============\n");
	return 0;
}

static void recv_handler(struct sock * sk, int length)
{
		print_log(KERN_INFO"PFM_KERN:we will recv a netlink message\n");
        wake_up(sk->sk_sleep);
}
#if 0
static int insert_netlink_transport_info_to_ifindex_dest_ip_tree(netlink_transport_info* netlink_data,ifindex_dest_ip_tree* tree)
{
	if(unlikely(NULL == netlink_data || NULL == tree))
	{
		printk(KERN_ERR"PFM_KERN:netlink_data or tree is NULL \n");
		return -1;
	}

	

	if(unlikely(-1 == (insert_ifindex_dest_ip_tree(tree,\
								netlink_data -> ifindex,\
								netlink_data -> forward_opt,\
								netlink_data -> forward_opt_para,\
								netlink_data -> ipaddr,\
								netlink_data -> forward_slot,\
								NULL))))
	{
		printk(KERN_ERR"insert_ifindex_dest_ip_tree error\n");
		return -1;
	}
	//list_all_elem(forwarding_table[0][6]);
			
	return 0;
}


static int insert_netlink_transport_info_to_forward_table(netlink_transport_info* netlink_data)
{
	if(unlikely(NULL == netlink_data))
	{
		printk(KERN_ERR"PFM_KERN:netlink_data is NULL\n");
		return -1;
	}

	if(6 == netlink_data -> protocol)
	{
		if(unlikely(-1 == insert_netlink_transport_info_to_ifindex_dest_ip_tree(netlink_data,(ifindex_dest_ip_tree*)&(forwarding_table[0][netlink_data -> port]))))
		{
			printk(KERN_ERR"PFM_KERN:=insert_netlink_transport_info_to_ifindex_dest_ip_tree error\n");
			return -1;
		}
		return 0;
	}else if(17 == netlink_data -> protocol){
	
		if(unlikely(-1 == insert_netlink_transport_info_to_ifindex_dest_ip_tree(netlink_data,(ifindex_dest_ip_tree*)&(forwarding_table[1][netlink_data -> port]))))
		{
			printk(KERN_ERR"PFM_KERN:insert_netlink_transport_info_to_ifindex_dest_ip_tree error\n");
			return -1;
		}
		return 0;
	}
	print_log(KERN_INFO"PFM_KERN:netlink transport info is not tcp/udp\n");
	return 0;
}

#endif

static int parse_netlink_transport_info(netlink_transport_info* data)
{
	entry_info* entry;
	int i;
	print_log(KERN_INFO"Enter pase_netlink_transport_info!!!!\n");
	print_log(KERN_INFO"data->opt == %d\n",data->opt);
	if(0 != (init_entry_info(&entry)))
		print_log(KERN_INFO"init_entry_info error\n");
	
	if(unlikely(NULL == data))
	{
		printk(KERN_ERR"parse_netlink_transport_info:data is NULL \n");
		return -1;
	}
   
	if (data->opt == DEL || data->opt == 11)	
	{
		print_log(KERN_INFO"get opt from userspace is DEL\n");

		
		set_opt 	(entry, 2);
		set_flag	(entry,data->forward_slot);
		if(data -> opt == DEL)
		set_proto	(entry,data->protocol);
		else
		set_proto	(entry,(data->protocol + 0xff));

		set_ifindex (entry,data->ifindex);
		set_d_addr	(entry,data->dest_ipaddr);
		set_s_addr	(entry,data->src_ipaddr);
		set_d_port	(entry,data->dest_port);
		set_s_port	(entry,data->src_port);
		set_d_mask	(entry,data->dest_ipmask);
		set_s_mask	(entry,data->src_ipmask);
		set_count	(entry,1);

		if(unlikely(-1 == insert_entry(entry)))
		{
			print_log(KERN_ERR"add info error\n");
			kfree(entry);
			return -1;
		}
	}

	if(data -> opt == ADD || data -> opt == 10)
	{
		print_log(KERN_INFO"get opt from userspace is ADD\n");

		
		set_opt 	(entry, 1);
		set_flag	(entry,data->forward_slot);
		if(data -> opt == ADD)
		set_proto	(entry,data->protocol);
		else
		set_proto	(entry,(data->protocol + 0xff));
		set_ifindex (entry,data->ifindex);
		set_d_addr	(entry,data->dest_ipaddr);
		set_s_addr	(entry,data->src_ipaddr);
		set_d_port	(entry,data->dest_port);
		set_s_port	(entry,data->src_port);
		set_d_mask	(entry,data->dest_ipmask);
		set_s_mask	(entry,data->src_ipmask);
		set_count	(entry,1);

		if(unlikely(-1 == insert_entry(entry)))
		{
			print_log(KERN_ERR"add info error\n");
			kfree(entry);
			return -1;
		}
	}	
/*
		if(unlikely(-1 == insert_netlink_transport_info_to_forward_table(data)))
		{
			printk(KERN_ERR"add info error\n");
			return -1;
		}
		
		
     	      list_all_elem(forwarding_table[0][data -> port]);
*/
/*
	if(data -> opt == DEL)
	{
		int protocol;
		int port;
		port = data -> port;
		protocol = (data -> protocol == 6)? 0:1;
		print_log(KERN_INFO"PFM_KERN:port is %d\n",port);
		print_log(KERN_INFO"PFM_KERN:protocol is %d\n",protocol);


		
		print_log(KERN_INFO"get opt from userspace is DEL\n");
		
		if(unlikely(-1 == del_dest_ip_in_ifindex_tree(&(forwarding_table[protocol][port]),\
												data -> ifindex,\
												data -> forward_opt,\
												data -> forward_opt_para,\
												data -> ipaddr)))
		{
			printk(KERN_ERR"del_dest_ip_in_ifindex_tree error\n");
			return -1;
		}
		list_all_elem(forwarding_table[protocol][port]);
	}							
*/

		

	if(data -> opt == SET_LOG_OPT)
	{
		
		print_log(KERN_INFO"get opt from userspace is SET_LOG_OPT\n");
		log_enable = data -> opt_para;
	}

	
	if(data -> opt == SET_PFM_DISABLE)
	{
		print_log(KERN_INFO"get opt from userspace is SET_PFM_DISABLE\n");
		cvm_pfm_rx_hook = NULL;
	}

	
	if(data -> opt == SET_PFM_ENABLE)
	{
		
		print_log(KERN_INFO"get opt from userspace is SET_PFM_ENABLE\n");
		cvm_pfm_rx_hook = catch_packet;
	}
	if(data -> opt == 5)
	{
		print_table_log(KERN_INFO"get opt from userspace is show all elem\n");
		read_lock(&pfm_table_rwlock);
		pfm_record_num_limit = 0;
		for(i=0;i<0x1fe;i++)
		{
			if(proto_table[i] != NULL)
			{
				struct entry_info temp;
				memset(&temp,0,sizeof(struct entry_info));
				
				temp.count =1;
				print_table_log(KERN_INFO"+++++++++++++++++++proto is %d ++++++++++++++++++\n",i);
				pfm_pro_id = i;
				list_all_elem(proto_table[i],&temp,1);

			}
		}
		memset(&pfm_table_info,0,sizeof(pfm_table_info));
		pfm_table_info.opt = -1;/*-1:end*/
		send_netlink_msg(pfm_pid,&pfm_table_info);
		read_unlock(&pfm_table_rwlock);
	}

	
	if(data -> opt == 7)
	{
		
		print_log(KERN_INFO"get opt from userspace is CLEAN_PFM_TABLE\n");
		write_lock(&pfm_table_rwlock);
		init_proto_table();
		
		write_unlock(&pfm_table_rwlock);
	}
	if(data -> opt == SET_PFM_PID)
	{
		
		print_log(KERN_INFO"get opt from userspace is SET_PFM_PID\n");
		pfm_pid = data -> opt_para;
	
	}		
		
	kfree(entry);
	return 0;
}
		



#if 0
static int process_message_thread(void * data)
#endif
static int process_message_thread(struct sock * sk, int length)

{

        struct sk_buff * skb = sk;
        struct nlmsghdr * nlhdr = NULL;
		#if 0
        DEFINE_WAIT(wait);

        daemonize("mynetlink");

        while (exit_flag == 0) {
				print_log(KERN_INFO"PFM_KERN:prepare_to_wait\n");
                prepare_to_wait(netlink_sock->sk_sleep, &wait, TASK_INTERRUPTIBLE);
                schedule();
                finish_wait(netlink_sock->sk_sleep, &wait);
				print_log(KERN_INFO"PFM_KERN:prepard_to_wait over\n");

                while ((skb = skb_dequeue(&netlink_sock->sk_receive_queue))!= NULL) {
#endif

						nlhdr = (struct nlmsghdr *)skb->data;
#if 0
                        if (nlhdr->nlmsg_len < sizeof(struct nlmsghdr)) {
                                printk(KERN_ERR"PFM_KERN:Corrupt netlink message.\n");
                                continue;

						}
						#endif
/*
						if(-1 == insert_netlink_transport_info_to_forward_table((netlink_transport_info*)NLMSG_DATA(nlhdr)))
						{
							return -1;
						}
*/
						show_netlink_transport_info((netlink_transport_info*)NLMSG_DATA(nlhdr));
						if(HAVE_RPA)
						parse_netlink_transport_info((netlink_transport_info*)NLMSG_DATA(nlhdr));

				        /*					
              				       nlhdr->nlmsg_pid = 0;
                     			       nlhdr->nlmsg_flags = 0;
                       		       NETLINK_CB(skb).pid = 0;
                        		       NETLINK_CB(skb).dst_pid = 0;
                        		       NETLINK_CB(skb).dst_group = 1;
                        		       netlink_broadcast(netlink_sock, skb, 0, 1, GFP_KERNEL);
                                           */
          	#if 0   
			}
			
			 print_log(KERN_INFO"PFM_KERN:Corrupt netlink message.\n");	
      }
		#endif
        return 0;
}
#if 0
static int netlink_exam_readproc(char *page, char **start, off_t off,
                          int count, int *eof, void *data)
{
        int len;

        if (off >= buffer_tail) {
                * eof = 1;
                return 0;
        }
        else {
                len = count;
                if (count > PAGE_SIZE) {
                        len = PAGE_SIZE;
                }
                if (len > buffer_tail - off) {
                        len = buffer_tail - off;
                }
                memcpy(page, buffer + off, len);
                *start = page;
                return len;
        }

}
#endif


/***************************************************************
*
*
*					delete data structure func
*
*
***************************************************************/
#if !NEW_PFM_STRUCT

ifindex_dest_ip_tree* look_for_ifindex_in_ifindex_tree(ifindex_dest_ip_tree* root,int ifindex)
{
	if(NULL == root)
	{
		print_log(KERN_INFO"loot_for_ifindex_in_ifindex_tree,can not find ifindex\n");
		return NULL;
	}

	if(ifindex == root -> ifindex)
	{
		return root;
	}

	if(ifindex > root -> ifindex)
	{
		return look_for_ifindex_in_ifindex_tree(GET_RIGHT_CHILD_IFINDEX(root),ifindex);
	}
	if(ifindex < root -> ifindex)
	{
		return look_for_ifindex_in_ifindex_tree(GET_LIFT_CHILD_IFINDEX(root),ifindex);
	}
	return NULL;
}

dest_ip_tree* look_for_dest_ip_in_dest_ip_tree(dest_ip_tree* root,_u32 dest_ip)
{

	if(NULL == root)
	{
		print_log(KERN_INFO"look_for_dest_ip_in_dest_ip_tree,can not find dest_ip\n");
		return NULL;
	}

	if(dest_ip == root -> dest_ip)
	{
		return root;
	}

	if(dest_ip > root -> dest_ip)
	{
		return look_for_dest_ip_in_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root),dest_ip);
	}

	if(dest_ip < root -> dest_ip)
	{
		return look_for_dest_ip_in_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root),dest_ip);
	}
	return NULL;
}

int del_dest_ip_elem(dest_ip_tree* root,ifindex_dest_ip_tree* root_ifindex)
{
	int ret;
	print_log(KERN_INFO"found dest_ip is %d\n",(root) -> dest_ip);
	if(((root) -> tree -> parent == NULL) && \
		((root) -> tree -> lc == NULL) && \
		((root) -> tree -> rc == NULL))
	{
		print_log(KERN_INFO"dest_ip_elem is only one elem\n");
		kfree((root) -> tree);
		kfree(root);
		ret = 1;
		return ret;

	}else{

		ret = 0;
	}

	if((root) -> tree -> lc == NULL)
	{
		print_log(KERN_INFO"dest_ip_elem lc is NULL\n");
		dest_ip_tree* temp;
		temp = (root) -> tree -> parent;
		print_log(KERN_INFO"parent is %d\n",temp -> dest_ip);
		if(GET_LIFT_CHILD_DEST_IP(temp) != NULL && \
			(GET_LIFT_CHILD_DEST_IP(temp) -> dest_ip == (root) -> dest_ip))
		{
			print_log(KERN_INFO"del lc\n");
			(temp) -> tree -> lc = GET_RIGHT_CHILD_DEST_IP(root);
			//change lc parent 
			if(NULL != (temp) -> tree -> lc)
			{
				print_log(KERN_INFO"change lc's parent\n");
				((dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
			}

		}else{
			print_log(KERN_INFO"del rc\n");

			(temp) -> tree -> rc = GET_RIGHT_CHILD_DEST_IP(root);
			
			if(NULL != (temp) -> tree -> rc)
			{
				print_log(KERN_INFO"change rc's parent\n");
				((dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
			}
		}
				
			

		kfree(root -> tree);
		kfree(root);

		return ret;
	}else if((root) -> tree -> rc == NULL){

		print_log(KERN_INFO"dest_ip_elem rc is NULL \n");
		dest_ip_tree* temp;

		temp = (root) -> tree -> parent;
		if(GET_LIFT_CHILD_DEST_IP(temp) != NULL && \
			(GET_LIFT_CHILD_DEST_IP(temp) -> dest_ip == (root) -> dest_ip))
		{
			
			(temp) -> tree -> lc = GET_LIFT_CHILD_DEST_IP(root);

			
			if(NULL != (temp) -> tree -> lc)
			{
				print_log(KERN_INFO"change lc's parent\n");
				((dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
			}

		}else{

			(temp) -> tree -> rc = GET_LIFT_CHILD_DEST_IP(root);

			
			if(NULL != (temp) -> tree -> rc)
			{
				print_log(KERN_INFO"change rc's parent\n");
				((dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
			}
		}

		kfree(temp -> tree);
		kfree(temp);

		return (ret);
	}else{

		print_log(KERN_INFO"dest_ip_elem rc and lc both no NULL\n");
		dest_ip_tree* temp;
		dest_ip_tree* temp2;


		temp 	= root;
		temp2	= GET_RIGHT_CHILD_DEST_IP(root);
		if(GET_PARENT_DEST_IP(root) == NULL)
		{
			ifindex_dest_ip_tree* temp3;
			print_log(KERN_INFO"root is we will del\n");
			temp3 	= root_ifindex;
			temp3 -> ip_tree	= GET_RIGHT_CHILD_DEST_IP(root);
		
		print_log(KERN_INFO"root ifindex is %d\n",temp3 -> ifindex);
		}else{
			dest_ip_tree* temp3;
			if(GET_LIFT_CHILD_DEST_IP(temp3) != NULL && \
				(GET_LIFT_CHILD_DEST_IP(temp3) -> dest_ip == (root) -> dest_ip))
			{
				
				(temp3) -> tree -> lc = GET_RIGHT_CHILD_DEST_IP(root);

				
				if(NULL != (temp) -> tree -> lc)
				{
					print_log(KERN_INFO"change lc's parent\n");
					((dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
				}

			}else{

				(temp3) -> tree -> rc = GET_RIGHT_CHILD_DEST_IP(root);

				
				if(NULL != (temp) -> tree -> rc)
				{
					print_log(KERN_INFO"change rc's parent\n");
					((dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
				}
			}
		}


		
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		
		if(NULL != (temp2) -> tree -> lc)
		{
			print_log(KERN_INFO"change lc's parent\n");
			((dest_ip_tree*)((temp2) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp2);
		}

		kfree(temp -> tree);
		kfree(temp);
		return ret;
	}
	return ret;
}


int del_ifindex_tree_elem(ifindex_dest_ip_tree* root,ifindex_dest_ip_tree** forwarding_table)
{
	#if 1
	
	int ret;
	print_log(KERN_INFO"found ifindex is %d\n",(root) -> ifindex);
	if(((root) -> tree -> parent == NULL) && \
		((root) -> tree -> lc == NULL) && \
		((root) -> tree -> rc == NULL))
	{
		print_log(KERN_INFO"ifindex_elem is only one elem\n");
		kfree((root) -> tree);
		kfree(root);
		ret = 1;
		return ret;

	}else{

		ret = 0;
	}

	if((root) -> tree -> lc == NULL)
	{
		print_log(KERN_INFO"ifindex_elem lc is NULL\n");
		ifindex_dest_ip_tree* temp;
		temp = (root) -> tree -> parent;
		print_log(KERN_INFO"parent is %d\n",temp -> ifindex);
		if(GET_LIFT_CHILD_IFINDEX(temp) != NULL && \
			(GET_LIFT_CHILD_IFINDEX(temp) -> ifindex == (root) -> ifindex))
		{
			print_log(KERN_INFO"del lc\n");
			(temp) -> tree -> lc = GET_RIGHT_CHILD_IFINDEX(root);

			
			if(NULL != (temp) -> tree -> lc)
			{
				print_log(KERN_INFO"change lc's parent\n");
				((ifindex_dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
			}

		}else{
			print_log(KERN_INFO"del rc\n");

			(temp) -> tree -> rc = GET_RIGHT_CHILD_IFINDEX(root);

			if(NULL != (temp) -> tree -> rc)
			{
				print_log(KERN_INFO"change rc's parent\n");
				((ifindex_dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
			}
		}
				
			

		kfree(root -> tree);
		kfree(root);

		return ret;
	}else if((root) -> tree -> rc == NULL){

		print_log(KERN_INFO"ifindex_elem rc is NULL \n");
		ifindex_dest_ip_tree* temp;

		temp = (root) -> tree -> parent;
		if(GET_LIFT_CHILD_IFINDEX(temp) != NULL && \
			(GET_LIFT_CHILD_IFINDEX(temp) -> ifindex == (root) -> ifindex))
		{
			
			(temp) -> tree -> lc = GET_LIFT_CHILD_IFINDEX(root);
			
			if(NULL != (temp) -> tree -> lc)
			{
				print_log(KERN_INFO"change lc's parent\n");
				((ifindex_dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
			}

		}else{

			(temp) -> tree -> rc = GET_LIFT_CHILD_IFINDEX(root);
			
			if(NULL != (temp) -> tree -> rc)
			{
				print_log(KERN_INFO"change rc's parent\n");
				((ifindex_dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
			}
		}

		kfree(temp -> tree);
		kfree(temp);

		return (ret);
	}else{

		print_log(KERN_INFO"ifindex_elem rc and lc both no NULL\n");
		ifindex_dest_ip_tree* temp;
		ifindex_dest_ip_tree* temp2;


		temp 	= root;
		temp2	= GET_RIGHT_CHILD_IFINDEX(root);
		if(GET_PARENT_IFINDEX(root) == NULL)
		{
			ifindex_dest_ip_tree** temp3;
			print_log(KERN_INFO"root is we will del\n");
			temp3 	= forwarding_table;
			(*temp3)= GET_RIGHT_CHILD_IFINDEX(root);
		
		}else{
			ifindex_dest_ip_tree* temp3;
			if(GET_LIFT_CHILD_IFINDEX(temp3) != NULL && \
				(GET_LIFT_CHILD_IFINDEX(temp3) -> ifindex == (root) -> ifindex))
			{
				
				(temp3) -> tree -> lc = GET_RIGHT_CHILD_IFINDEX(root);

				
				if(NULL != (temp) -> tree -> lc)
				{
					print_log(KERN_INFO"change lc's parent\n");
					((ifindex_dest_ip_tree*)((temp) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
				}

			}else{

				(temp3) -> tree -> rc = GET_RIGHT_CHILD_IFINDEX(root);
				
				if(NULL != (temp) -> tree -> rc)
				{
					print_log(KERN_INFO"change rc's parent\n");
					((ifindex_dest_ip_tree*)((temp) -> tree -> rc)) -> tree -> parent = (tree_elem*)(temp);
				}
			}
		}


		
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (ifindex_dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		
		if(NULL != (temp2) -> tree -> lc)
		{
			print_log(KERN_INFO"change lc's parent\n");
			((ifindex_dest_ip_tree*)((temp2) -> tree -> lc)) -> tree -> parent = (tree_elem*)(temp);
		}

		kfree(temp -> tree);
		kfree(temp);
		return ret;
	}
	return ret;
	#else
	int ret;
	if(((*root) -> tree -> parent == NULL) && \
		((*root) -> tree -> rc == NULL) && \
		((*root) -> tree -> lc == NULL))
	{
		
		free((*root) -> tree);
		free(*root);
		ret = 1;
		return ret;
		
	}else{
	
		ret = 0;
		
	}

	
	if((*root) -> tree -> lc == NULL)
	{
		ifindex_dest_ip_tree* temp;
		temp = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> rc);

		free(temp -> tree);
		free(temp);

		return ret;
	}else if((*root) -> tree -> rc == NULL){

		ifindex_dest_ip_tree* temp;
		temp = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> lc);

		free(temp -> tree);
		free(temp);

		return (ret);
	}else{

		ifindex_dest_ip_tree* temp;
		ifindex_dest_ip_tree* temp2;
		temp = temp2 = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> rc);
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (ifindex_dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		free(temp -> tree);
		free(temp);
		return ret;
	}
	return ret;
	#endif
}



int hook_of_del_opt(ifindex_dest_ip_tree** root,\
					int opt,\
					int opt_para)
{
	return 0;
}

int del_dest_ip_in_ifindex_tree(ifindex_dest_ip_tree** root,\
								int ifindex,\
								int	opt,\
								int opt_para,\
								_u32 dest_ip)
{
	ifindex_dest_ip_tree *temp_ifindex;
	dest_ip_tree *temp_dest_ip;
/*	dest_ip_tree **temp;*/
	
	if(NULL == (*root))
	{
		print_log(KERN_INFO"del_dest_ip_in_ifindex_tree, root is NULL\n");
		return -1;
	}
	/*look_for_ifindex_in_ifindex_tree*/
#if 1
	temp_ifindex = look_for_ifindex_in_ifindex_tree((*root),ifindex);

	
	if(NULL == temp_ifindex)
	{
		print_log(KERN_INFO"del_dest_ip_in_ifindex_tree, can not find ifindex in tree");
		return -1;
	}
#else

	for(temp_ifindex = (*root);temp_ifindex -> ifindex != ifindex;)
	{
		if(ifindex > temp_ifindex)
		{
			temp_ifindex = GET_LIFT_CHILD_IFINDEX(temp_ifindex);
		}
		if(ifindex < temp_ifindex)
		{
			temp_ifindex = GET_RIGHT_CHILD_IFINDEX(temp_ifindex);
		}
		if(NULL == temp_ifindex)
		{
			print_log(KERN_INFO"can not find ifindex\n");
			return 0;
		}
	}


#endif
	

	if(hook_of_del_opt((temp_ifindex),opt,opt_para))
	{
		return 0;
	}
/*lookfor dest_ip*/
	#if 1
	temp_dest_ip = look_for_dest_ip_in_dest_ip_tree((temp_ifindex) -> ip_tree,dest_ip);
	if(NULL == (temp_dest_ip))
	{
		print_log(KERN_INFO"del_dest_ip_in_ifindex_tree, can not find dest_ip in tree");
		return -1;
	}
	#else
	
	for(temp_dest_ip = (temp_ifindex -> ip_tree);temp_dest_ip -> dest_ip != dest_ip;)
	{
		if(dest_ip > temp_dest_ip -> dest_ip)
		{
			temp_dest_ip = GET_LIFT_CHILD_IFINDEX(temp_dest_ip);
		}
		if(dest_ip < temp_dest_ip -> dest_ip)
		{
			temp_ifindex = GET_RIGHT_CHILD_IFINDEX(temp_ifindex);
		}
		if(NULL == temp_ifindex)
		{
			print_log(KERN_INFO"can not find ifindex\n");
			return 0;
		}
	}
	#endif
	/*temp = &temp_dest_ip;*/
	if(del_dest_ip_elem(temp_dest_ip,temp_ifindex))
	{	
		print_log(KERN_INFO"dest_ip_tree is NULL ,we will del ifindex tree\n");

		if(del_ifindex_tree_elem(temp_ifindex,root))
		{
			print_log(KERN_INFO"ifindex_tree_elem is NULL, we will make forwarding table is null\n");
			(*root) = NULL;
		}else{
			print_log(KERN_INFO"ifindex_tree_elem is not null,del it\n");
		}
	}else{

		print_log(KERN_INFO"dest_ip_tree is not null ,del it\n");
	}

	return 0;
}
#endif


#if NEW_PFM_STRUCT

int init_proto_table(void)
{
	int i;
	print_log(KERN_INFO"pfm : enter init_proto_table\n");
	
	for(i=0;i<0x1fe;i++)
	{
		proto_table[i] = NULL;
	}
	for(i=0;i<0x1fe;i++)
	{
		if(proto_table[i]!=NULL)
		{
			print_log(KERN_INFO"proto_table of %d is not NULL\n",i);
		}
	}
	print_log(KERN_INFO"init proto_table OK\n");
	return 0;
}
int init_date2(struct date** temp)
{
	*temp = kmalloc(sizeof(struct date),GFP_KERNEL);
	if(NULL != *temp)
	{
		mem_use += sizeof(struct date);
		#if HAVE_IPV6_ADDR
		memset(&((*temp) -> date),0,sizeof(struct in6_addr));
		((*temp) -> date).s6_addr32[0] = -1;
		#else
		(*temp) -> date = -1;
		#endif
		(*temp) -> opt = 0;
		(*temp) -> opt_para = 0;
		(*temp) -> next_keyword = NULL;
		(*temp) -> tree = NULL;/*
		(*temp) -> tree = kmalloc(sizeof(struct tree_elem), GFP_KERNEL);
		if(NULL != (*temp) -> tree)
		{
			mem_use += sizeof(struct tree_elem);
			(*temp) -> tree -> rc = NULL;
			(*temp) -> tree -> lc = NULL;
			(*temp) -> tree -> parent = NULL;
			(*temp) -> tree -> deep = 0;

		}else{*/
			printk(KERN_INFO"malloc of struct tree_elem error\n");
			kfree(*temp);
			*temp=NULL;
			mem_use -=sizeof(struct date);
			return -1;
	/*	}*/
	}else{
		printk(KERN_INFO"malloc of struct date error\n");
		return -1;
	}
	return 0;
}


int init_date(struct date** temp)
{
	*temp = kmalloc(sizeof(struct date),GFP_KERNEL);
	if(NULL != *temp)
	{
		mem_use += sizeof(struct date);
		#if HAVE_IPV6_ADDR
		memset(&((*temp) -> date),0,sizeof(struct in6_addr));
		((*temp) -> date).s6_addr32[0] = -1;
		#else
		(*temp) -> date = -1;
		#endif
		(*temp) -> opt = 0;
		(*temp) -> opt_para = 0;
		(*temp) -> next_keyword = NULL;
		(*temp) -> tree = NULL;
		(*temp) -> tree = kmalloc(sizeof(struct tree_elem), GFP_KERNEL);
		if(NULL != (*temp) -> tree)
		{
			mem_use += sizeof(struct tree_elem);
			(*temp) -> tree -> rc = NULL;
			(*temp) -> tree -> lc = NULL;
			(*temp) -> tree -> parent = NULL;
			(*temp) -> tree -> deep = 0;

		}else{
			printk(KERN_INFO"malloc of struct tree_elem error\n");
			kfree(*temp);
			*temp=NULL;
			mem_use -=sizeof(struct date);
			return -1;
		}
	}else{
		printk(KERN_INFO"malloc of struct date error\n");
		return -1;
	}
	return 0;
}
int show_entry_info(struct entry_info* entry)
	{
		print_log(KERN_INFO"***********show entry_info************\n");
		print_log(KERN_INFO"opt is \t\t\t :%d\n",get_opt(entry));
		print_log(KERN_INFO"slot is \t\t :%d\n",get_flag(entry));
		print_log(KERN_INFO"proto is \t\t :%d\n",get_proto(entry));
		print_log(KERN_INFO"ifindex is \t\t :%d\n",get_ifindex(entry));
		print_log(KERN_INFO"d_port is \t\t :%d\n",get_d_port(entry));
		print_log(KERN_INFO"s_port is \t\t :%d\n",get_s_port(entry));
		print_log(KERN_INFO"d_addr is \t\t :");
		print_ip_o(get_d_addr(entry));
		print_log(KERN_INFO"d_mask is \t\t :%d\n",get_d_mask(entry));
		print_log(KERN_INFO"s_addr is \t\t :");
		print_ip_o(get_s_addr(entry));
		print_log(KERN_INFO"s_mask is \t\t :%d\n",get_s_mask(entry));
		
		print_log(KERN_INFO"count is \t\t :%d\n",get_count(entry));
		
		print_log(KERN_INFO"**************************************\n");
		return 1;
	}

int show_entry_table_info(struct entry_info* entry)
	{
		print_table_log(KERN_INFO"***********show entry_info************\n");
		print_table_log(KERN_INFO"opt is \t\t\t :%d\n",get_opt(entry));
		print_table_log(KERN_INFO"slot is \t\t :%d\n",get_flag(entry));
		print_table_log(KERN_INFO"proto is \t\t :%d\n",get_proto(entry));
		print_table_log(KERN_INFO"ifindex is \t\t :%d\n",get_ifindex(entry));
		print_table_log(KERN_INFO"d_port is \t\t :%d\n",get_d_port(entry));
		print_table_log(KERN_INFO"s_port is \t\t :%d\n",get_s_port(entry));
		print_table_log(KERN_INFO"d_addr is \t\t :");
		print_ip_o_table(get_d_addr(entry));
		print_table_log(KERN_INFO"d_mask is \t\t :%d\n",get_d_mask(entry));
		print_table_log(KERN_INFO"s_addr is \t\t :");
		print_ip_o_table(get_s_addr(entry));
		print_table_log(KERN_INFO"s_mask is \t\t :%d\n",get_s_mask(entry));
		
		print_table_log(KERN_INFO"count is \t\t :%d\n",get_count(entry));
		
		print_table_log(KERN_INFO"**************************************\n");
		return 1;
	}

#if HAVE_IPV6_ADDR
int get_value_from_entry(const struct entry_info* entry,struct in6_addr* temp)
#else
int get_value_from_entry(const struct entry_info* entry)
#endif
{
	int value;
	#if HAVE_IPV6_ADDR
	if(NULL == entry || NULL == temp)
	#else
	if(NULL == entry)
	#endif
	{
		printk(KERN_ERR"func:get_value_form_entry entry is NULL \n");
		return -1;
	}
	switch (entry -> count)
	{
		#if HAVE_IPV6_ADDR
		case 0:
			temp->s6_addr32[0] = entry -> proto;
			value = entry -> proto;
			break;
		case 1:
			temp->s6_addr32[0] = entry -> ifindex;
			value = entry -> ifindex;
			break;
		case 2:
			memcpy(temp,&(entry->d_addr),sizeof(struct in6_addr));
			value = (entry -> d_addr).s6_addr32;
			break;
		case 3:
			memcpy(temp,&(entry->s_addr),sizeof(struct in6_addr));
			value = (entry -> s_addr).s6_addr32;
			break;
		case 4:
			temp->s6_addr32[0] = entry -> d_port;
			value = entry -> d_port;
			break;
		case 5:
			temp->s6_addr32[0] = entry -> s_port;
			value = entry -> s_port;
			break;
		default:
			return -2;
		#else
		
		
		case 0:
			value = entry -> proto;
			break;
		case 1:
			value = entry -> ifindex;
			break;
		case 2:
			value = entry -> d_port;
			break;
		case 3:
			value = entry -> s_port;
			break;	
		case 4:
			return -3;
		case 5:
			value = entry -> d_mask;
			break;
		case 6:
			value = entry -> d_addr;
			break;
		case 7:
			value = entry -> s_mask;
			break;
		case 8:
			value = entry -> s_addr;
			break;
		default:
			return -2;
		
		#endif
		
	}
/*	entry -> count += 1;*/
	return value;
}

int add_addr_to_proto_table(struct date** temp_date,struct entry_info* temp_entry,int mask)
{
	int value=0;
	
	print_log(KERN_INFO"************add addr***************\n");
	value = get_value_from_entry(temp_entry);
	if(value == -2)
	{
		print_log(KERN_INFO"over return -1");
		return -1;
	}
	if(0 == mask)
		value = 0;
	apply_mask_ipv4(&value,mask);
	
	print_log(KERN_INFO"value is :%d add mask is %d addr is :",value,mask);
	print_ip_o(value);

	if((*temp_date) == NULL)
	{
		
		if(init_date(temp_date) != 0)
		{
			print_log(KERN_INFO"init_date error\n");
			return -1;
		}

		{
		
			(*temp_date)->date =value;

			temp_entry->count ++;
			if(-2 == get_value_from_entry(temp_entry))
			{
			print_log(KERN_INFO"add entry over\n");

				(*temp_date) -> opt = get_opt(temp_entry);
				(*temp_date) -> opt_para = get_flag(temp_entry);
				return 0;
			}
			
			
		}

		return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
	}else{
		print_log(KERN_INFO"value is %d,date is %d\n",value,(*temp_date) -> date);
		print_ip_o(value);
		print_ip_o((*temp_date)->date);

		if(value == (*temp_date) -> date)
		{
			temp_entry->count ++;
			
			if(-2 == get_value_from_entry(temp_entry))
			{
			print_log(KERN_INFO"add entry over\n");

				(*temp_date) -> opt = get_opt(temp_entry);
				(*temp_date) -> opt_para = get_flag(temp_entry);
				return 0;
			}
			return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
		}
		if(value > (*temp_date) -> date)
			return (add_addr_to_proto_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry,mask));
		if(value < (*temp_date) -> date)
			return (add_addr_to_proto_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry,mask));
		
}

	return -1;/*coverity cid:11349 issue missing return*/
		
	
}
#if 1
int add_mask_to_proto_table(struct date** temp_date,struct entry_info* temp_entry)
{
	int value=0;
	print_log(KERN_INFO"************add mask***************\n");
	value = get_value_from_entry(temp_entry);
	if(value == -2)
	{
		print_log(KERN_INFO"over return -1");
		return -1;
	}
	print_log(KERN_INFO"value is %d\n",value);
	if((*temp_date) == NULL)
	{
		
		if(init_date(temp_date) != 0)
		{
			print_log(KERN_INFO"init_date error\n");
			return -1;
		}

		
		(*temp_date)->date = value;

		temp_entry->count ++;

		return(add_addr_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,value));
	}else{

		if(value == (*temp_date) -> date)
		{
			print_log(KERN_INFO"mask match\n");
			temp_entry->count ++;
			return(add_addr_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,value));
		}
		if(value > (*temp_date) -> date)
			return (add_mask_to_proto_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry));
		if(value < (*temp_date) -> date)
			return (add_mask_to_proto_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry));
		
		
}
		return -1;/*coverity cid:11350 issue missing return */
}
#endif	
int add_date_to_proto_table(struct date** temp_date,struct entry_info* temp_entry,int is_root)
{
	int write_value;
	int return_value;
    print_log(KERN_INFO"\n***********************************************\n");
	if((*temp_date) == NULL && is_root == 1)
	{
		print_log(KERN_INFO"root is NULL make it\n");
		if(get_value_from_entry(temp_entry) == -2)
		{
			print_log(KERN_INFO"over return -1");
			return 0;
		}
		if(init_date(temp_date) != 0)
		{
			print_log(KERN_INFO"init_date error\n");
			return -1;
		}

		if(0 == get_value_from_entry(temp_entry))
		{
			print_log(KERN_INFO"value is 0");
			(*temp_date) -> date = 0;
			/*
			//if key_word is "d_addr" or "s_addr",set mask
			{
				if(get_count(temp_entry) == 2)
				{
					if(((*temp_date) -> opt_para) < 128)
					{
						(*temp_date) -> opt = 10;
						(*temp_date) -> opt_para = get_d_mask(temp_entry);
					}else{
						(*temp_date) -> opt = 11;
						(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
					}
				}

				if(get_count(temp_entry) == 3)
				{
					if(((*temp_date) -> opt_para) < 128)
					{
						(*temp_date) -> opt = 10;
						(*temp_date) -> opt_para = get_d_mask(temp_entry);
					}else{
						(*temp_date) -> opt = 11;
						(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
					}
				}
			}
*/
			temp_entry -> count += 1;
			if(-3 == get_value_from_entry(temp_entry))
			{
			print_log(KERN_INFO"ifindex and port add over,add addr and mask\n");
			temp_entry -> count += 1;

			return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
			}
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,1));
		}
		print_log(KERN_INFO"IN root value is not 0,get lc");
	
		(*temp_date) -> date = -1;

		write_value = get_value_from_entry(temp_entry);

		if(write_value > (*temp_date) -> date)
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry,0));

		if(write_value < (*temp_date) -> date)
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry,0));
				
	}
	
	write_value = get_value_from_entry(temp_entry);
	
	
	if(-1 == write_value)
	{
		print_log(KERN_INFO"get write_value error\n");
		return -1;
	}
	if(-2 == write_value)
	{
		print_log(KERN_INFO"add entry over\n");
		return 0;
	}

	print_log(KERN_INFO"write_value is :%d\n",write_value);

	if(write_value == 0 && is_root != 1)
	{
		print_log(KERN_INFO"get_value_from_entry is 0 and is not root\n");
		return -1;
	}

	if(write_value == 0)
	{
		
		(*temp_date) -> date = 0;
		/*
		//if key_word is "d_addr" or "s_addr",set mask
		{
			if(get_count(temp_entry) == 2)
			{
				if(((*temp_date) -> opt_para) < 128)
				{
					(*temp_date) -> opt = 10;
					(*temp_date) -> opt_para = get_d_mask(temp_entry);
				}else{
					(*temp_date) -> opt = 11;
					(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
				}
			}

			if(get_count(temp_entry) == 3)
			{
				if(((*temp_date) -> opt_para) < 128)
				{
					(*temp_date) -> opt = 10;
					(*temp_date) -> opt_para = get_d_mask(temp_entry);
				}else{
					(*temp_date) -> opt = 11;
					(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
				}
			}
		}
	*/
		temp_entry -> count += 1;
		print_log(KERN_INFO"count is %d\n",temp_entry->count);
		if(-3 == get_value_from_entry(temp_entry))
		{
			print_log(KERN_INFO"ifindex and port add over,add addr and mask\n");
			temp_entry -> count += 1;

			return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
		}
		return (add_date_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,1));
	}


	if((*temp_date) == NULL)
	{
		print_log(KERN_INFO"proto_table of %d is NULL\n",get_proto(temp_entry));
/*		if(init_date(temp_date) != 0)
		{
			print_log("init_date error\n");
			return -1;
		}
		write_value = get_value_form_entry(temp_entry);
		if(-1 == write_value)
		{
			print_log("get write_value error\n");
			return -1;
		}
		if(-2 == write_value)
		{
			print_log("add entry over\n");
			return 0;
		}
		print_log("write_value is :%d\n",write_value);
	*/	
		if(init_date(temp_date) != 0)
		{
			print_log(KERN_INFO"init_date error\n");
			return -1;
		}


		(*temp_date) -> date = write_value;
		/*
		//if key_word is "d_addr" or "s_addr",set mask
		{
				if(get_count(temp_entry) == 2)
				{
					if(((*temp_date) -> opt_para) < 128)
					{
						(*temp_date) -> opt = 10;
						(*temp_date) -> opt_para = get_d_mask(temp_entry);
						(*temp_date) -> date = write_value
					}else{
						(*temp_date) -> opt = 11;
						(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
					}
				}

				if(get_count(temp_entry) == 3)
				{
					if(((*temp_date) -> opt_para) < 128)
					{
						(*temp_date) -> opt = 10;
						(*temp_date) -> opt_para = get_d_mask(temp_entry);
					}else{
						(*temp_date) -> opt = 11;
						(*temp_date) -> opt_para = get_d_mask(temp_entry) -128;
					}
				}
			}
*/
		temp_entry -> count += 1;

		if(get_value_from_entry(temp_entry) == -3)
		{
			print_log(KERN_INFO"ifindex and port add over,add addr and mask\n");
			temp_entry -> count += 1;
			return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
		}

		if((return_value = add_date_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,1))!= 0)
		{
			print_log(KERN_INFO"add_date_to_proto_table error\n");
			return -1;
		}
		return return_value;

	}else{

		if(write_value == (*temp_date) -> date)
		{
			print_log(KERN_INFO"find write_value , get next_keyword\n");
			temp_entry -> count += 1;
			if(get_value_from_entry(temp_entry) == -3)
			{
				print_log(KERN_INFO"ifindex and port add over,add addr and mask\n");
				temp_entry -> count += 1;
				
				return(add_mask_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));
			}
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,1));
		}

		if(write_value > (*temp_date) -> date)
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry,0));

		if(write_value < (*temp_date) -> date)
			return (add_date_to_proto_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry,0));
	}
	return -1;
}




int get_slot_from_addr_table(struct date** temp_date,struct entry_info* temp_entry,int mask)
{
	int value;
	int temp;/*zhaocg modify*/
	print_log(KERN_INFO"*************get_slot_from_addr_table*************\n");
	print_log(KERN_INFO"***count=%d\n",temp_entry -> count);/*zhaocg add*/
	if((*temp_date) == NULL)
		return -1;
	value = get_value_from_entry(temp_entry);
	print_log(KERN_INFO"get_slot_from_addr_table func,value is:%d\t",value);
	print_ip_o(value);
	
	if(0 == mask)
	{
		//int temp;
		
		temp_entry -> count ++;
		if(-2 == get_value_from_entry(temp_entry))
		{
			print_log(KERN_INFO"get slot is :%d,flag is :%d\n",(*temp_date) -> opt,(*temp_date) -> opt_para);
			set_opt(temp_entry,(*temp_date) -> opt);
			set_flag(temp_entry,(*temp_date) -> opt_para);
			if(2 == (*temp_date) -> opt)
			{
				temp_entry->count -=1;
				return -1;
			}
			return 0;
		}
		
		temp_entry -> count ++;
		//return(get_slot_from_mask_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry));

		/*zhaocg modify*/
		temp = get_slot_from_mask_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry);
		if(temp != 0)
				temp_entry -> count -=2;
		return temp;
		
	}else{

		if(1 == prefix_same(value,(*temp_date)->date,mask))
		{
			print_log(KERN_INFO"net code match\n");
			temp_entry -> count ++;
			if(-2 == get_value_from_entry(temp_entry))
			{
				print_log(KERN_INFO"get slot is :%d,flag is :%d\n",(*temp_date) -> opt,(*temp_date) -> opt_para);
				set_opt(temp_entry,(*temp_date) -> opt);
				set_flag(temp_entry,(*temp_date) -> opt_para);
				if(2 == (*temp_date) -> opt)
				{
					temp_entry->count -=1;
					return -1;
				}
				return 0;
			}
			
			temp_entry -> count ++;
		
			temp =get_slot_from_mask_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry);
			/*zhaocg modify*/
			if(temp != 0)
				temp_entry -> count -=2;
			return temp;
		}
		if(value > (*temp_date)->date)
			return(get_slot_from_addr_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry,mask));
		if(value < (*temp_date)->date)
			return(get_slot_from_addr_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry,mask));
	
		return -1;/*add by dingkang because coverity cid:11351 */
	
	}
}
		
	
	
int get_slot_from_mask_table(struct date** temp_date,struct entry_info* temp_entry)
{
	
	print_log(KERN_INFO"*************get_slot_from_mask_table*************\n");
	print_log(KERN_INFO"***count=%d\n",temp_entry -> count-1);/*zhaocg add*/

	if((*temp_date)== NULL)
		return -1;
	if(-1 == get_slot_from_mask_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry))
	{
		print_log(KERN_INFO"mask :%d is less\n",(*temp_date)->date);
		if(0==get_slot_from_addr_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry,(*temp_date)->date))
			return 0;
		else
		{
			if(temp_entry->count == 8)
			{
				print_log(KERN_INFO"find rigth tree addr\n");
				//temp_entry->count -=2;/*zhaocg modify*/
			}
		}
		
		return(get_slot_from_mask_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry));
	}
	return 0;
}
	
		



int get_slot_from_table(struct date** temp_date,struct entry_info* temp_entry)
{
	int find_value;
	int outflag=0;
	int outflag2=0;
	if(temp_entry == NULL)
	{
		print_log(KERN_INFO"temp_entry is null\n");
		return -1;
	} 
	if((*temp_date) == NULL)
	{
		
		print_log(KERN_INFO"can't find entry\n");
		return -1;
	}

	find_value = get_value_from_entry(temp_entry);

	if(find_value == -2)
	{
		print_log(KERN_INFO"find_vlaue = -2");
		return -1;
	}


	if(0 == (*temp_date) -> date)
	{
		int temp;
		print_log(KERN_INFO"temp_date -> date is 0\n");
		temp_entry -> count += 1;
		
		if(-3 == get_value_from_entry(temp_entry))
		{
			print_log(KERN_INFO"get ifindex port over\n");
			
			temp_entry -> count += 2;
			if(0!= get_slot_from_mask_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry))
			{
				outflag=1;
				goto out;
			}
			return 0;
		}
		temp = get_slot_from_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry);
		if(temp == 0)
			return temp;
		else
		{
			outflag2=1;
			goto out2;
		}
			
	out:
		if(1 == outflag)
		{
			print_log(KERN_INFO"countinue find end of mask and ipaddr\n");
			temp_entry -> count -=3;
			outflag =0;
		}
	out2:
		if(1 == outflag2)
		{
			
			print_log(KERN_INFO"countinue find \n");
			temp_entry -> count -=1;
			outflag2=0;
		}

	}
print_log(KERN_INFO"find_value is %d,date is :%d\n",find_value,(*temp_date) -> date);
	if(find_value == (*temp_date) -> date)
	{
		int temp;	
		print_log(KERN_INFO"find_value == date,get next_keyword\n");
		print_log(KERN_INFO"***count=%d\n",temp_entry -> count);/*zhaocg add*/
		temp_entry -> count += 1;
		if(-3 == get_value_from_entry(temp_entry))
		{
		
			print_log(KERN_INFO"get ifindex port over\n");
			print_log(KERN_INFO"***count=%d\n",temp_entry -> count);/*zhaocg add*/
			temp_entry -> count += 2;
			if(0!= get_slot_from_mask_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry))
			{
			/*	temp_entry -> count -=1;*/
				temp_entry -> count -=3;/*zhaocg modify*/
				print_log(KERN_INFO"***count=%d,from mask\n",temp_entry -> count);/*zhaocg add*/
				return -1;
			}
			return 0;
		}
		temp= get_slot_from_table((struct date**)(&((*temp_date) -> next_keyword)),temp_entry);
		temp_entry -> count -=1;
		return temp;
	}
 
	if(find_value > (*temp_date) -> date)
	{
		
		print_log(KERN_INFO"find_value > date,get lc\n");
		return (get_slot_from_table((struct date**)(&((*temp_date) -> tree -> lc)),temp_entry));
	}

	if(find_value < (*temp_date) -> date)
	{
		print_log(KERN_INFO"find_value < date,get rc\n");
		return (get_slot_from_table((struct date**)(&((*temp_date) -> tree -> rc)),temp_entry));
	}
	return -1;
}





				
int insert_entry(entry_info* entry)
	{
		print_log(KERN_INFO"proto is:\t\t%d\nifindex is\t\t%d\nd_port is:\t\t%d\ns_port is:\t\t%d\nd_addr is:\t\t%d\nd_mask is:\t\t%d\ns_addr is:\t\t%d\ns_mask is:\t\t%d\ncount is:%d\n",get_proto(entry),get_ifindex(entry),get_d_port(entry),get_s_port(entry),get_d_addr(entry),get_d_mask(entry),get_s_addr(entry),get_s_mask(entry),get_count(entry));
		print_log(KERN_INFO"s_addr is:");
		print_ip_o(get_s_addr(entry));
		print_log(KERN_INFO"d_addr is:");
		print_ip_o(get_d_addr(entry));
		print_log(KERN_INFO"enter insert_entry!!!!!!!\n");
		write_lock(&pfm_table_rwlock);
		add_date_to_proto_table(&proto_table[get_proto(entry)],entry,1);
		write_unlock(&pfm_table_rwlock);
		return 0;
	}


	
/*	
       set_count(entry,1);
	print_log(KERN_INFO"++++++++++++++++++++++++++++++++++++++++++++\n");
	add_date_to_proto_table(&proto_table[get_proto(entry)],entry,1);
	set_count(entry,1);
	set_proto(entry,9);
	print_log(KERN_INFO"=============================================\n");
	add_date_to_proto_table(&proto_table[get_proto(entry)],entry,1);
*/


/*
int delete_entry_from_table(struct date** temp_date,struct entry_info* temp_entry)
{
	long find_value;
	if(temp_entry == NULL)
	{
		print_log(KERN_INFO"in delete_entry_form_table temp_entry is NULL\n");
		return -1;
	}
	
	if((*temp_date) == NULL)
	{
		return 0;
	}
	
	find_value = get_value_from_entry(temp_entry);

	if(find_vlaue == -2)
	{
		print_log(KERN_INFO"get_vlaue_from_entry error\n");
		return -1;
	}
	if(find_value == (*temp_date) -> date)
	{

*/


int init_entry_info(entry_info** temp)
{
	//entry_info* temp;
	*temp = NULL;
	*temp = kmalloc(sizeof(entry_info),GFP_KERNEL);
	if(*temp == NULL)
		return -1;
	memset(*temp,0,sizeof(entry_info));
	return 0;
}

int back_value_to_entry(struct entry_info* entry)
	{
		entry -> count --;
		switch (entry -> count)
		{
	case 0:
		entry -> proto = 0;
		break;
	case 1:
		entry -> ifindex = 0;
		break;
	case 2:
		entry -> d_port= 0;
		break;
	case 3:
		entry -> s_port= 0;
		break;
	case 4:
		entry -> d_mask= 0;
		break;
	case 5:
		entry -> d_addr= 0;
		break;
	case 6:
		entry -> s_mask= 0;
		break;
	case 7:
		entry -> s_addr= 0;
		return -1;
		break;
	default:
		return -2;
		}
		return -2;
	}

int send_value_to_entry(struct entry_info* entry,int date)
{
#if HAVE_IPV6_ADDR
	if(NULL == entry || NULL == temp)
#else
	if(NULL == entry)
#endif
	{
		print_log(KERN_ERR,"func:get_value_form_entry entry is NULL \n");
		return -1;
	}
	switch (entry -> count)
	{
	#if HAVE_IPV6_ADDR
		case 0:
			temp->s6_addr32[0] = entry -> proto;
			value = entry -> proto;
			break;
		case 1:
			temp->s6_addr32[0] = entry -> ifindex;
			value = entry -> ifindex;
			break;
		case 2:
			memcpy(temp,&(entry->d_addr),sizeof(struct in6_addr));
			value = (entry -> d_addr).s6_addr32;
			break;
		case 3:
			memcpy(temp,&(entry->s_addr),sizeof(struct in6_addr));
			value = (entry -> s_addr).s6_addr32;
			break;
		case 4:
			temp->s6_addr32[0] = entry -> d_port;
			value = entry -> d_port;
			break;
		case 5:
			temp->s6_addr32[0] = entry -> s_port;
			value = entry -> s_port;
			break;
		default:
			return -2;
	#else
		
		case 0:
			entry -> proto = date;
			break;
		case 1:
			entry -> ifindex = date;
			break;
		case 2:
			entry -> d_port= date;
			break;
		case 3:
			entry -> s_port= date;
			
			entry -> count += 1;
			return -3;
		case 4:
			entry -> d_mask= date;
			break;
		case 5:
			entry -> d_addr= date;
			break;
		case 6:
			entry -> s_mask= date;
			break;
		case 7:
			entry -> s_addr = date;
			entry -> count += 1;
			return -1;
			break;
		default:
			return -2;
	#endif
		
	}
	entry -> count += 1;
	return 0;
}




int list_all_elem_mask_addr(struct date *table,struct entry_info* entry)
{
	if(table == NULL || entry == NULL)
	{
		return -1;
	}

		if(-1 == send_value_to_entry(entry,table->date))
		{
			entry -> opt = table -> opt;
			entry -> flag = table -> opt_para;
			pfm_table_info.opt = pfm_pro_id;/*-1:end*/
			pfm_table_info.opt_para = entry -> opt;
			pfm_table_info.src_port = entry->s_port;
			pfm_table_info.dest_port = entry->d_port;
			pfm_table_info.protocol = entry->proto;
			pfm_table_info.ifindex = entry->ifindex;
			pfm_table_info.src_ipaddr = entry->s_addr;
			pfm_table_info.dest_ipaddr = entry->d_addr;
			pfm_table_info.forward_slot =entry -> flag;
			pfm_table_info.forward_opt = entry->count;
			pfm_table_info.src_ipmask = entry->s_mask;
			pfm_table_info.dest_ipmask = entry->d_mask;
			pfm_record_num_limit++;
			if(pfm_record_num_limit < RECORD_MAX)
				send_netlink_msg(pfm_pid,&pfm_table_info);
			show_entry_table_info(entry);

		}else{
		/*entry->count ++;*/
		list_all_elem_mask_addr((struct date *)table->next_keyword,entry);
		}
			back_value_to_entry(entry);
	
	/*back_value_to_entry(entry);*/
		list_all_elem_mask_addr((struct date *)table->tree->lc,entry);
		list_all_elem_mask_addr((struct date *)table->tree->rc,entry);
	return 0;
			    
}



int list_all_elem(struct date *table,struct entry_info* entry,int is_root)
{
	if(table == NULL || entry == NULL)
	{
		return -1;
	}
	if(is_root == 1)
	{
		if(table ->date == 0)
		{
			if(-3 == send_value_to_entry(entry,0))
			{
				list_all_elem_mask_addr((struct date *)table->next_keyword,entry);
				back_value_to_entry(entry);
			}else{
			list_all_elem((struct date *)table->next_keyword,entry,1);
			back_value_to_entry(entry);
			}
		}
			
		/*list_all_elem((struct date *)table->tree->lc,entry,0);*/
		/*list_all_elem((struct date *)table->tree->rc,entry,0);*/
	}else{
		if(-3 == send_value_to_entry(entry,table->date))
		{
			
			list_all_elem_mask_addr((struct date *)table->next_keyword,entry);

		}else{
		/*entry->count ++;*/
		list_all_elem((struct date *)table->next_keyword,entry,1);
		}
			back_value_to_entry(entry);
	}
	/*back_value_to_entry(entry);*/
		list_all_elem((struct date *)table->tree->lc,entry,0);
		list_all_elem((struct date *)table->tree->rc,entry,0);
	return 0;
			    
}


#endif


/************************************************************
*
*
*                       init and exit func
*
*
*************************************************************/



static int __init pfm_init(void)
{
/*************************netlink init******************************/
#if 0
        netlink_sock = netlink_kernel_create(&init_net,30, 0, (void*)recv_handler,NULL, THIS_MODULE);
#else
        netlink_sock = netlink_kernel_create(&init_net,30, 0, (void*)process_message_thread,NULL, THIS_MODULE);
#endif
        if (!netlink_sock) {
                printk(KERN_ERR"Fail to create netlink socket.\n");
                return 1;
        }
				print_log(KERN_INFO"PFM_KERN:netlink socket creat success\n");
				#if 0
        kernel_thread(process_message_thread, NULL, CLONE_KERNEL);
				print_log(KERN_INFO"PFM_KERN:netlink process_message_thread create success\n");
#endif
/**************************data structure init*****************************/
	#if NEW_PFM_STRUCT
		init_proto_table();
	#else
	forwarding_table_init();

	#endif	
	if(HAVE_RPA)
		cvm_pfm_rx_hook = catch_packet;
	else
		cvm_pfm_rx_hook = NULL;
        return 0;
}

static void __exit pfm_exit(void)
{

cvm_pfm_rx_hook = NULL;
/******************************netlink release*******************************/
    exit_flag = 1;
    wake_up(netlink_sock->sk_sleep);
			print_log(KERN_INFO"PFM_KERN:exit wake_up\n");
    wait_for_completion((struct completion *)&netlink_sock->sk_sleep);
			print_log(KERN_INFO"PFM_KERN:exit wait_for_completion\n");
    sock_release(netlink_sock->sk_socket);
			print_log(KERN_INFO"PFM_KERN:sock_release\n");

/******************************data structure release**************************/
	{
		int i,j;
		for(i = 0;i < MAX_PROTOCOL;i++)
		{
			for(j = 0;j < MAX_PORT;j++)
			{
				if(NULL != forwarding_table[i][j])
				{
					print_log(KERN_INFO"release protocol %d,port %d\n",i,j);
					release_ifindex_dest_ip_tree(forwarding_table[i][j]);
				}
			}
		}
	}	

}






module_init(pfm_init);
module_exit(pfm_exit);
MODULE_LICENSE("GPL");
