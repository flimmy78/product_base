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
*kap.c
*
*CREATOR:
*	<zhubo@autelan.com>
*
*DESCRIPTION:
*     device driver for kap
*
*DATE:
*	02/24/2009	
*
*  FILE REVISION NUMBER:
*       $Revision: 1.13 $
*
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#define DRV_NAME	"kap"
#define DRV_VERSION	"1.6"
#define DRV_DESCRIPTION	"Universal KAP device driver"
#define DRV_COPYRIGHT	"(C) 2007 -2008 zhubo "

#define VNIC_PHYIF_MAX 1
#define MAX_DEV_NO (2*4096+500)  /* max ifindex in system */
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include <linux/miscdevice.h>
#include <linux/ethtool.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/crc32.h>
#include <linux/in.h>
#include <linux/ctype.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/signal.h>   /* for SIGIO in sdk2.2 */
#include <linux/sched.h>   /* for TASK_INTERRUPTIBLE in sdk2.2 */
#include <net/net_namespace.h>  /* for struct net init_net in sdk2.2 */
#include <linux/smp_lock.h>   /* for lock_kernel in sdk2.2 */

#include "if_kap.h"
#if 0
#include "cvmx-app-init.h"

extern cvmx_bootinfo_t *octeon_bootinfo;
#endif
extern int mac_addr_get(unsigned char * mac);
extern struct net init_net;

#ifdef KAP_DEBUG
static int kap_debug = 0;
module_param(kap_debug, int, 0644);
MODULE_PARM_DESC(kap_debug,"Enable debug messages.");
#endif

#define KAP_MINOR 210

/* Network device part of the driver */
//extern int printk_console_no_output = 0;
//static LIST_HEAD(kap_dev_list);
//static struct ethtool_ops kap_ethtool_ops;
static struct semaphore kapReadPacketSem;


/* Net device open. */
static int kap_net_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

/* Net device close. */
static int kap_net_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

/* Net device start xmit */
static int kap_net_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct kap_struct *kap = (struct kap_struct *)dev->priv;
	
	DBG(KERN_INFO "%s:: %s kap_net_xmit %d\n",__func__,dev->name, skb->len);

	/* Drop packet if interface is not attached 
	if (!kap->attached)
		goto drop*/;

	/* Packet dropping */
	if (skb_queue_len(&kap->readq) >= dev->tx_queue_len) {
		if (!(kap->flags & KAP_ONE_QUEUE)) {
			/* Normal queueing mode. */
			/* Packet scheduler handles dropping of further packets. */
			netif_stop_queue(dev);

			/* We won't see all dropped packets individually, so overrun
			 * error is more appropriate. */
			kap->stats.tx_fifo_errors++;
		} else {
			/* Single queue mode.
			 * Driver handles dropping of all packets itself. */
			goto drop;
		}
	}

	/* Queue packet */
	skb_queue_tail(&kap->readq, skb);
	dev->trans_start = jiffies;

	/*ready*/
	/* Notify and wake up reader process */
	if (kap->flags & KAP_FASYNC)
		kill_fasync(&kap->fasync, SIGIO, POLL_IN);
	wake_up_interruptible(&kap->read_wait);

	up(&kapReadPacketSem);

	return 0;

drop:
	kap->stats.tx_dropped++;
	kfree_skb(skb);
	return 0;
}


static void kap_net_mclist(struct net_device *dev)
{
#ifdef KAP_DEBUG
	//struct kap_struct *kap = (struct kap_struct *)dev->priv;

	DBG(KERN_INFO "%s:: %s kap_net_mclist\n", __func__,dev->name);
#endif

	/* Nothing to do for multicast filters. 
	 * We always accept all frames.
	 */
	return;
}


static struct net_device_stats *kap_net_stats(struct net_device *dev)
{
	struct kap_struct *kap = (struct kap_struct *)dev->priv;
	return &kap->stats;
}

/* Initialize net device. */
static void kap_net_init(struct net_device *dev)
{
	/* add for sdk2.2 */
    struct  net_device_ops  *dev_ops;
    dev_ops  =  dev->netdev_ops;
	struct kap_struct *kap = (struct kap_struct *)dev->priv;
   
	switch (kap->flags & KAP_TYPE_MASK) {
	case KAP_TUN_DEV:
		/* Point-to-Point KAP Device */
		dev->hard_header_len = 0;
		dev->addr_len = 0;
		dev->mtu = 1500;

		/* Zero header length */
		dev->type = ARPHRD_NONE; 
		dev->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
		dev->tx_queue_len = KAP_READQ_SIZE;  /* We prefer our own queue length */
		break;

	case KAP_TAP_DEV:
		/* Ethernet TAP Device */
		dev_ops->ndo_set_multicast_list = kap_net_mclist;

		ether_setup(dev);
		#if 0
		random_ether_addr(dev->dev_addr);
		#endif
		dev->dev_addr[0] = kap->if_mac_addr[0];
		dev->dev_addr[1] = kap->if_mac_addr[1];
		dev->dev_addr[2] = kap->if_mac_addr[2];
		dev->dev_addr[3] = kap->if_mac_addr[3];
		dev->dev_addr[4] = kap->if_mac_addr[4];
		dev->dev_addr[5] = kap->if_mac_addr[5];
		
		dev->tx_queue_len = KAP_READQ_SIZE;  /* We prefer our own queue length */
		break;
	}
}
/* Uninitialize net device. */
static void kap_net_uninit(struct net_device *dev)
{
    struct  net_device_ops  *dev_ops;
    dev_ops  =  dev->netdev_ops;	
	struct kap_struct *kap = (struct kap_struct *)dev->priv;
   
	switch (kap->flags & KAP_TYPE_MASK) {
	case KAP_TUN_DEV:
		/* Point-to-Point KAP Device */
		dev->hard_header_len = 0;
		dev->addr_len = 0;
		dev->mtu = 0;

		/* Zero header length */
		dev->type = 0; 
		dev->flags = 0;
		dev->tx_queue_len = 0;  /* We prefer our own queue length */
		break;

	case KAP_TAP_DEV:
		/* Ethernet TAP Device */
		dev_ops->ndo_set_multicast_list = NULL;

		ether_unsetup(dev);
		#if 0
		random_ether_addr(dev->dev_addr);
		#endif
		memset(dev->dev_addr, 0, 6);
		
		dev->tx_queue_len = 0;  /* We prefer our own queue length */
		break;
	}
}

/* Character device part */

/* Poll  
*	modify
*/
static __inline__ struct user_netdevice* kap_get_by_name(struct kap_struct* kap,const char *name)
{
	struct user_netdevice* udev;

	ASSERT_RTNL();
	list_for_each_entry(udev, &kap->dev_list, list) {
		if (!strncmp(udev->dev->name, name, IFNAMSIZ))
		    return udev;
	}

	return NULL;
}

static __inline__ struct user_netdevice * kap_get_by_interfaceId(struct kap_struct* kap,unsigned int if_index)
{	
	struct user_netdevice* udev;

	ASSERT_RTNL();
	list_for_each_entry(udev, &kap->dev_list, list) {
		if (udev->l3_index == if_index)
		    return udev;
	}

	return NULL;
}

static unsigned int kap_chr_poll(struct file *file, poll_table * wait)
{  
	struct kap_struct *kap = (struct kap_struct *)file->private_data;
	unsigned int mask = POLLOUT | POLLWRNORM;

	if (!kap)
			return -EBADFD;

	//DBG(KERN_INFO "%s: kap_chr_poll\n", kap->dev->name);

	poll_wait(file, &kap->read_wait, wait);
 
	if (!skb_queue_empty(&kap->readq))
			mask |= POLLIN | POLLRDNORM;

	return mask;
}

// print packet buffer( op - 1 for tx, 0 for rx)
static void print_skb_info(struct sk_buff* skb, unsigned char op) 
{
	unsigned char* ptr1;
	char ch,*dataOrigin,dataBuf[128] ={0};
	int cbyte=0,tmp=0,len;

	if(op) {
		DBG(KERN_INFO "KERN::::::::::::::::::::::::kap tx packet detail::::::::::::::::::::::::::::\r\n");
	}
	else {
		DBG(KERN_INFO "KERN------------------------kap rx packet detail----------------------------\r\n");	
	}

	if(skb)
	{
		if(skb->data)
		{
			len = skb->len;
			ptr1 = skb->data;
			dataOrigin = dataBuf;
			while((len-cbyte)>= 16)
			{
				dataOrigin += sprintf(dataOrigin,"[ ");
				for(tmp = 0; tmp<16;tmp++)
					dataOrigin += sprintf(dataOrigin,"%02x ",ptr1[cbyte+tmp]);
				dataOrigin += sprintf(dataOrigin,"]\t[ ");
				for(tmp=0;tmp<16;tmp++)
				{
					ch = ptr1[cbyte+tmp];
					if(isalnum(ch))
						dataOrigin += sprintf(dataOrigin,"%c",ch);
					else
						dataOrigin += sprintf(dataOrigin,".");
				}
				dataOrigin += sprintf(dataOrigin," ]\r\n");
				DBG(KERN_INFO"%s",dataBuf);
				memset(dataBuf,0,128);
				cbyte += 16;	
				dataOrigin = dataBuf;
			}
			if(len>cbyte)
			{
				tmp = cbyte;
				dataOrigin += sprintf(dataOrigin,"[ ");
				while(tmp<len)
				dataOrigin += sprintf(dataOrigin,"%02x ",ptr1[tmp++]);
				dataOrigin += sprintf(dataOrigin,"]");
				tmp = len - cbyte;
				tmp = 16 - tmp;
				while(tmp--)
					dataOrigin += sprintf(dataOrigin,"   ");
				dataOrigin += sprintf(dataOrigin,"\t[ ");
				tmp = cbyte;
				while(tmp<len)
				{
					ch = ptr1[tmp++];
					if(isalnum(ch))
						dataOrigin += sprintf(dataOrigin,"%c",ch);
					else
						dataOrigin += sprintf(dataOrigin,".");

				}
				dataOrigin += sprintf(dataOrigin," ]\r\n");
				DBG(KERN_INFO"%s",dataBuf);
				memset(dataBuf,0,128);
			}
		}
	}
	else
	{
		DBG(KERN_INFO "packet info null!\r\n");
	}
	
	if(op){
		DBG(KERN_INFO "KERN::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\r\n");
	}
	else {
		DBG(KERN_INFO "KERN------------------------------------------------------------------------\r\n");
	}
	
}
/* Get packet from user space buffer */
static __inline__ ssize_t kap_get_user(struct kap_struct *kap,struct user_netdevice* udev,struct data_struct* data)
{
	struct kap_pi pi = { 0, __constant_htons(ETH_P_IP) };
	struct sk_buff *skb;
	//size_t len = iv->iov_len, align = 0;
	unsigned int len = data->data_len,align = 0;

	if (!(kap->flags & KAP_NO_PI)) {
		DBG(KERN_INFO"%s :: HASN'T CONFIG KAP_NO_PI \n",__func__);
		if ((len -= sizeof(pi)) > data->data_len)
		{
			DBG(KERN_INFO"%s :: len ERROR\n",__func__);
			return -EINVAL;
		}
		if(copy_from_user((void *)&pi, data->data_addr, sizeof(pi)))
			return -EFAULT;
	}

	if ((kap->flags & KAP_TYPE_MASK) == KAP_TAP_DEV)
		align = NET_IP_ALIGN;

 	DBG(KERN_INFO"%s :: packet len %d ,align %d\n",__func__,len,align);
	if (!(skb = alloc_skb(len + align, GFP_KERNEL))) {
		kap->stats.rx_dropped++;
		DBG(KERN_INFO"%s :: NO MEM TO ALLOC\n",__func__);
		return -ENOMEM;
	}

	if (align)
		skb_reserve(skb, align);
	if (copy_from_user(skb_put(skb, len), data->data_addr, len)) {
		kap->stats.rx_dropped++;
		kfree_skb(skb);
		DBG(KERN_INFO"%s :: copy from user ERROR\n",__func__);
		return -EFAULT;
	}

#ifdef KAP_DEBUG
	DBG(KERN_INFO"%s :: dev name %s\n",__func__,udev->dev->name);
	print_skb_info(skb, 0);
#endif
	
	skb->dev = udev->dev;
	switch (kap->flags & KAP_TYPE_MASK) {
	case KAP_TUN_DEV:
/* by qinhs@autelan.com for XCAT kernel version 2.6.22.18 support */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22) 
		skb->mac_header = skb->data;
#else
		skb->mac.raw = skb->data;
#endif
		skb->protocol = pi.proto;
		break;
	case KAP_TAP_DEV:
		skb->protocol = eth_type_trans(skb, udev->dev);
		break;
	};

	if (kap->flags & KAP_NOCHECKSUM)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
 
	netif_rx_ni(skb);
	udev->dev->last_rx = jiffies;
   
	kap->stats.rx_packets++;
	kap->stats.rx_bytes += len;

	return len;
} 


/* Writev */
static ssize_t kap_chr_write(struct file * file, const char __user * buf, 
			     size_t count, loff_t *pos)
{
	/*code optimize: Derference before null check
	zhangcl@autelan.com 2013-1-21*/
	struct kap_struct *kap = NULL;	
	struct user_netdevice* udev = NULL;
	struct data_struct data;
	//struct iovec iv;
	//unsigned int ifId;
    if(NULL == file||NULL == buf||NULL == pos){
		return -EINVAL;
    }
	kap =file->private_data; 
	void __user* argp = (void __user*)buf;

	if(copy_from_user(&data,argp,sizeof data))
		return -EINVAL;

	udev = kap->udev_index[data.l3_index];
	if (!udev)
	{
		DBG(KERN_INFO"%s ::no net device found for ifindex %d\n",__func__,data.l3_index);
		return -EBADFD;
	}


	DBG(KERN_INFO"%s :: dev name %s, dev l3_index  %d,data's len  %d \n",		\
		__func__,udev->dev->name,udev->l3_index,count); 

	//iv.iov_base = (unsigned char*)data.data_addr;
	//iv.iov_len = data.data_len;

	//DBG(KERN_INFO "%s: iv.iov_base = %p\n", udev->dev->name,iv.iov_base);

	//return kap_get_user(kap, &iv, iov_total(&iv, count));
	return kap_get_user(kap,udev,&data);
}


/* Put packet to the user space buffer */
static __inline__ ssize_t kap_put_user(struct kap_struct *kap,
				       struct sk_buff *skb,
				       struct data_struct *data,
				       int len)
{
	struct kap_pi pi = { 0, skb->protocol };
	ssize_t total = 0;
	unsigned char space[64] = {0};
	int spacelen = 0,enlen = 64;

	if (!(kap->flags & KAP_NO_PI)) {
		if ((len -= sizeof(pi)) < 0)
			return -EINVAL;

		if (len < skb->len) {
			/* Packet will be striped */
			pi.flags |= KAP_PKT_STRIP;
		}
 
		if (copy_to_user(data->data_addr, (void *) &pi, sizeof(pi)))
			return -EFAULT;
		
		total += sizeof(pi);
		data->data_addr += sizeof(pi);
	}       

	len = min_t(int, skb->len, len);
	DBG(KERN_INFO"%s :: kernel skb buffer len  = %d \n",__func__,len);

	if(copy_to_user(data->data_addr,skb->data,len))
		return -EFAULT;

	/*
	 *	 packet len must reach 64
	*/
	if(len < enlen)
	{
	 	spacelen = enlen - len;	
		if(copy_to_user(data->data_addr+len,space,spacelen))
			return -EFAULT;
	}

	total = len + spacelen;
	
#ifdef KAP_DEBUG
	DBG(KERN_INFO"%s :: user buffer len  = %d \n",__func__,total);	
	print_skb_info(skb, 1);
#endif 

	kap->stats.tx_packets++;
	kap->stats.tx_bytes += len;

	return total;
}

/* Readv */
static ssize_t kap_chr_read(struct file * file, char __user * buf, 
			    size_t count, loff_t *pos)
{
	/*code optimize: Derference before null check
	zhangcl@autelan.com 2013-1-21*/
	struct kap_struct *kap = NULL;
	struct user_netdevice* udev;
	struct data_struct data;
	struct sk_buff *skb;
	unsigned int len , ret = 0;
    if(NULL == file||NULL == buf||NULL == pos)
	{
		return -EINVAL;
    }
	kap = file->private_data;
	void __user * argp = (void __user*)buf;
	
	if(copy_from_user(&data,argp,sizeof data))
		return -EINVAL;
	
	DECLARE_WAITQUEUE(wait, current);

	len = data.data_len;
	/*code optimize:  Unsigned compared against 0
	zhangcl@autelan.com 2013-1-21*/
	#if 0
	if (len < 0)
		return -EINVAL;
	#endif
	
	add_wait_queue(&kap->read_wait, &wait);
	while (len) 
	{

		current->state = TASK_INTERRUPTIBLE;

		/* Read frames from the queue */
		if (!(skb=skb_dequeue(&kap->readq))) 
		{
			if (file->f_flags & O_NONBLOCK) 
			{
				ret = -EAGAIN;
				break;
			}
			if (signal_pending(current)) 
			{
				ret = -ERESTARTSYS;
				break;
			}

			/* Nothing to read, let's sleep */
			schedule();
			continue;
		}
		udev = kap->udev_index[skb->dev->ifindex];
		if(udev) 
		{
			netif_wake_queue(udev->dev);
			ret = kap_put_user(kap, skb,&data,len);
			if(ret > 0)
			{
				data.data_len = ret;
				data.l2_index = udev->l2_index;
				data.vId = udev->vId;
				data.l3_index = udev->l3_index;
				data.dev_type = udev->dev_type;
				DBG(KERN_INFO"%s :: %s send data on intf %#0x vlan %d index %#0x\n",	\
								__func__,skb->dev->name ,udev->l3_index,udev->vId,udev->l2_index);
				copy_to_user(argp,&data,sizeof(struct data_struct));	
			}
		}
		kfree_skb(skb);
		break;
	}

	current->state = TASK_RUNNING;
	remove_wait_queue(&kap->read_wait, &wait);

	return ret;
}

static const struct net_device_ops kap_netdev_ops = {
    .ndo_open            = kap_net_open,
    .ndo_stop            = kap_net_close,
    .ndo_start_xmit      = kap_net_xmit,
	.ndo_get_stats       = kap_net_stats,
};

static void kap_setup(struct net_device *dev)
{
	/*struct kap_struct *kap = netdev_priv(dev);

	skb_queue_head_init(&kap->readq);
	init_waitqueue_head(&kap->read_wait);

	kap->owner = -1;
	*/	
	/*printk(KERN_ERR "run to %s \n",__func__);	*/
	SET_MODULE_OWNER(dev);
    dev->netdev_ops      = &kap_netdev_ops;
	dev->destructor      = free_netdev;
	/*dev->ethtool_ops = &kap_ethtool_ops;*/
	
}

static void kap_unsetup(struct net_device *dev)
{
	/*struct kap_struct *kap = netdev_priv(dev);

	skb_queue_head_init(&kap->readq);
	init_waitqueue_head(&kap->read_wait);

	kap->owner = -1;
	*/
    dev->netdev_ops      = NULL;
	dev->destructor      = NULL;
	/*dev->ethtool_ops = NULL;*/	
}

/*change link up down*/
static int set_dev_flags(struct net_device* dev,struct ifreq* if_info)
{
	int err;
    mm_segment_t fs;
	
	if((NULL == dev) || (NULL == if_info))
		return 0;
	
	fs = get_fs();
       	set_fs(get_ds());     /* get user space block */ 

	/* Change the local ip address of the interface to 0.
	 * This will also delete the destination route.
	 */	

	DBG(KERN_INFO"%s:: call devinet_ioctl start! %s\n",__func__,if_info->ifr_name);
	//rtnl_unlock();
	err = devinet_ioctl(&init_net,SIOCSIFFLAGS,if_info);
	
	DBG(KERN_INFO"config_in_dev:: call devinet_ioctl finish!\n");

     set_fs(fs);           /* restore old block */
	if (err) {
		DBG (KERN_INFO "%s: config in_device failed %d!\n",
			 dev->name, err);
		return err;
	}else{
		DBG (KERN_INFO "%s: config in_device successfuly\n",
			dev->name);
	}
	return err;
}


static int kap_set_iff(struct kap_struct* kap,struct if_cfg_struct*pif)
{
	
	struct user_netdevice *user_dev;
	int err;
	struct ifreq if_info;
	memset(&if_info,0,sizeof(struct ifreq));


	 if (__dev_get_by_name(&init_net, pif->if_name)) 
	{
			DBG(KERN_INFO"%s : is exist\n",pif->if_name);
			return -EINVAL;
	}
	else {
		char *name;
		unsigned int flags = 0;

		err = -EINVAL;

		/* Set dev type */
		if (pif->if_flag & KAP_IFF_TUN) {
			/* KAP device */
			flags |= KAP_TUN_DEV;
			name = "kun%d";
		} else if (pif->if_flag & KAP_IFF_TAP) {
			/* TAP device */
			flags |= KAP_TAP_DEV;
			name = "kap%d";
		} else
		{
			DBG(KERN_INFO"param is Error flag = %d \n",pif->if_flag);
			goto failed;
		}
  
		if (*pif->if_name)
			name = pif->if_name;

		
		/*netdevice*/
		user_dev = kmalloc(sizeof(struct user_netdevice)*VNIC_PHYIF_MAX,GFP_KERNEL);
		if(user_dev == NULL)
		{
		    DBG(KERN_INFO "kap_set_iff::: kmalloc user_netdevice failed \n");
			return -ENOMEM;
		}
		memset(user_dev,0,sizeof(struct user_netdevice)*VNIC_PHYIF_MAX);

		user_dev->dev = alloc_netdev(sizeof(struct kap_struct), name, kap_setup);
		if (!user_dev->dev)
		{
			DBG(KERN_INFO"kap_set_iff::: Don't have Mem\n");
			return -ENOMEM;
		}

		user_dev->dev->priv = kap; 
		/* 
		kap = netdev_priv(dev);
		kap->dev = dev;
		*/		
		
		kap->flags = flags;
		/* Be promiscuous by default to maintain previous behaviour. */
		kap->if_flags = IFF_PROMISC;
		/* Generate random Ethernet address. */

		kap_net_init(user_dev->dev);

		if (strchr(user_dev->dev->name, '%')) {
			err = dev_alloc_name(user_dev->dev, user_dev->dev->name);
			if (err < 0)
			{
				DBG(KERN_INFO"dev_alloc Error\n");
				goto err_free_dev;
			}
		}

		/*exception*/
		rtnl_lock();
		err = register_netdevice(user_dev->dev);
		rtnl_unlock();
		if (err < 0)
		{
			DBG(KERN_INFO"register dev Error\n");
			goto err_free_dev;
		}
	
		list_add(&user_dev->list, &kap->dev_list);
	}

	/*exception*/
	rtnl_lock();
	dev_open(user_dev->dev);
	rtnl_unlock();
	
	strcpy(if_info.ifr_name,user_dev->dev->name);
	if(DEV_LINK_UP == pif->dev_state){
		if_info.ifr_flags = (IFF_RUNNING |IFF_UP | IFF_MULTICAST);
		set_dev_flags(user_dev->dev,&if_info);
	}
	else {
		//kap_net_open(user_dev->dev);
		if_info.ifr_flags = (IFF_BROADCAST | IFF_MULTICAST );
		set_dev_flags(user_dev->dev,&if_info);
	}
	DBG(KERN_INFO"%s set interface %s %s flag %#x\n", \
				__func__, user_dev->dev->name, pif->dev_state ? "UP":"DOWN", if_info.ifr_flags);

	
	if (pif->if_flag & KAP_IFF_NO_PI)
		kap->flags |= KAP_NO_PI;

	if (pif->if_flag & KAP_IFF_ONE_QUEUE)
		kap->flags |= KAP_ONE_QUEUE;

	/*set dev point array*/
	if((user_dev->dev->ifindex)>=MAX_DEV_NO) 	/* add to avoid Unhandled kernel error */
	{
		DBG(KERN_INFO"ifindex %d >= max:%d, %s failed !\n",user_dev->dev->ifindex,MAX_DEV_NO,__func__);
		err = -EINVAL;
		goto err_unreg_dev;		
	}
	
	kap->udev_index[user_dev->dev->ifindex] = user_dev; 

	user_dev->l3_index = user_dev->dev->ifindex;
	user_dev->dev_type = pif->dev_type;
	if(pif->dev_type == KAP_INTERFACE_PORT_E )
	{
		DBG(KERN_INFO"kap_set_iff::interface type port\n");
		user_dev->l2_index = pif->l2_index;
		user_dev->vId = pif->vId;
	}else if(pif->dev_type == KAP_INTERFACE_VID_E)
	{
		DBG(KERN_INFO"kap_set_iff::interface type vlan\n");
		user_dev->l2_index = ~0UI;
		user_dev->vId = pif->vId;
	}
	DBG(KERN_INFO"kap_set_iff:: l3_index %d,l2_index %d,vid %d\n",	\
					user_dev->l3_index,user_dev->l2_index,user_dev->vId);
	
	strcpy(pif->if_name, user_dev->dev->name);
	pif->l3_index = user_dev->dev->ifindex;
	return 0;
 err_unreg_dev:
 	rtnl_lock();	
 	unregister_netdevice(user_dev->dev);
	rtnl_unlock();	
 err_free_dev:
	kfree(user_dev);
 failed:
	return err;
}


static int kap_set_iff_from_adv(struct kap_struct* kap,struct if_cfg_struct*pif)
{
	
	struct user_netdevice *user_dev;
	int err;
	struct ifreq if_info;
	struct net_device * dev = NULL;
	memset(&if_info,0,sizeof(struct ifreq));


	if (dev = dev_get_by_name(&init_net, pif->if_name)) 
	{
		char *name = NULL;
		unsigned int flags = 0;

		err = -EINVAL;

		/* Set dev type */
		if (pif->if_flag & KAP_IFF_TUN) {
			/* KAP device */
			flags |= KAP_TUN_DEV;
			name = "kun%d";
		} else if (pif->if_flag & KAP_IFF_TAP) {
			/* TAP device */
			flags |= KAP_TAP_DEV;
			name = "kap%d";
		} else
		{
			DBG(KERN_INFO"param is Error flag = %d \n",pif->if_flag);
			dev_put(dev);
			goto failed;
		}
  
		if (*pif->if_name)/*not "" ? */
			name = pif->if_name;

		
		/*netdevice*/
		user_dev = kmalloc(sizeof(struct user_netdevice)*VNIC_PHYIF_MAX,GFP_KERNEL);
		if(user_dev == NULL)
		{
		    DBG(KERN_INFO "kap_set_iff::: kmalloc user_netdevice failed \n");
			dev_put(dev);
			return -ENOMEM;
		}
		memset(user_dev,0,sizeof(struct user_netdevice)*VNIC_PHYIF_MAX);

		user_dev->dev = dev;
		kap_setup(dev);

		user_dev->dev->priv = kap; 		
		kap->flags = flags;
		/* Be promiscuous by default to maintain previous behaviour. */
		kap->if_flags = IFF_PROMISC;
		/* Generate random Ethernet address. */

		kap_net_init(user_dev->dev);	
	
		list_add(&user_dev->list, &kap->dev_list);
		dev_put(dev);
	}
	else{
		err = -ENODEV;
		DBG(KERN_INFO"The dev %s not exists when set advanced-routing disable\n",pif->if_name);
		goto failed;
	}

	/*exception*/
	rtnl_lock();
	dev_open(user_dev->dev);
	rtnl_unlock();
	
	strcpy(if_info.ifr_name,user_dev->dev->name);
	if(DEV_LINK_UP == pif->dev_state){
		if_info.ifr_flags = (IFF_RUNNING |IFF_UP | IFF_MULTICAST);
		set_dev_flags(user_dev->dev,&if_info);
	}
	else {
		//kap_net_open(user_dev->dev);
		if_info.ifr_flags = (IFF_BROADCAST | IFF_MULTICAST );
		set_dev_flags(user_dev->dev,&if_info);
	}
	DBG(KERN_INFO"%s set interface %s %s flag %#x\n", \
				__func__, user_dev->dev->name, pif->dev_state ? "UP":"DOWN", if_info.ifr_flags);

	
	if (pif->if_flag & KAP_IFF_NO_PI)
		kap->flags |= KAP_NO_PI;

	if (pif->if_flag & KAP_IFF_ONE_QUEUE)
		kap->flags |= KAP_ONE_QUEUE;

	/*set dev point array*/
	kap->udev_index[user_dev->dev->ifindex] = user_dev; 

	user_dev->l3_index = user_dev->dev->ifindex;
	user_dev->dev_type = pif->dev_type;
	if(pif->dev_type == KAP_INTERFACE_PORT_E )
	{
		DBG(KERN_INFO"kap_set_iff::interface type port\n");
		user_dev->l2_index = pif->l2_index;
		user_dev->vId = pif->vId;
	}else if(pif->dev_type == KAP_INTERFACE_VID_E)
	{
		DBG(KERN_INFO"kap_set_iff::interface type vlan\n");
		user_dev->l2_index = ~0UI;
		user_dev->vId = pif->vId;
	}
	DBG(KERN_INFO"kap_set_iff:: l3_index %d,l2_index %d,vid %d\n",	\
					user_dev->l3_index,user_dev->l2_index,user_dev->vId);
	
	strcpy(pif->if_name, user_dev->dev->name);
	pif->l3_index = user_dev->dev->ifindex;
	return 0;

 failed:
	return err;
}


static __inline__  int kap_set_checksum(struct kap_struct *kap,struct if_cfg_struct*pif)
{
		//struct kap_struct* kap = file->private_data;
		//kap = kap_get_by_name(pif->if_name,chrdev);

		if(!kap)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -EINVAL;
		}
			
		if (pif->if_flag)
			kap->flags |= KAP_NOCHECKSUM;
		else
			kap->flags &= ~KAP_NOCHECKSUM;

		return 0;
}

static __inline__  int kap_set_persist(struct kap_struct *kap,struct if_cfg_struct*pif)
{
		//struct kap_struct* kap;
		//kap = kap_get_by_name(pif->if_name,chrdev);

		if(!kap)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -EINVAL;
		}
		
		if (pif->if_flag)
			kap->flags |= KAP_PERSIST;
		else
			kap->flags &= ~KAP_PERSIST;


		return 0;
}

static __inline__  int __kap_set_link(struct kap_struct *kap,struct if_cfg_struct*pif)
{
		struct user_netdevice* udev;
		
		udev = kap->udev_index[pif->l3_index];
		if(!udev)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -ENODEV;
		}

		if(DEV_LINK_UP == pif->dev_state){
			if (udev->dev->flags & IFF_UP) 
			{
				DBG(KERN_INFO "%s: Linktype set failed because interface is UP\n",udev->dev->name);
				return 0;
			} else 
			{
				DBG(KERN_INFO "%s: Linktype set interface UP\n",udev->dev->name);
				//rtnl_lock();
				dev_open(udev->dev);
				//rtnl_unlock();
				return 0;
			}
		}
		else if(DEV_LINK_DOWN == pif->dev_state)
		{
			if (udev->dev->flags & IFF_UP) 
			{
				DBG(KERN_INFO "%s: Linktype set  interface DOWN\n",udev->dev->name),
				//rtnl_lock();
				dev_close(udev->dev);
				//rtnl_unlock();
				return 0;
			}
			else 
			{
				DBG(KERN_INFO "%s: Linktype set failed because interface is DOWN\n",udev->dev->name);
				return 0;
			}
		}
		else
		{
			/*code optimize:  Missing return statement
			zhangcl@autelan.com 2013-1-21*/
			DBG(KERN_INFO "%s: Linkstate is not up/down.\n",udev->dev->name);			
			return 0;
		}					
}

static __inline__  int kap_set_debug(struct kap_struct *kap,struct if_cfg_struct*pif)
{

		if(!kap)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -EINVAL;
		}
#ifdef KAP_DEBUG
		kap->debug = pif->if_debug;
		//printk_console_no_output = pif->if_debug;
#endif
		return 0;
}


static int config_in_dev(struct kap_struct* kap, unsigned int cmd,struct if_cfg_struct* pif)
{

	int err;
    mm_segment_t fs;
	struct ifreq if_info;
	struct sockaddr_in *if_data_req,*if_data_cfg;
	struct user_netdevice* udev = NULL;
	
	udev = kap->udev_index[pif->l3_index];
	if(!udev)
	{
			DBG(KERN_INFO"Don't find dev\n");
			return -ENODEV;
	}

	/* Set Local and remote addresses */
	memset(&if_info, 0, sizeof(if_info));
	strcpy(if_info.ifr_name, udev->dev->name);
	DBG(KERN_INFO"config_in_dev:: Into this fun!\n");

	fs = get_fs();
       	set_fs(get_ds());     /* get user space block */ 

	/* Change the local ip address of the interface to 0.
	 * This will also delete the destination route.
	 */	
		
	if_data_req = (struct sockaddr_in *)&if_info.ifr_addr;
	if_data_cfg = (struct sockaddr_in *)&pif->if_addr;
	if_data_req->sin_addr.s_addr = if_data_cfg->sin_addr.s_addr;
	if_data_req->sin_family = if_data_cfg->sin_family;	
	DBG(KERN_INFO"config_in_dev:: call devinet_ioctls_addr = %0x, sin_family = %d\n",if_data_cfg->sin_addr.s_addr,if_data_cfg->sin_family);
	err = devinet_ioctl(&init_net, cmd, &if_info );
	DBG(KERN_INFO"config_in_dev:: call devinet_ioctl finish!\n");

     set_fs(fs);           /* restore old block */
	if (err) {
		DBG (KERN_INFO "%s: config in_device failed %d!\n",
			 udev->dev->name, err);
		return err;
	}else{
		DBG (KERN_INFO "%s: config in_device successfuly\n",
			udev->dev->name );
	}
	return err;
}
#if 0
static int getipcfg(struct kap_struct * kap, unsigned int cmd, struct if_cfg_struct * pif)
{
	int ret = 0;
	struct ifreq if_info;
	struct user_netdevice* udev = NULL;
	struct sockaddr_in sin_orig;
	struct sockaddr_in *sin = (struct sockaddr_in *)(pif->if_addr);
	struct in_device *in_dev;
	struct in_ifaddr **ifap = NULL;
	struct in_ifaddr *ifa = NULL;
	int tryaddrmatch = 0;
	
	rtnl_lock();
	udev = kap->udev_index[pif->l3_index];
	if(!udev || !udev->dev)
	{
		DBG(KERN_INFO"Don't find dev\n");
		rtnl_unlock();
		return -EINVAL;
	}


	/* Set Local and remote addresses */
	memset(&if_info, 0, sizeof(if_info));
	strcpy(if_info.ifr_name, udev->dev->name);
	DBG(KERN_INFO"getcfg:: Into this fun!\n");

	memcpy(&sin_orig, sin, sizeof(*sin));
	tryaddrmatch = (sin_orig.sin_family == AF_INET);
	memset(sin, 0, sizeof(*sin));
	sin->sin_family = AF_INET;

	if ((in_dev = (struct in_device*)(udev->dev->ip_ptr)) != NULL) {
		if (tryaddrmatch) {
			/* Matthias Andree */
			/* compare label and address (4.4BSD style) */
			/* note: we only do this for a limited set of ioctls
			   and only if the original address family was AF_INET.
			   This is checked above. */
			for (ifap = &in_dev->ifa_list; (ifa = *ifap) != NULL;
			     ifap = &ifa->ifa_next) {
				if (!strcmp(if_info.ifr_name, ifa->ifa_label) &&
				    sin_orig.sin_addr.s_addr ==
							ifa->ifa_address) {
					break; /* found */
				}
			}
		}
		/* we didn't get a match, maybe the application is
		   4.3BSD-style and passed in junk so we fall back to
		   comparing just the label */
		if (!ifa) {
			for (ifap = &in_dev->ifa_list; (ifa = *ifap) != NULL;
			     ifap = &ifa->ifa_next)
				if (!strcmp(if_info.ifr_name, ifa->ifa_label))
					break;
			    }
	}


	if(ifa) {
		sin->sin_addr.s_addr = ifa->ifa_local;
		pif->netmask[0] = ifa->ifa_mask;
	}
	else {

		ret = -EINVAL;
	}
	
	rtnl_unlock();
	return ret;
	
}
#define STATUS_NO_IP 0x0101 /*l3intf no ip address*/
#define STATUS_HAVE_SAME_IP 0x0102 /*have the same ip address */
#define STATUS_NO_SAME_SUBNET_IP 0x0103 /*no same subnet ip address*/
#define STATUS_OK 0x0100 /*have the same sub net ip ,and no same ip */

#endif

#define INVALID_HOST_IP 0xFFFFFFFF
static int getipcfgs(struct kap_struct * kap, unsigned int cmd, struct if_cfg_struct * pif)
{
	int ret = 0;
	struct ifreq if_info;
	struct user_netdevice* udev = NULL;
	struct sockaddr_in sin_orig[MAX_IP_COUNT];
	struct sockaddr_in *sin = (struct sockaddr_in *)(pif->if_addr);
	struct in_device *in_dev;
	struct in_ifaddr **ifap = NULL;
	struct in_ifaddr *ifa = NULL;
	int i=0;
	if(!kap || !pif)
	{
		DBG(KERN_INFO"Don't find dev\n");
		return -EINVAL;
	}

	rtnl_lock();
	rcu_read_lock();
	udev = kap->udev_index[pif->l3_index];
	if(!udev || !udev->dev)
	{
		DBG(KERN_INFO"Don't find dev\n");
		rcu_read_unlock();
		rtnl_unlock();
		return -ENODEV;
	}


	/* Set Local and remote addresses */
	memset(&if_info, 0, sizeof(if_info));
	strcpy(if_info.ifr_name, udev->dev->name);
	DBG(KERN_INFO"getipcfgs:: Into this function!\n");

	memset(sin_orig, 0, sizeof(struct sockaddr_in)*MAX_IP_COUNT);
	memcpy(sin_orig, sin, sizeof(struct sockaddr_in)*MAX_IP_COUNT);
	memset(sin, 0, sizeof(struct sockaddr_in)*MAX_IP_COUNT);
	for(i=0; i<MAX_IP_COUNT; i++) {
		sin[i].sin_family = AF_INET;
		sin[i].sin_addr.s_addr = INVALID_HOST_IP;
	}
	i = 0;

	if ((in_dev = __in_dev_get_rcu(udev->dev)) != NULL) {
		for (ifap = &in_dev->ifa_list ;(ifa = *ifap) != NULL ;ifap = &ifa->ifa_next) {
			if(MAX_IP_COUNT == i) {
				break;
			}
			sin[i].sin_addr.s_addr = htonl(ifa->ifa_local);
			pif->netmask[i]= htonl(ifa->ifa_mask);
			DBG(KERN_DEBUG"ip %d %#x mask %#x\n", i, sin[i].sin_addr.s_addr, pif->netmask[i]);
			i++;
		}
	}

	if(0 == i) {
		ret = -ENXIO;
	}

	rcu_read_unlock();
	rtnl_unlock();
	return ret;
	
}

static __inline__  int kap_set_link(struct kap_struct *kap,struct if_cfg_struct*pif)
{
		struct user_netdevice* udev = NULL;
		struct ifreq if_info; 
		if(!kap || !pif)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -EINVAL;
		}
		udev = kap->udev_index[pif->l3_index];
		if(!udev)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -ENODEV;
		}

		/* Set Local and remote addresses */
		memset(&if_info,0, sizeof(struct ifreq));
		strcpy(if_info.ifr_name,udev->dev->name);
		DBG(KERN_INFO"config_in_dev:: Into this fun!\n");

		if(DEV_LINK_UP == pif->dev_state){
			if (udev->dev->flags & IFF_UP) {
				DBG(KERN_INFO "%s: Linktype set failed because interface is UP\n",udev->dev->name);
				return 0;
			} else {
				if_info.ifr_flags = (IFF_RUNNING |IFF_UP | IFF_MULTICAST);
				DBG(KERN_INFO "%s: Linktype set interface UP\n",udev->dev->name);
				set_dev_flags(udev->dev,&if_info);
				return 0;
			}
		}
		else if(DEV_LINK_DOWN == pif->dev_state){
			if (udev->dev->flags & IFF_UP) {
				DBG(KERN_INFO "%s: Linktype set  interface is DOWN\n",udev->dev->name);
				if_info.ifr_flags = (IFF_BROADCAST | IFF_MULTICAST);
				set_dev_flags(udev->dev,&if_info);
				return 0;
			} else {
				DBG(KERN_INFO "%s: Linktype set failed because interface is DOWN\n",udev->dev->name);
				return 0;
			}
		}
		return 0;   /*for: kap.c:1300: warning: control reaches end of non-void function*/			
}

static __inline__  int kap_set_ip(struct kap_struct *kap,struct if_cfg_struct*pif)
{

		int ret = 0;

		if(!kap || !pif)
		{
				DBG(KERN_INFO"Don't find dev\n");
				return -EINVAL;
		}

		DBG(KERN_INFO "###### config_in_dev \n");	
		ret = config_in_dev(kap,SIOCSIFADDR,pif);
		
		return ret;
}
#if 0
static __inline__ int kap_get_ip(struct kap_struct *kap,void __user* argp,struct if_cfg_struct*pif)
{
		int ret = 0;

		if(!kap || !pif)
		{
			DBG(KERN_INFO"Don't find dev\n");
			return -EINVAL;
		}

		ret = getipcfg(kap,SIOCGIFADDR,pif);
		if(0 != ret) {
			return ret;
		}
	
		/*ret = getnetmask(kap,SIOCGIFNETMASK,pif);
		if(0 != ret) {
			return ret;
		}*/

		if(copy_to_user(argp,pif,sizeof(struct if_cfg_struct))) {
			DBG(KERN_INFO"kap_get_ip :: copy_to_user Failed\n");
			return -EFAULT;
		}
	
		return ret;
}
#endif

static __inline__ int kap_get_ips(struct kap_struct *kap,void __user* argp,struct if_cfg_struct*pif)
{
		int ret = 0;

		if(!kap || !pif)
		{
			DBG(KERN_INFO"Don't find dev\n");
			return -EINVAL;
		}

		ret = getipcfgs(kap,SIOCGIFADDR,pif);
		if(0 != ret) {
			return ret;
		}
	
		/*ret = getnetmask(kap,SIOCGIFNETMASK,pif);
		if(0 != ret) {
			return ret;
		}*/

		if(copy_to_user(argp,pif,sizeof(struct if_cfg_struct))) {
			DBG(KERN_INFO"kap_get_ip :: copy_to_user Failed\n");
			return -EFAULT;
		}
	
		return ret;
}

static __inline__ int kap_set_dev_info(struct kap_struct* kap,struct if_cfg_struct* pif)
{
	struct user_netdevice* udev;
	/* add to avoid Unhandled kernel error */
	if((pif->l3_index)>=MAX_DEV_NO)
	{
		DBG(KERN_INFO"ifindex %d >= max:%d, %s failed !\n",pif->l3_index,MAX_DEV_NO,__func__);
		return -EFAULT;		
	}
	udev = kap->udev_index[pif->l3_index];
	if(!udev)
	{
		DBG(KERN_INFO"%s can't find dev\n", __func__);
		return -ENODEV;
	}

	if(pif->dev_type == KAP_INTERFACE_PORT_E)
	{
		udev->l2_index = pif->l2_index;
		udev->vId = pif->vId;
	}
	if(pif->dev_type == KAP_INTERFACE_VID_E)
	{
		udev->l2_index = 0xffffffff;
		udev->vId = pif->vId;
	}

	return 0;
	
}

static __inline__ int kap_get_dev_info(struct kap_struct* kap,void __user* argp,struct if_cfg_struct* pif)
{
	struct user_netdevice* udev = NULL;;
	struct net_device* dev = NULL;
	

	/*code optimize: Array compared against 0
	zhangcl@autelan.com 2013-1-21*/
	#if 0
	if(NULL == pif->if_name)
	{
		return -EINVAL;
	}
	#endif
	if(NULL == (dev = __dev_get_by_name(&init_net, pif->if_name)))
	{
	    return -ENODEV;
	}
	else if((MAX_DEV_NO > dev->ifindex)&&(NULL != (udev = kap->udev_index[dev->ifindex])))
	{
		DBG(KERN_INFO "%s name %s vid %d l3if %d\n", \
				__func__, pif->if_name, udev->vId, udev->l3_index);
		pif->vId = udev->vId;
		pif->l2_index = udev->l2_index;
		pif->l3_index = udev->l3_index;
		pif->dev_type = udev->dev_type;
		if(copy_to_user(argp,pif,sizeof(struct if_cfg_struct)))
		{
			DBG(KERN_INFO"kap_get_dev_info :: copy_to_user Failed\n");
			return -EFAULT;
		}
	}
	else
	{
		DBG(KERN_INFO"ifindex %d >= max:%d, %s failed !\n",dev->ifindex,MAX_DEV_NO,__func__);
		return -EFAULT;		
	}
	
	return 0;
}
static __inline__ int kap_detect(struct kap_struct*kap)
{
    int pkg_num = 0;
		
	rtnl_lock();
	pkg_num = skb_queue_len(&kap->readq) ;
	rtnl_unlock();
	
	if(pkg_num)
	{
		DBG(KERN_INFO"kap detected %d packets need send\n",pkg_num);
	}
	return pkg_num;
}

static __inline__ int kap_get_dev_mac(struct kap_struct* kap,void __user* argp,struct if_cfg_struct* pif)
{
	struct user_netdevice* udev = NULL;

	/* add to avoid Unhandled kernel error */
	if((pif->l3_index)>=MAX_DEV_NO)
	{
		DBG(KERN_INFO"ifindex %d >= max:%d, %s failed !\n", pif->l3_index,MAX_DEV_NO,__func__);
		return -ENODEV;		
	}

	udev = kap->udev_index[pif->l3_index];
	if(!udev)
	{
			DBG(KERN_INFO"L3 if %d with no kap udev,use default mac %02x:%02x:%02x:%02x:%02x:%02x\n", \
					pif->l3_index, kap->if_mac_addr[0],kap->if_mac_addr[1],kap->if_mac_addr[2], \
					kap->if_mac_addr[3],kap->if_mac_addr[4],kap->if_mac_addr[5]);
			memcpy(pif->mac_addr,kap->if_mac_addr,6);
				
			if(copy_to_user(argp,pif,sizeof(struct if_cfg_struct)))
			{
					DBG(KERN_INFO"kap_get_dev_mac :: copy_to_user Failed\n");
					return -EFAULT;
			}

			return -ENODEV;
	}
			
	DBG(KERN_INFO"L3 if %d with kap udev mac %02x:%02x:%02x:%02x:%02x:%02x\n", \
			pif->l3_index, udev->dev->dev_addr[0],udev->dev->dev_addr[1],udev->dev->dev_addr[2], \
			udev->dev->dev_addr[3],udev->dev->dev_addr[4],udev->dev->dev_addr[5]);
	memcpy(pif->mac_addr, udev->dev->dev_addr, 6);
	
	if(copy_to_user(argp,pif,sizeof(struct if_cfg_struct)))
	{
			DBG(KERN_INFO"kap_get_dev_mac :: copy_to_user Failed\n");
			return -EFAULT;
	}

	return 0;
	
}

static __inline__ int kap_set_dev_mac(struct kap_struct* kap,struct if_cfg_struct* pif)
{
	struct user_netdevice* udev;
	int i;

	/* add to avoid Unhandled kernel error */
	if((pif->l3_index)>=MAX_DEV_NO)
	{
		DBG(KERN_INFO"ifindex %d >= max:%d, %s failed !\n", pif->l3_index,MAX_DEV_NO,__func__);
		return -EFAULT;		
	}

	udev = kap->udev_index[pif->l3_index];
	if(!udev)
	{
			DBG(KERN_INFO"Don't find dev\n");
			return -ENODEV;
	}

	DBG(KERN_INFO"kap_set_dev_mac :: get mac\n");
	for(i = 0; i < 6 ; i++)
	{
		//kap->if_mac_addr[i] = pif->mac_addr[i];
		udev->dev->dev_addr[i] = pif->mac_addr[i];
		DBG(KERN_INFO"%02x%s",kap->if_mac_addr[i],(5 == i) ? "" : ":");
	}
	DBG(KERN_INFO"\n");

	return 0;
}

static __inline__ int kap_del_iff(struct kap_struct* kap,struct if_cfg_struct* pif)
{
	struct user_netdevice *udev,*nxt;
	int del_sta = 0; 

	if (!kap)
		return -EINVAL;

	rtnl_lock();
	list_for_each_entry_safe(udev, nxt, &kap->dev_list, list) {
		DBG(KERN_INFO "%s cleaned up\n", udev->dev->name);
		if(udev->l3_index == pif->l3_index)
		{
			unregister_netdevice(udev->dev);
			list_del(&udev->list);
			kap->udev_index[udev->l3_index] = NULL;
			/*free_netdev(udev->dev);*/
			kfree(udev);
			del_sta = 1;
			break;
		}
		
	}
	rtnl_unlock();

	if(1 != del_sta)
	{
		DBG(KERN_INFO"kap_del_iff :: Don't find the dev by index %d\n",pif->l3_index);
		return -ENODEV;
	}

    DBG(KERN_INFO "kap_del_iff del %d  succeed\n",pif->l3_index);
	return 0;

	
}


static __inline__ int kap_del_iff_for_adv(struct kap_struct* kap,struct if_cfg_struct* pif)
{
	struct user_netdevice *udev,*nxt;
	int del_sta = 0; 

	if (!kap)
		return -EINVAL;

	rtnl_lock();
	list_for_each_entry_safe(udev, nxt, &kap->dev_list, list) {
		DBG(KERN_INFO "%s cleaned up\n", udev->dev->name);
		if(udev->l3_index == pif->l3_index)
		{
			if(udev->dev->flags & IFF_UP){
				dev_close(udev->dev);
			}
			kap_unsetup(udev->dev);
			kap_net_uninit(udev->dev);
			/* priv should be kap, we can't kfree it */
			udev->dev->priv = NULL;
			list_del(&udev->list);
			kap->udev_index[udev->l3_index] = NULL;
			kfree(udev);
			del_sta = 1;
			break;
		}
		
	}
	rtnl_unlock();

	if(1 != del_sta)
	{
		DBG(KERN_INFO"kap_del_iff :: Don't find the dev by index %d\n",pif->l3_index);
		return -ENODEV;
	}

    DBG(KERN_INFO "kap_del_iff del %d  succeed\n",pif->l3_index);
	return 0;

	
}

static __inline__ int kap_create_iff(struct kap_struct* kap,void __user* argp, struct if_cfg_struct* pif)
{
		int err;

		pif->if_name[IFNAMSIZ-1] = '\0';

		DBG(KERN_INFO"create iff\n");
		
		//rtnl_lock();
		err = kap_set_iff(kap,pif);
		//rtnl_unlock();

		if (err)
		{
			DBG(KERN_INFO"kap_set_iff Failed\n");
			return err;
		}

		if (copy_to_user(argp, pif, sizeof(struct if_cfg_struct)))
		{
			DBG(KERN_INFO"copy_to_user Failed\n");
			return -EFAULT;
		}
		return 0;
}

static __inline__ int kap_set_adv_disable(struct kap_struct* kap,void __user* argp, struct if_cfg_struct* pif)
{
		int err;

		pif->if_name[IFNAMSIZ-1] = '\0';

		DBG(KERN_INFO"create iff\n");
		
		//rtnl_lock();
		err = kap_set_iff_from_adv(kap,pif);
		//rtnl_unlock();

		if (err)
		{
			DBG(KERN_INFO"kap_set_iff Failed\n");
			return err;
		}

		if (copy_to_user(argp, pif, sizeof(struct if_cfg_struct)))
		{
			DBG(KERN_INFO"copy_to_user Failed\n");
			return -EFAULT;
		}
		return 0;
}


static char *
kap_chr_iocmd_map
(
	unsigned int cmd
)
{
	// KAP ioctl command descriptors
	switch(cmd) {
		default:
			return "UNKNOWN CMD";
		case KAPSETNOCSUM:
			return "KAPSETNOCSUM";
		case KAPSETDEBUG:
			return "KAPSETDEBUG";
		case KAPSETIFF:
			return "KAPSETIFF";
		case KAPSETPERSIST:
			return "KAPSETPERSIST";
		case KAPSETOWNER:
			return "KAPSETOWNER";
		case KAPSETLINK:
			return "KAPSETLINK";
		case KAPADDIFF:
			return "KAPADDIFF";
		case KAPWAITING:
			return "KAPWAITING";
		case KAPSETIPADDR:
			return "KAPSETIPADDR";
		case KAPGETIPADDR:
			return "KAPGETIPADDR";
		case KAPRETURNPKGNO:
			return "KAPRETURNPKGNO";
		case KAPSETDEVINFO:
			return "KAPSETDEVINFO";
		case KAPSETMACADDR:
			return "KAPSETMACADDR";
		case KAPGETMACADDR:
			return "KAPGETMACADDR";
		case KAPDELIFF:
			return "KAPDELIFF";
		case KAPGETDEVINFO:
			return "KAPGETDEVINFO";
		case KAPGETIPADDRS:
			return "KAPGETIPADDRS";		
		case KAPSETADVDIS:
			return "KAPSETADVDIS";
		case KAPSETADVEN:
			return "KAPSETADVEN";
	}
}

static int kap_chr_ioctl(struct inode *inode, struct file *file, 
			 unsigned int cmd, unsigned long arg)
{
	struct kap_struct *kap = file->private_data;
	void __user* argp = (void __user*)arg;
	struct if_cfg_struct if_cfg ;
	memset(&if_cfg,0,sizeof(struct if_cfg_struct));
	if(_IOC_TYPE(cmd) != 'T'){return -ENOTTY;}

	/*(cmd & 0xff) >255. zhangcl@autelan.com 2013-1-21  */
	if(_IOC_NR(cmd) > KAP_MAXNR){return -ENOTTY;}

	if (copy_from_user(&if_cfg, argp, sizeof if_cfg))
	{
			return -EFAULT;
	}

	DBG(KERN_INFO"%s name %s flag %d index %d cmd[%#x] %s\n",	\
		__func__,if_cfg.if_name,if_cfg.if_flag,if_cfg.l3_index,cmd, kap_chr_iocmd_map(cmd));

	switch (cmd) 
	{
			
		case KAPSETIFF:
		case KAPADDIFF:
			/*create dev*/
			return kap_create_iff(kap,argp, &if_cfg);

		case KAPDELIFF:
			/*delete dev*/
			return kap_del_iff(kap,&if_cfg);
		case KAPSETADVDIS:
			/*create dev*/
			return kap_set_adv_disable(kap,argp, &if_cfg);

		case KAPSETADVEN:
			/*delete dev*/
			return kap_del_iff_for_adv(kap,&if_cfg);
			
		case KAPSETNOCSUM:
			/* Disable/Enable checksum */
			return  kap_set_checksum(kap,&if_cfg);

		case KAPSETPERSIST:
			/* Disable/Enable persist mode */
			return kap_set_persist(kap,&if_cfg);

		//case KAPSETOWNER:
			/* Set owner of the device */
			//return kap_set_owner(kap,&if_cfg);
			
		case KAPSETLINK:
			/* Only allow setting the type when the interface is down */
			return kap_set_link(kap,&if_cfg);			
			

#ifdef KAP_DEBUG
		case KAPSETDEBUG:
			return kap_set_debug(kap,&if_cfg);
#endif
		case KAPRETURNPKGNO:
			DBG(KERN_INFO"%s line %d get packet number\n", __func__, __LINE__);
			return kap_detect(kap);

		case KAPSETIPADDR:
			/** Set the character device's interface flags. Currently only
			 * IFF_PROMISC and IFF_ALLMULTI are used. */
			 DBG(KERN_INFO"%s line %d set ip addr\n",__func__,__LINE__);
			return kap_set_ip(kap,&if_cfg);
#if 0
		case KAPGETIPADDR:
			 DBG(KERN_INFO"file %s line %d get ip addr\n",__func__,__LINE__);
			 return kap_get_ip(kap,argp,&if_cfg);
#endif
		case KAPGETIPADDRS:
			 DBG(KERN_INFO"%s line %d get ip addr\n",__func__,__LINE__);
			 return kap_get_ips(kap,argp,&if_cfg);
			
		case KAPWAITING:
			DBG(KERN_INFO"wait for a packet...... \n");
			if(down_interruptible(&kapReadPacketSem))
			//down(&kapReadPacketSem);
			{
				DBG(KERN_INFO"wait for catching packet sync lock error\n");
				return -ERESTARTSYS;
			}
			DBG(KERN_INFO"catch packet sync lock, new packet need send\n");
			break;

		case KAPSETDEVINFO:
			return kap_set_dev_info(kap,&if_cfg);

		case KAPGETDEVINFO:
			return kap_get_dev_info(kap,argp, &if_cfg);

		case KAPGETMACADDR:
			return kap_get_dev_mac(kap,argp, &if_cfg);

		case KAPSETMACADDR:
			return kap_set_dev_mac(kap, &if_cfg);
			
		default:
			return -EINVAL;
	};

	return 0;
}

/*no problem*/
static int kap_chr_fasync(int fd, struct file *file, int on)
{
	struct kap_struct *kap = file->private_data;
	int ret;

	if (!kap)
		return -EBADFD;

	//DBG(KERN_INFO "%s: kap_chr_fasync %d\n", kap->dev->name, on);

	if ((ret = fasync_helper(fd, file, on, &kap->fasync)) < 0)
		return ret; 
 
	if (on) {
		ret = f_setown(file, current->pid, 0);
		if (ret)
			return ret;
		kap->flags |= KAP_FASYNC;
	} else 
		kap->flags &= ~KAP_FASYNC;

	return 0;
}

static int kap_chr_open(struct inode *inode, struct file * file)
{
	struct kap_struct *kap = NULL; 
	DBG(KERN_INFO "kapX: tun_chr_open\n");

	kap = kmalloc(sizeof(struct kap_struct), GFP_KERNEL);
	if (kap == NULL)
	{
	    DBG(KERN_INFO "chr_open kmalloc kap_struct failed \n");
		return -ENOMEM;

	}
	memset(kap, 0, sizeof(struct kap_struct));
	file->private_data = kap;  

	sema_init(&kapReadPacketSem,0);

	kap->udev_index = kmalloc(sizeof(struct net_device*)*MAX_DEV_NO,GFP_KERNEL);
	if(!kap->udev_index)
	{
	    DBG(KERN_INFO "chr_open kmalloc net_device port failed \n");
		return -ENOMEM;
	}
	memset(kap->udev_index,0,sizeof(struct user_netdevice*)*MAX_DEV_NO);
	
    /*init list*/
	skb_queue_head_init(&kap->readq);
	init_waitqueue_head(&kap->read_wait);	

	INIT_LIST_HEAD(&kap->dev_list);
	
    mac_addr_get(kap->if_mac_addr);
#if 0
	kap->if_mac_addr[0] = octeon_bootinfo->mac_addr_base[0];
	kap->if_mac_addr[1] = octeon_bootinfo->mac_addr_base[1];
	kap->if_mac_addr[2] = octeon_bootinfo->mac_addr_base[2];
	kap->if_mac_addr[3] = octeon_bootinfo->mac_addr_base[3];
	kap->if_mac_addr[4] = octeon_bootinfo->mac_addr_base[4];
	kap->if_mac_addr[5] = octeon_bootinfo->mac_addr_base[5];
#endif
	DBG(KERN_INFO "chr_open Open chr device succesful \n");
	return 0;
}


/* modify same to origine*/
static int kap_chr_close(struct inode *inode, struct file *file)
{
	struct kap_struct *kap = (struct kap_struct *)file->private_data;
	struct user_netdevice *udev ,*nxt; 

	if (!kap)
		return 0;

	kap_chr_fasync(-1, file, 0);

	rtnl_lock();
	list_for_each_entry_safe(udev, nxt, &kap->dev_list, list) {
		DBG(KERN_INFO "%s cleaned up\n", udev->dev->name);
		unregister_netdevice(udev->dev);
		/*free_netdev(udev->dev);*/
		kfree(udev);
	}
	rtnl_unlock();

	kfree(kap->udev_index);
	kfree(kap);
	file->private_data = NULL;
    DBG(KERN_INFO "chr_close unregister net/KAP device succesful \n");

	return 0;

}


long kap_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg)
{
    int rval;

    DBG(KERN_INFO"COMPAT IOCTL cmd is [%x] IOC Type is [%d].\n",cmd,_IOC_TYPE(cmd));
    lock_kernel();
    rval = kap_chr_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);
    unlock_kernel();

    return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;

}


static struct file_operations kap_fops = {
	.owner	= THIS_MODULE,	
	.llseek = no_llseek,
	.read = kap_chr_read,
	/*.readv	= kap_chr_readv,*/
	.write = kap_chr_write,
	/*.writev = kap_chr_writev,*/
	.poll	= kap_chr_poll,
	.ioctl	= kap_chr_ioctl,
	.compat_ioctl = kap_compat_ioctl,
	.open	= kap_chr_open,
	.release = kap_chr_close,
	.fasync = kap_chr_fasync		
};

static struct miscdevice kap_miscdev = {
	.minor = KAP_MINOR,
	.name = "kap0",
	.fops = &kap_fops,
//	.devfs_name = "net/kap",
};


static int __init kap_init(void)
{
	int ret = 0;

	printk(KERN_INFO "kap: %s, %s\n", DRV_DESCRIPTION, DRV_VERSION);
	//printk(KERN_INFO "kap: %s\n", DRV_COPYRIGHT);

	ret = misc_register(&kap_miscdev);
	if (ret)
		printk(KERN_ERR "kap: Can't register misc device %d\n", KAP_MINOR);

	printk(KERN_INFO"kap dev register succeed\n");
	return ret;
}

static void kap_cleanup(void)
{
	misc_deregister(&kap_miscdev); 
	printk(KERN_INFO"unregister miscdev succeed\n");
}

module_init(kap_init);
module_exit(kap_cleanup);
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR(DRV_COPYRIGHT);
MODULE_LICENSE("Autelan");
MODULE_ALIAS_MISCDEV(KAP_MINOR);

#ifdef __cplusplus
}
#endif
