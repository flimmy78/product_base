#include <linux/stddef.h>
#if 0
#include <cvmx.h>
#include <cvmx-app-init.h>
#endif
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/inetdevice.h>
#include <asm/unaligned.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/route.h>
#include <net/sock.h>
#include <net/route.h>
#include <linux/miscdevice.h>
#include <linux/in.h>
#include <linux/netfilter_ipv4.h>
#include "aat.h"
#include "aat_ioctl.h"

extern int allif_lock;

int aat_kernel_rx(struct sk_buff *skb){
	struct ethhdr *eth = NULL;	
	struct iphdr *iph = NULL;
	struct udphdr *udph = NULL;
	struct tcphdr *tcph = NULL;
	struct icmphdr *icmph = NULL;
	struct sta_info *sta = NULL;
	int ethlen = sizeof(struct ethhdr);
	int iplen = sizeof(struct iphdr);
	int datalen = 0;
	char *tmp = NULL;

	if (1 == allif_lock)
	{
		if (aat_debug >= AAT_DEBUG)
		{
			printk("%s-%d allif_lock LOCKED, drop\n", __func__, __LINE__);
		}
		return AAT_NEED_DROP_PACKET;
	}
	
	tmp = skb->data;
	eth = (struct ethhdr *)(skb->data);
	if(aat_debug >= AAT_DEBUG)
	printk("%s sta: %02X:%02X:%02X:%02X:%02X:%02X\n",__func__,tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
	if(aat_debug >= AAT_DEBUG)
	printk("%s sta: %02X:%02X:%02X:%02X:%02X:%02X\n",__func__,eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	if(eth->h_proto == 0x0800){
		iph = (struct iphdr *)(skb->data + ethlen);
		if (iph->ihl < 5 || iph->version != 4)
		{		
			printk(KERN_ERR"%s-%d error sta %02X:%02X:%02X:%02X:%02X:%02X ip head ver %d len %d\n", \
						__func__, __LINE__, eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4], \
						eth->h_dest[5],iph->version, iph->ihl);
			return AAT_NEED_DROP_PACKET; /* error packet, need drop!*/
		}
		if(iph->protocol == IPPROTO_UDP){
			udph = (struct udphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_source);
			if(sta == NULL)
			{
				if(aat_debug >= AAT_ERROR)
				{
					printk(KERN_ERR"%s-%d error :udp pkt from %02X:%02X:%02X:%02X:%02X:%02X no sta found\n",__func__, __LINE__,
							eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
				}
				return AAT_NORMAL;
			}
			if(aat_debug >= AAT_DEBUG)	
			{
				printk("aat skb name %s  sta %s\n",skb->dev->name,sta->in_ifname);
			}/*yjl*	
			if (memcmp(sta->in_ifname , skb->dev->name, ETH_LEN))
			{
				return AAT_NORMAL;
			}*/
			if(aat_debug >= AAT_DEBUG)
			printk("aat_kernel_rx  we get sta in udp\n");
			sta->realsip = iph->saddr;
			if(aat_debug >= AAT_DEBUG)
			printk("%s iph->saddr before changed,sta real ip %d.%d.%d.%d.\n", \
						__func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
			memcpy(sta->realdmac, eth->h_dest, MAC_LEN);
			memcpy(eth->h_dest, sta->acmac, MAC_LEN);
			put_unaligned(htonl(sta->staip), &(iph->saddr));
			if(aat_debug >= AAT_DEBUG)
			{
			    printk("%s,%d,sta->realdmac:%02X:%02X:%02X:%02X:%02X:%02X.\n",__func__,__LINE__,sta->realdmac[0],sta->realdmac[1],sta->realdmac[2],sta->realdmac[3],sta->realdmac[4],sta->realdmac[5]);
			    printk("%s,%d,sta->acmac:%02X:%02X:%02X:%02X:%02X:%02X.\n",__func__,__LINE__,sta->acmac[0],sta->acmac[1],sta->acmac[2],sta->acmac[3],sta->acmac[4],sta->acmac[5]);
			    printk("%s sta virtual ip ,sta->staip %d.%d.%d.%d.\n", \
						    __func__, ((sta->staip) >> 24) & 0xFF, ((sta->staip) >> 16) & 0xFF,((sta->staip) >> 8) & 0xFF, (sta->staip) & 0xFF);
			    printk("%s iph->saddr after changed %d.%d.%d.%d.\n", \
						    __func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
			}
			iph->check = 0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			udph->check = 0;
			udph->check = csum_tcpudp_magic(htonl(sta->staip),
							htonl(iph->daddr),
							udph->len,  IPPROTO_UDP,
							csum_partial((unsigned char *)udph, udph->len, 0));
			

		}else if((iph->protocol == IPPROTO_TCP)||(iph->protocol == IPPROTO_IP)){
			if(aat_debug >= AAT_DEBUG)
			printk("aat_kernel_rx iph->ihl %d iplen %d\n",iph->ihl, iplen);
			datalen = skb->len - iph->ihl * 4 - ethlen;
			tcph = (struct tcphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_source);
			if(sta == NULL)
			{
				if(aat_debug >= AAT_ERROR)
				{
					printk(KERN_ERR"%s-%d error :tcp pkt from %02X:%02X:%02X:%02X:%02X:%02X no sta found\n",__func__, __LINE__,
						eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
				}
				
				return AAT_NORMAL;
			}
			if(aat_debug >= AAT_DEBUG)	
			{
				printk("aat skb name %s  sta %s\n",skb->dev->name,sta->in_ifname);
			}/*yjl*	
			if (memcmp(sta->in_ifname , skb->dev->name, ETH_LEN))
			{
				return AAT_NORMAL;
			}*/
			if(aat_debug >= AAT_DEBUG)
			printk("aat_kernel_rx  we get sta in tcp\n");
			sta->realsip = iph->saddr;
			if(aat_debug >= AAT_DEBUG)
			printk("%s iph->saddr before changed,sta real ip %d.%d.%d.%d.\n", \
						__func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
			memcpy(sta->realdmac, eth->h_dest, MAC_LEN);
			memcpy(eth->h_dest, sta->acmac, MAC_LEN);
			put_unaligned(htonl(sta->staip), &(iph->saddr));
			if(aat_debug >= AAT_DEBUG)
			{
			    printk("%s,%d,sta->realdmac:%02X:%02X:%02X:%02X:%02X:%02X.\n",__func__,__LINE__,sta->realdmac[0],sta->realdmac[1],sta->realdmac[2],sta->realdmac[3],sta->realdmac[4],sta->realdmac[5]);
			    printk("%s,%d,sta->acmac:%02X:%02X:%02X:%02X:%02X:%02X.\n",__func__,__LINE__,sta->acmac[0],sta->acmac[1],sta->acmac[2],sta->acmac[3],sta->acmac[4],sta->acmac[5]);
			    printk("%s sta virtual ip ,sta->staip %d.%d.%d.%d.\n", \
						    __func__, ((sta->staip) >> 24) & 0xFF, ((sta->staip) >> 16) & 0xFF,((sta->staip) >> 8) & 0xFF, (sta->staip) & 0xFF);
			    printk("%s iph->saddr after changed %d.%d.%d.%d.\n", \
						    __func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
			}
			iph->check = 0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			tcph->check = 0;
			tcph->check = csum_tcpudp_magic(htonl(iph->saddr),
							htonl(iph->daddr),
							datalen,  iph->protocol,
							csum_partial((unsigned char *)tcph, datalen, 0));
						
		}
		else if(IPPROTO_ICMP == iph->protocol)
		{
			if(aat_debug >= AAT_DEBUG)
			{
				printk("aat_kernel_rx iph->ihl %d iplen %d protocol %d\n",iph->ihl, iplen, iph->protocol);
			}
			datalen = skb->len - iph->ihl * 4 - ethlen;
			icmph = (struct icmphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_source);
			if(sta == NULL)
			{
				if(aat_debug >= AAT_ERROR)
				{
					printk(KERN_ERR"%s-%d error :icmp pkt for %02X:%02X:%02X:%02X:%02X:%02X no sta found\n",__func__, __LINE__,
						eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
				}				
				return AAT_NORMAL;
			}
			if(aat_debug >= AAT_DEBUG)	
			{
				printk("aat skb name %s  sta %s\n",skb->dev->name,sta->in_ifname);
			}/*yjl*	
			if (memcmp(sta->in_ifname , skb->dev->name, ETH_LEN))
			{
				return AAT_NORMAL;
			}*/
			if(aat_debug >= AAT_DEBUG)
			{
				printk("aat_kernel_rx  we get sta in icmp\n");
			}
			if(aat_debug >= AAT_DEBUG)
			{
				printk("%s before transform iph->saddr %d.%d.%d.%d.\n", \
						__func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
				printk("%s before transform sta->staip %d.%d.%d.%d.\n", \
						__func__, ((sta->staip) >> 24) & 0xFF, ((sta->staip) >> 16) & 0xFF,((sta->staip) >> 8) & 0xFF, (sta->staip) & 0xFF);
			}
			sta->realsip = iph->saddr;
			memcpy(sta->realdmac, eth->h_dest, MAC_LEN);
			memcpy(eth->h_dest, sta->acmac, MAC_LEN);
			put_unaligned(htonl(sta->staip), &(iph->saddr));
			
			if(aat_debug >= AAT_DEBUG)
			{
				printk("%s after transform iph->saddr %d.%d.%d.%d.\n", \
						__func__, ((iph->saddr) >> 24) & 0xFF, ((iph->saddr) >> 16) & 0xFF,((iph->saddr) >> 8) & 0xFF, (iph->saddr) & 0xFF);
				printk("%s after transform sta->staip %d.%d.%d.%d.\n", \
						__func__, ((sta->staip) >> 24) & 0xFF, ((sta->staip) >> 16) & 0xFF,((sta->staip) >> 8) & 0xFF, (sta->staip) & 0xFF);
			}
			iph->check = 0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			icmph->checksum = 0;
			icmph->checksum = csum_fold(skb_checksum(skb, ethlen + iplen, datalen, 0));
		}
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		return AAT_NORMAL;
	}

	return AAT_NORMAL;
}

EXPORT_SYMBOL(aat_kernel_rx);



