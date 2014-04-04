#include <linux/stddef.h>
//#include <cvmx.h>
//#include <cvmx-app-init.h>
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

int aat_kernel_tx(struct sk_buff *skb){
	struct ethhdr *eth;	
	struct iphdr *iph;
	struct udphdr *udph;
	struct tcphdr *tcph;
	struct icmphdr *icmph = NULL;
	struct sta_info *sta = NULL;
	int ethlen = sizeof(struct ethhdr);
	int iplen = sizeof(struct iphdr);
	/*
	int udplen = sizeof(struct udphdr);
	int tcplen = sizeof(struct tcphdr);
	int len = skb->len;
	*/
	int datalen = 0;
	eth = (struct ethhdr *)(skb->data);
	if(aat_debug >= AAT_DEBUG)
	printk("%s sta: %02X:%02X:%02X:%02X:%02X:%02X\n",__func__,eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
	if(eth->h_proto == 0x0800){
		iph = (struct iphdr *)(skb->data + ethlen);	
		if (iph->ihl < 5 || iph->version != 4)
		{
			printk(KERN_ERR"[aat tx]sta %02x:%02x:%02x:%02x:%02x:%02x ip head ver %d len %d err\n", \
						eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4], \
						eth->h_dest[5],iph->version, iph->ihl);
			return AAT_NEED_DROP_PACKET; /* error packet, need drop!*/
		}
		if(iph->protocol == IPPROTO_UDP){
			udph = (struct udphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_dest);
			if(sta == NULL)
				return AAT_NORMAL;
			if(aat_debug >= AAT_DEBUG)
			printk("aat_kernel_tx  we get sta in udp\n");
			memcpy(eth->h_source, sta->realdmac, MAC_LEN);
			put_unaligned(htonl(sta->realsip), &(iph->daddr));
			iph->check	  =  0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			udph->check = 0;
			udph->check = csum_tcpudp_magic(htonl(iph->saddr),
							htonl(iph->daddr),
							udph->len,  IPPROTO_UDP,
							csum_partial((unsigned char *)udph, udph->len, 0));
			

		}else if((iph->protocol == IPPROTO_TCP)||(iph->protocol == IPPROTO_IP)){
			datalen = skb->len - iph->ihl * 4 - ethlen;
			tcph = (struct tcphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_dest);
			if(sta == NULL)
				return AAT_NORMAL;
			if(aat_debug >= AAT_DEBUG)
			printk("aat_kernel_tx  we get sta in tcp\n");
			memcpy(eth->h_source, sta->realdmac, MAC_LEN);
			put_unaligned(htonl(sta->realsip), &(iph->daddr));
			iph->check	  =  0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			tcph->check = 0;
			tcph->check = csum_tcpudp_magic(htonl(iph->saddr),
							htonl(iph->daddr),
							datalen,  IPPROTO_TCP,
							csum_partial((unsigned char *)tcph, datalen, 0));
			
			
		}
		else if(IPPROTO_ICMP == iph->protocol)
		{
			datalen = skb->len - iph->ihl * 4 - ethlen;
			icmph = (struct icmphdr *)(skb->data + ethlen + iplen);
			sta = aat_get_sta(&allif, eth->h_dest);
			if(sta == NULL)
				return AAT_NORMAL;
			if(aat_debug >= AAT_DEBUG)
			{
				printk("aat_kernel_tx  we get sta in icmp\n");
			}
			if(aat_debug >= AAT_DEBUG)
			{
				printk("%s before transform iph->daddr %d.%d.%d.%d.\n", \
						__func__, ((iph->daddr) >> 24) & 0xFF, ((iph->daddr) >> 16) & 0xFF,((iph->daddr) >> 8) & 0xFF, (iph->daddr) & 0xFF);
				printk("%s before transform sta->realsip %d.%d.%d.%d.\n", \
						__func__, ((sta->realsip) >> 24) & 0xFF, ((sta->realsip) >> 16) & 0xFF,((sta->realsip) >> 8) & 0xFF, (sta->realsip) & 0xFF);
			}
			memcpy(eth->h_source, sta->realdmac, MAC_LEN);
			put_unaligned(htonl(sta->realsip), &(iph->daddr));

			if(aat_debug >= AAT_DEBUG)
			{
				printk("%s after transform iph->daddr %d.%d.%d.%d.\n", \
						__func__, ((iph->daddr) >> 24) & 0xFF, ((iph->daddr) >> 16) & 0xFF,((iph->daddr) >> 8) & 0xFF, (iph->daddr) & 0xFF);
				printk("%s after transform sta->realsip %d.%d.%d.%d.\n", \
						__func__, ((sta->realsip) >> 24) & 0xFF, ((sta->realsip) >> 16) & 0xFF,((sta->realsip) >> 8) & 0xFF, (sta->realsip) & 0xFF);
			}
			
			iph->check	  =  0;
			iph->check	  = ip_fast_csum((unsigned char *)iph, iph->ihl);
			icmph->checksum = 0;
			icmph->checksum = csum_fold(skb_checksum(skb, ethlen + iplen, datalen, 0));			
		}
	}
	return AAT_NORMAL;
}

EXPORT_SYMBOL(aat_kernel_tx);
