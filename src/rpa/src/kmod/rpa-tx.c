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
* rpa-tx.c
*
*
* CREATOR:
* autelan.software.bsp&boot. team
*
* DESCRIPTION:
* rpa-ethernet module transmit packets routine
*
*
*******************************************************************************/

#include <linux/stddef.h>
#include <linux/ip.h>
#include <linux/udp.h>
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
#include <linux/mii.h>


#include "rpa.h"
#include "rpa-includes.h"


extern int rpa_tx_debug;
extern int rpa_tx_eth_debug;
extern int rpa_local_slot_num;
extern int first_mcb_slot_num;
extern int second_mcb_slot_num;
extern struct net_device *cvm_oct_device[];
extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
extern rpa_tx_dev_info_t rpa_tx_dev_index[RPA_SLOT_NUM][RPA_SLOT_NUM];

uint8_t  broadcast_dhost[RPA_ENET_ETHER_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


typedef struct  cvm_enet_ether_header {
	uint8_t  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
	uint16_t ether_type;
}cvm_eth_header;
 
typedef struct  cvm_enet_ether_vlantagheader {
	uint8_t  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
	uint32_t  vlantag;
	uint16_t ether_type;
}vlan_eth_header;

typedef struct  cvm_enet_ether_dsatagheader {
	uint8_t  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
	uint8_t  dsatag[RPA_ENET_DSATAG_LEN];
	uint16_t ether_type;
}dsa_eth_header;

extern int dev_refcnt_decrement(atomic_t *v);
extern struct netdev_queue *dev_pick_tx(struct net_device *dev,
					 struct sk_buff *skb);


 void printPacketBuffer(unsigned char *buffer,unsigned long buffLen)
 {
	 unsigned int i;
 
	 if(!buffer)
		 return;
	 printk(":::::::::::::::::::::::::::::::::::::::::::::::\n");
	 
	 for(i = 0;i < buffLen ; i++)
	 {
		 printk("%02x ",buffer[i]);
		 if(0==(i+1)%16) {
			 printk("\n");
		 }
	 }
	 if((buffLen%16)!=0)
	 {
		 printk("\n");
	 }
	 printk(":::::::::::::::::::::::::::::::::::::::::::::::\n");
 }


 /**
  * Display a buffer in hex
  *
  * @param buffer	  Data buffer to display
  * @param buffer_len Length of the buffer
  */
 void hex_dump(const void *buffer, int buffer_len)
 {
	 const uint8_t *ptr = (const uint8_t*)buffer;
	 int i;
	 printk("%p:\n",ptr);
	 for (i=0;i<buffer_len;i++) 
	 {
		 if (i % 16 == 0) 
		 	printk("\n");
		 printk("%02X ", (unsigned int)*ptr);
		 ptr++;
	 }
	 printk("\n");
 }

void skb_dump(char *name, struct sk_buff *skb)
{
	printk(KERN_ERR
		"**********************************************\n"
		"==>%s SKB dump\n"
		"[skb->dev->name]: %s\n"
		"[skb]           : %p\n"
		"[head]          : %p\n"
		"[data]          : %p\n"
		"[tail]          : %d\n"
		"[end]           : %d\n"
		"[mac_header]       : %d\n"
		"[network_header]        : %d\n"
		"[skb->len]      : %d\n"
		"**********************************************\n",
	    name, skb->dev->name, skb, 
	    skb->head,skb->data, skb->tail, skb->end,
		skb->mac_header, skb->network_header,skb->len);

    printk(KERN_ERR"[data]:\n");
      hex_dump(skb->data, skb->len);
    printk(KERN_ERR
		"**********************************************\n");
}

#if 0
/* add DSA_tag to sk_buff
  *
  * @portNum
  * 		is related to dst slot number
  * @devNum
  * 		is related to dst slot number
  *
  * caojia 20110305
  */
void rpa_add_dsa(struct sk_buff *skb, int portNum, int devNum)
{
	int ret;
	int i;
	dsa_eth_header *new_head;
	unsigned char *temp_original_head;
	unsigned char *temp_new_head;
	unsigned int vlantag;
	unsigned short vlanID;
	unsigned char CFI;
	unsigned char UP;

	/*
	CPSS_DXCH_NET_DSA_PARAMS_STC dsaInfor;
	memset(&dsaInfor,0,sizeof(CPSS_DXCH_NET_DSA_PARAMS_STC));
	dsaInfor.commonParams.dsaTagType=CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
	dsaInfor.dsaType = CPSS_DXCH_NET_DSA_CMD_FORWARD_E;
	dsaInfor.dsaInfo.forward.srcDev = srcDev;
	dsaInfor.dsaInfo.forward.dstInterface.devPort.portNum = portNum;
   	dsaInfor.dsaInfo.forward.dstInterface.devPort.devNum = devNum;
	dsaInfor.dsaInfo.forward.egrFilterRegistered = GT_FALSE; // no egress filter

	if(rpa_dev_debug) 
	{
		printk("before expand head\n");
		printPacketBuffer(skb->head, skb->len+(skb->data-skb->head));   
	}

	
	ret = pskb_expand_head(skb,0x10,0,GFP_ATOMIC);
	if(ret < 0)
	{
		printk("skb expand head error.\n");
		return;
	}
	if(rpa_dev_debug) 
	{
		printk("before add dsa\n");
		printPacketBuffer(skb->head, skb->len+(skb->data-skb->head));   
	}

	if(0x8100 != *(unsigned short *)(skb->data + RPA_ENET_ETHER_ADDR_LEN*2))
    {
		cvm_eth_header *original_head;
		original_head = (cvm_eth_header *)skb->data;
		new_head = (dsa_eth_header *)(skb->data -(RPA_ENET_DSATAG_LEN+RPA_ENET_RPATAG_LEN));
		memcpy(new_head->ether_dhost,original_head->ether_dhost,RPA_ENET_ETHER_ADDR_LEN);
		memcpy(new_head->ether_shost, original_head->ether_shost,RPA_ENET_ETHER_ADDR_LEN);
		new_head->ether_type = original_head->ether_type;
		memset(new_head->dsatag,0,RPA_ENET_DSATAG_LEN);
		skb->len = skb->len + (RPA_ENET_DSATAG_LEN+RPA_ENET_RPATAG_LEN);
    }
	else
    {
    	vlan_eth_header *original_head;
    	original_head = (vlan_eth_header *)skb->data;
    	new_head = (dsa_eth_header *)(skb->data - (RPA_ENET_DSATAG_LEN+RPA_ENET_RPATAG_LEN-RPA_ENET_VLANTAG_LEN));
    	temp_original_head = (unsigned char *)original_head;
    	temp_new_head = (unsigned char *)new_head;
		for(i=0; i<12; i++)
		{
			(*temp_new_head++) = (*temp_original_head++);
		}
		vlantag = original_head->vlantag;
		vlanID = vlantag & 0x00000fff;
		CFI = (vlantag>>12) & 0x00000001;
		UP = (vlantag>>13) & 0x00000007;
		dsaInfor.commonParams.vid = vlanID;
		dsaInfor.commonParams.cfiBit = CFI;
		dsaInfor.commonParams.vpt = UP;
		dsaInfor.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
		memset(new_head->dsatag,0,RPA_ENET_DSATAG_LEN);
		new_head->ether_type = original_head->ether_type;
		skb->len = skb->len + (RPA_ENET_DSATAG_LEN+RPA_ENET_RPATAG_LEN-RPA_ENET_VLANTAG_LEN);
    }   
    */

	CPSS_DXCH_NET_DSA_PARAMS_STC dsaInfor;
	memset(&dsaInfor,0,sizeof(CPSS_DXCH_NET_DSA_PARAMS_STC));
	dsaInfor.commonParams.dsaTagType=CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
	dsaInfor.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
	dsaInfor.dsaInfo.fromCpu.dstInterface.devPort.portNum = portNum;
	dsaInfor.dsaInfo.fromCpu.dstInterface.devPort.devNum = devNum;
	dsaInfor.dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE; // no egress filter
	
	ret = pskb_expand_head(skb,0x10,0,GFP_ATOMIC);
	if(ret < 0)
	 {
	  printk("skb expand head error.\n");
	  return;
	 }
	if(rpa_eth_debug) 
	 {
		 printk("before add dsa mixport\n");
		 printPacketBuffer(skb->data, skb->len);   
	 }
	
	if(0x8100 != *(unsigned short *)(skb->data + 12))
	 {
		  cvm_eth_header *original_head;
		  original_head = (cvm_eth_header *)skb->data;
		  new_head = (dsa_eth_header *)(skb->data -16);
		  memcpy(new_head->ether_dhost,original_head->ether_dhost,CVM_ENET_ETHER_ADDR_LEN);
		  memcpy(new_head->ether_shost, original_head->ether_shost,CVM_ENET_ETHER_ADDR_LEN);
		  new_head->ether_type = original_head->ether_type;
		  memset(new_head->dsatag,0,CVM_ENET_DSATAG_LEN);
		  skb->len = skb->len + 16;
	 }
	else
	 {
	  vlan_eth_header *original_head;
	  original_head = (vlan_eth_header *)skb->data;
	  new_head = (dsa_eth_header *)(skb->data -12);
	  temp_original_head = (unsigned char *)original_head;
	  temp_new_head = (unsigned char *)new_head;
	  for(i=0; i<12; i++)
	   {
		 (*temp_new_head++) = (*temp_original_head++);
	   }
	  vlantag = original_head->vlantag;
	  vlanID = vlantag & 0x00000fff;
	  CFI = (vlantag>>12) & 0x00000001;
	  UP = (vlantag>>13) & 0x00000007;
	  dsaInfor.commonParams.vid = vlanID;
	  dsaInfor.commonParams.cfiBit = CFI;
	  dsaInfor.commonParams.vpt = UP;
	  dsaInfor.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_TRUE;
	  memset(new_head->dsatag,0,CVM_ENET_DSATAG_LEN);
	  new_head->ether_type = original_head->ether_type;
	  skb->len = skb->len + 12;
	 }	 


	ret = cpssDxChNetIfDsaTagBuild(&dsaInfor,new_head->dsatag);
	if(ret != GT_OK)
    {
    	printk("DSA tag build error!\r\n");
    }
	skb->data = (unsigned char *)new_head;
	if(rpa_eth_debug && 0) 
   	{
   		printk("after add dsa\n");
   		printPacketBuffer(skb->head, skb->len+(skb->data-skb->head));
   	}

	return ;
}
#endif

/**
 * Description:
 *  rpa xmit function.
 */
int rpa_send_xmit_with_vlan(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag, unsigned short vid)
{
	rpa_dev_private_t *rpa_priv = (rpa_dev_private_t *)netdev_priv(netdev);
	struct net_device *tx_netdev = rpa_tx_dev_index[rpa_priv->sslotNum][rpa_priv->dslotNum].tx_dev; /*the device which send the package actually*/
	cvm_eth_header *original_head;
	rpa_eth_header_with_vlan *rpa_head;
	struct netdev_queue *txq;
	int cpu = smp_processor_id();
	int ret;

	if (netdevNum >= RPA_NETDEV_NUM) {
		rpa_info("error netdevNum(%d), exit rpa send function!\n", netdevNum);
		goto out_kfree_skb;
	}

	if (tx_netdev && (tx_netdev->flags & IFF_UP)) {
		skb->dev = tx_netdev;
	}
	else {
		if (rpa_tx_debug) {
			rpa_info("tx_netdev not exist or the interface is not up.\n");
		}

		goto out_kfree_skb;
	}

	if(rpa_tx_eth_debug) {
		rpa_info("before add rpa tag\n");
		printPacketBuffer(skb->data, skb->len);   
	}

	/* add rpa tag */
	ret = pskb_expand_head(skb, 0x16, 0, GFP_ATOMIC);
	if(ret < 0) {
		rpa_info("skb expand head error.\n");

		goto out_kfree_skb;
	}

	original_head = (cvm_eth_header *)skb->data;
	rpa_head = (rpa_eth_header_with_vlan *)(skb->data - 22);
	memcpy(rpa_head->ether_dhost, rpa_tx_dev_index[rpa_priv->sslotNum][rpa_priv->dslotNum].dest_mac_addr, RPA_ENET_ETHER_ADDR_LEN);
	memcpy(rpa_head->ether_shost, rpa_tx_dev_index[rpa_local_slot_num][rpa_local_slot_num].dest_mac_addr, RPA_ENET_ETHER_ADDR_LEN);
	rpa_head->vlan_ether_type = 0x8100;
	rpa_head->vlan_id = vid;
	rpa_head->cookie = RPA_COOKIE;
	skb->len = skb->len + 22;
	skb->data = (unsigned char *)rpa_head;

	/* add other rpa private information */
	/* type: 1 for packet, 0 for other packet */
	rpa_head->type &= 0x0;
	if (flag == RPA_ENTER_STACK_ENABLE) {
		rpa_head->type |= 0x1;
	}
	
	rpa_head->dnetdevNum = rpa_priv->dnetdevNum;
	rpa_head->snetdevNum = netdevNum;
	rpa_head->d_s_slotNum = (((rpa_priv->dslotNum << RPA_DST_SLOT) & RPA_DST_SLOT_MASK) | (rpa_priv->sslotNum & RPA_SRC_SLOT_MASK));	

	if(rpa_tx_eth_debug) {
   		rpa_info("after add rpa\n");
   		printPacketBuffer(skb->data, skb->len);
		rpa_info("TX_DEVICE : %s , LENGTH : %d\n", tx_netdev->name, skb->len);
   	}

	txq = dev_pick_tx(tx_netdev, skb);
	if (txq->xmit_lock_owner != cpu) {
		spin_lock(&txq->_xmit_lock);
		txq->xmit_lock_owner = cpu;
	
		ret = tx_netdev->netdev_ops->ndo_start_xmit(skb, skb->dev);
		if (ret == NETDEV_TX_OK) {
			//printk("cvm_oct_xmit_v success!");
			txq->xmit_lock_owner = -1;
			spin_unlock(&txq->_xmit_lock);
			
			goto delivered;
		}
		else {
			rpa_err("rpa send failed. actual send dev : %s, error return value : %d\n", tx_netdev->name, ret);
			txq->xmit_lock_owner = -1;
			spin_unlock(&txq->_xmit_lock);
		}
	}
	else {
		rpa_crit("RPA : Dead loop on virtual device "
			       "%s.\n", skb->dev->name);
	}

out_kfree_skb:
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;
	
	return 1;

delivered:
	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += skb->len;

	return 0;
}

int rpa_xmit_with_4094_vlan(struct sk_buff *skb, struct net_device *netdev)
{
	int result;

	result = rpa_send_xmit_with_vlan(skb, netdev, 0, 0, 0xffe);

	return 0;
}

int rpa_xmit_with_4093_vlan(struct sk_buff *skb, struct net_device *netdev)
{
	int result;

	result = rpa_send_xmit_with_vlan(skb, netdev, 0, 0, 0xffd);

	return 0;
}

/**
 * Description:
 *  rpa xmit function.
 */
int rpa_send_xmit(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag)
{
	rpa_dev_private_t *rpa_priv = (rpa_dev_private_t *)netdev_priv(netdev);
	struct net_device *tx_netdev = rpa_tx_dev_index[rpa_priv->sslotNum][rpa_priv->dslotNum].tx_dev; /*the device which send the package actually*/
	cvm_eth_header *original_head;
	rpa_eth_header *rpa_head;
	struct netdev_queue *txq;
	int cpu = smp_processor_id();
	int ret;

	if (netdevNum >= RPA_NETDEV_NUM) {
		rpa_info("error netdevNum(%d), exit rpa send function!\n", netdevNum);
		goto out_kfree_skb;
	}

	if (tx_netdev && (tx_netdev->flags & IFF_UP)) {
		skb->dev = tx_netdev;
	}
	else {
		if (rpa_tx_debug) {
			rpa_info("tx_netdev not exist or the interface is not up.\n");
		}

		goto out_kfree_skb;
	}

	if(rpa_tx_eth_debug) {
		rpa_info("before add rpa tag\n");
		printPacketBuffer(skb->data, skb->len);   
	}

	/* add rpa tag */
	ret = pskb_expand_head(skb, 0x12, 0, GFP_ATOMIC);
	if(ret < 0) {
		rpa_info("skb expand head error.\n");

		goto out_kfree_skb;
	}

	original_head = (cvm_eth_header *)skb->data;
	rpa_head = (rpa_eth_header *)(skb->data - RPA_ENET_RPATAG_LEN);
	memcpy(rpa_head->ether_dhost, rpa_tx_dev_index[rpa_priv->sslotNum][rpa_priv->dslotNum].dest_mac_addr, RPA_ENET_ETHER_ADDR_LEN);
	memcpy(rpa_head->ether_shost, rpa_tx_dev_index[rpa_local_slot_num][rpa_local_slot_num].dest_mac_addr, RPA_ENET_ETHER_ADDR_LEN);
	rpa_head->cookie = RPA_COOKIE;
	skb->len = skb->len + RPA_ENET_RPATAG_LEN;
	skb->data = (unsigned char *)rpa_head;

	/* add other rpa private information */
	/* type: 1 for packet, 0 for other packet */
	rpa_head->type &= 0x0;
	if (flag == RPA_ENTER_STACK_ENABLE) {
		rpa_head->type |= 0x1;
	}
	
	rpa_head->dnetdevNum = rpa_priv->dnetdevNum;
	rpa_head->snetdevNum = netdevNum;
	rpa_head->d_s_slotNum = (((rpa_priv->dslotNum << RPA_DST_SLOT) & RPA_DST_SLOT_MASK) | (rpa_priv->sslotNum & RPA_SRC_SLOT_MASK));	

	if(rpa_tx_eth_debug) {
   		rpa_info("after add rpa\n");
   		printPacketBuffer(skb->data, skb->len);
		rpa_info("TX_DEVICE : %s , LENGTH : %d\n", tx_netdev->name, skb->len);
   	}

	txq = dev_pick_tx(tx_netdev, skb);
	if (txq->xmit_lock_owner != cpu) {
		spin_lock(&txq->_xmit_lock);
		txq->xmit_lock_owner = cpu;
	
		ret = tx_netdev->netdev_ops->ndo_start_xmit(skb, skb->dev);
		if (ret == NETDEV_TX_OK) {
			//printk("cvm_oct_xmit_v success!");
			txq->xmit_lock_owner = -1;
			spin_unlock(&txq->_xmit_lock);
			
			goto delivered;
		}
		else {
			rpa_err("rpa send failed. actual send dev : %s, error return value : %d\n", tx_netdev->name, ret);
			txq->xmit_lock_owner = -1;
			spin_unlock(&txq->_xmit_lock);
		}
	}
	else {
		rpa_crit("RPA : Dead loop on virtual device "
			       "%s.\n", skb->dev->name);
	}

out_kfree_skb:
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;
	
	return 1;

delivered:
	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += skb->len;

	return 0;
}

int rpa_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	int result;
	int default_portNum = 0;
	int default_flag = 0;

	result = rpa_send_xmit(skb, netdev, default_portNum, default_flag);

	return 0;
}

int rpa_send_xmit_ax81_1x12g12s(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag)
{
	rpa_dev_private_t *rpa_priv = (rpa_dev_private_t *)netdev_priv(netdev);
	if (rpa_priv->dslotNum == first_mcb_slot_num || 
		rpa_priv->dslotNum == second_mcb_slot_num) {
		return rpa_send_xmit(skb, netdev, netdevNum, flag);
	}
	else {
		return rpa_send_xmit_with_vlan(skb, netdev, netdevNum, flag, 0xffe);
	}
}

int rpa_send_xmit_ax71_crsmu(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag)
{
	return rpa_send_xmit_with_vlan(skb, netdev, netdevNum, flag, 0xffd);
}

