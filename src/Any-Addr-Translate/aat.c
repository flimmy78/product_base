
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/spinlock.h>

#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#include "aat.h"
#include "aat_ioctl.h"
#include "aat_rx.h"
#include "aat_tx.h"

/*netlink to asd,ht add 110226*/
#define NETLINK_WITH_ASD	17
unsigned int  asd_pid = 0;
struct sock *nl_sk = NULL;
struct task_struct *my_netlink_test = NULL;
struct aat_info allif;

/* lock for allif,
     0: R for ioctl,rx/tx,fastfwd get sta
     1: RW for ioctl(add/del/clean all sta)
 */
int allif_lock = 0;

/*
   spinlock allif_splock: RW ioctl(add/del sta) and clean all sta
 */
spinlock_t allif_splock;

#define DRIVER_NAME "aat"
#define AAT_SDK_VERSION_STRING "aat_v0.1"

int aat_major_num = 249;
module_param(aat_major_num, int, 0444);
int aat_minor_num = 0;
module_param(aat_minor_num,int,0444);
int aat_debug = 0;
module_param(aat_debug,int,0644);


extern int (*aat_ko_rx_hook)(struct sk_buff *);
extern int (*aat_ko_tx_hook)(struct sk_buff *);


struct oct_dev_s {
	long quantum;
	long qset;
	unsigned long size;
	unsigned long access_key;
	struct cdev cdev;
};

static struct oct_dev_s aat_dev;

/* get sta vip/rip for ipfwd_learn
 * input:
 *       int type                 (now support 0:STA MAC 1:STA VIP)
 *       struct aat_sta_info *sta 
 *                                (type is 0: member stamac is must
 *                                 type is 1: member staip is must)
 * return:
         0: error
         1: success
 */
unsigned int aat_get_sta_info
(
	int type,
	struct aat_sta_info *sta
)
{
	struct sta_info *tmp = NULL;

	if (NULL == sta)
	{
		return 0;
	}

	if (1 == allif_lock)
	{
		if (aat_debug >= AAT_DEBUG)
		{
			printk("%s-%d allif LOCKED, no allow get sta info\n", __func__, __LINE__);
		}
		return 0;
	}

	switch (type)
	{
		case AAT_STA_GET_TYPE_MAC:
		{
			tmp = aat_get_sta(&allif, sta->stamac);
			if (NULL != tmp)
			{
				memcpy(sta->acmac, tmp->acmac, MAC_LEN);
				memcpy(sta->ifname, tmp->ifname, strlen(tmp->ifname));
				sta->staip = tmp->staip;			   /*sta virtual ip address */
				sta->realsip = tmp->realsip;		   /*sta real ip address */
			}
			else
			{
				if (aat_debug >= AAT_DEBUG)
				{
					printk(KERN_DEBUG "%s-%d error: by MAC %02x:%02x:%02x:%02x:%02x:%02x not found\n",
							__func__, __LINE__,
							   sta->stamac[0], sta->stamac[1], sta->stamac[2],
							   sta->stamac[3], sta->stamac[4], sta->stamac[5]);
				}
			
				memset(sta->acmac, 0, MAC_LEN);
				memset(sta->ifname, 0, ETH_LEN);
				sta->staip = 0;
				sta->realsip = 0;
			}
		
			break;
		}

		case AAT_STA_GET_TYPE_VIP:
		{
			tmp = aat_get_sta_by_vip(&allif, sta->staip);
			if (NULL != tmp)
			{
				memcpy(sta->stamac, tmp->stamac, MAC_LEN);
				memcpy(sta->acmac, tmp->acmac, MAC_LEN);
				memcpy(sta->ifname, tmp->ifname, strlen(tmp->ifname));
				sta->realsip = tmp->realsip;		   /*sta real ip address */
			}
			else
			{
				if (aat_debug >= AAT_DEBUG)
				{
					printk(KERN_DEBUG"%s-%d error: by IP %u.%u.%u.%u not found\n",
								__func__, __LINE__,
									((sta->staip & 0xff000000) >> 24),
									((sta->staip & 0xff0000) >> 16),
									((sta->staip & 0xff00) >> 8),
									(sta->staip & 0xff));
				}

				memset(sta->stamac, 0, MAC_LEN);			
				memset(sta->acmac, 0, MAC_LEN);
				memset(sta->ifname, 0, ETH_LEN);
				sta->realsip = 0;
			}
		
			break;
		}

		default:
		{
			if (aat_debug >= AAT_DEBUG)
			{
				printk(KERN_DEBUG "%s-%d error: type %d not support\n", __func__, __LINE__,type);
			}
			
			return 0;
		}
	}

	if (aat_debug >= AAT_DEBUG)
	{
		printk(KERN_DEBUG "%s-%d by %s %02x:%02x:%02x:%02x:%02x:%02x %u.%u.%u.%u real %u.%u.%u.%u\n",
					__func__, __LINE__,  (AAT_STA_GET_TYPE_MAC ==type) ? "MAC": \
								( (AAT_STA_GET_TYPE_VIP==type) ?"VIP":"nil"),
								sta->stamac[0], sta->stamac[1], sta->stamac[2],
								sta->stamac[3], sta->stamac[4], sta->stamac[5],
								((sta->staip & 0xff000000) >> 24),
								((sta->staip & 0xff0000) >> 16),
								((sta->staip & 0xff00) >> 8),
								(sta->staip & 0xff),
								((sta->realsip & 0xff000000) >> 24),
								((sta->realsip & 0xff0000) >> 16),
								((sta->realsip & 0xff00) >> 8),
								(sta->realsip & 0xff));
	}

	return 1;
}

int legalUserIoctlInfo(struct io_info *sta)
{
#define IP_ADDRESS_ZERO 	0x0
#define IP_ADDRESS_BCAST	0xFFFFFFFF

	if (!sta) 
	{
		return 0;
	}

	if ((sta->staip == IP_ADDRESS_ZERO)||
			(sta->staip == IP_ADDRESS_BCAST))
	{
		return 0;
	}

	/*
	   if(sta->stamac[0] & 0x1)
	   {
	   return 0;
	   }*/

	return 1;
}

int aat_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	int op_ret = 0;
	struct sta_info *tmp = NULL;	 
	struct io_info stainfo;
	struct io_info stainfo_temp;
	memset(&stainfo_temp, 0, sizeof(struct io_info));

	op_ret = copy_from_user(&stainfo_temp, (struct io_info *)arg, sizeof(struct io_info));

	if(aat_debug > AAT_INFO)
	{
		printk("%s-%d cmd %#x %u  sta %02x:%02x:%02x:%02x:%02x:%02x\n", 
				__func__, __LINE__, cmd, stainfo_temp.seq_num,
				stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
				stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);
	}
	if (_IOC_TYPE(cmd) != AAT_IOC_MAGIC) 
	{
		printk("%s-%d error cmd %#x %u sta %02x:%02x:%02x:%02x:%02x:%02x IOC_MAGIC\n", 
				__func__, __LINE__, cmd,stainfo_temp.seq_num,
				stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
				stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);

		return -ENOTTY;
	}

	if (_IOC_NR(cmd) > AAT_IOC_MAXNR) 
	{
		printk("%s-%d error cmd %#x  %u _IOC_NR error sta %02x:%02x:%02x:%02x:%02x:%02x IOC_MAXNR\n", 
				__func__, __LINE__, cmd,stainfo_temp.seq_num,
				stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
				stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);

		return -ENOTTY;
	}

	memset(&stainfo, 0, sizeof(struct io_info));
	switch (cmd)
	{
		case AAT_IOC_ADD_STA: 
			if(aat_debug > AAT_INFO)
			{
				printk("%s-%d cmd %#x %u ADD sta %02x:%02x:%02x:%02x:%02x:%02x\n", 
						__func__, __LINE__, cmd, stainfo_temp.seq_num,
						stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
						stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);
			}

			op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			if (legalUserIoctlInfo(&stainfo)) 
			{
				spin_lock(&allif_splock);
				allif_lock = 1;
				aat_add_sta(&stainfo);			
				allif_lock = 0;
				spin_unlock(&allif_splock); 
			}
			else
			{
				printk(KERN_ERR"ioctl add sta "MACFORMAT" "IPFORMAT" illegal\n",
						format_mac(stainfo.stamac), format_ip(stainfo.staip));

			}
			stainfo.seq_num += 1;
			op_ret = copy_to_user((struct io_info *)arg, &stainfo,sizeof(struct io_info) );
			break;

		case AAT_IOC_DEL_STA: 
			if(aat_debug > AAT_INFO)
			{
				printk("%s-%d cmd %#x %u DEL sta %02x:%02x:%02x:%02x:%02x:%02x\n", 
						__func__, __LINE__, cmd, stainfo_temp.seq_num,
						stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
						stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);
			}

			op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			if(legalUserIoctlInfo(&stainfo)) 
			{
				spin_lock(&allif_splock);
				allif_lock = 1;
				aat_del_sta(&stainfo);			 
				allif_lock = 0;
				spin_unlock(&allif_splock); 
			}
			else
			{
				printk(KERN_ERR"ioctl del sta "MACFORMAT" "IPFORMAT" illegal\n",
						format_mac(stainfo.stamac), format_ip(stainfo.staip));

			}

			stainfo.seq_num += 1;
			op_ret = copy_to_user((struct io_info *)arg, &stainfo,sizeof(struct io_info) );
			break;

		case AAT_IOC_GET_STA_IP:
			op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			if (1 == allif_lock)
			{
				printk("%s-%d cmd %#x %u get sta %02x:%02x:%02x:%02x:%02x:%02x LOCKED return\n", 
						__func__, __LINE__, cmd, stainfo_temp.seq_num,
						stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
						stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);

				stainfo.staip = 0;
				copy_to_user((char*)arg, &stainfo, sizeof(struct io_info));

				break;
			}

			tmp = aat_get_stav2(&allif, stainfo.stamac, stainfo.vrrid);
			if (NULL != tmp)
			{
				if (0 == strcmp(tmp->ifname, stainfo.ifname))
				{
					stainfo.staip = tmp->realsip;
					if (aat_debug >= AAT_DEBUG)
					{
						printk(KERN_INFO "%s-%d cmd %#x %u GET sta %02x:%02x:%02x:%02x:%02x:%02x real ip %#0x\n", 
								__func__,__LINE__,cmd, stainfo_temp.seq_num,
								stainfo.stamac[0], stainfo.stamac[1], stainfo.stamac[2], 
								stainfo.stamac[3], stainfo.stamac[4], stainfo.stamac[5], stainfo.staip);			 
					}
				}
				else
				{
					if (aat_debug >= AAT_DEBUG) 
					{
						printk("%s-%d cmd %#x %u GET error sta %02x:%02x:%02x:%02x:%02x:%02x intf diff %s vs %s set real ip 0\n", 
								__func__,__LINE__,cmd, stainfo_temp.seq_num,stainfo.stamac[0], stainfo.stamac[1], stainfo.stamac[2], stainfo.stamac[3],
								stainfo.stamac[4], stainfo.stamac[5], tmp->ifname, stainfo.ifname);
					}
					stainfo.staip = 0;
				}
			}
			else
			{
				if (aat_debug >= AAT_DEBUG) 
				{
					printk("%s-%d cmd %#x %u GET error sta %02x:%02x:%02x:%02x:%02x:%02x not exist\n", 
							__func__,__LINE__,cmd, stainfo_temp.seq_num,
							stainfo.stamac[0], stainfo.stamac[1], stainfo.stamac[2], 
							stainfo.stamac[3], stainfo.stamac[4], stainfo.stamac[5]);
				}
				stainfo.staip = 0;
			}
			copy_to_user((char*)arg, &stainfo, sizeof(struct io_info));
			break;			 
		case AAT_IOC_CLEAN_STAS: 
			op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			spin_lock(&allif_splock);
			allif_lock = 1;
			retval = aat_clean_hansi_stas(&(allif.sta_list), stainfo.vrrid);
			allif_lock = 0;
			spin_unlock(&allif_splock); 

			printk("%s-%d cmd %#x %u CLEAN sta %02x:%02x:%02x:%02x:%02x:%02x vrid %d ret %d\n", 
					__func__, __LINE__, cmd, stainfo_temp.seq_num,stainfo.stamac[0], stainfo.stamac[1], stainfo.stamac[2], 
					stainfo.stamac[3], stainfo.stamac[4], stainfo.stamac[5],stainfo.vrrid,retval);
			break;

		default:

			printk("%s-%d cmd %#x %u error sta %02x:%02x:%02x:%02x:%02x:%02x\n", 
					__func__, __LINE__, cmd,stainfo_temp.seq_num,
					stainfo_temp.stamac[0], stainfo_temp.stamac[1], stainfo_temp.stamac[2], 
					stainfo_temp.stamac[3], stainfo_temp.stamac[4], stainfo_temp.stamac[5]);

			retval = -1;
			break;
	}	
	return retval;
}


static long aat_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg) 
{
	int rval;

	rval = aat_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);

	return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;
}


static int aat_open(struct inode *inode, struct file *filp)
{
	struct oct_dev_s *dev;

	dev = container_of(inode->i_cdev, struct oct_dev_s, cdev);
	filp->private_data = dev;
	dev->size = 0;
	return 0;
}


static int aat_release(struct inode *inode, struct file *file)
{
	//aat_ko_rx_hook = NULL;
	//aat_ko_tx_hook = NULL;

	if (aat_debug >= AAT_DEBUG)
	{
		printk("aat orig release done\n");
	}
	return 0;
}

static struct file_operations aat_fops = 
{
	.owner	= THIS_MODULE,
	.compat_ioctl = aat_compat_ioctl,
	.ioctl	= aat_ioctl,
	.open	= aat_open,
	.release= aat_release
};



int aat_dev_init(void)

{
	int result;
	dev_t aat_devt;

	aat_devt = MKDEV(aat_major_num, aat_minor_num);
	result = register_chrdev_region(aat_devt,1,DRIVER_NAME);
	printk(KERN_INFO DRIVER_NAME ":register major dev [%d]\n",MAJOR(aat_devt));

	if (result < 0) {
		printk(KERN_WARNING DRIVER_NAME ":register_chrdev_region err[%d]\n",result);
		return 0;
	} 
	cdev_init(&(aat_dev.cdev),&aat_fops);
	aat_dev.cdev.owner = THIS_MODULE;

	result = cdev_add(&(aat_dev.cdev),aat_devt,1);
	if (result < 0) {
		printk(KERN_WARNING DRIVER_NAME ":cdev_add err[%d]\n",result);
	} else {
		printk(KERN_INFO DRIVER_NAME ":loaded successfully.\n");
	}
	return 0;
}


int aat_dev_cleanup(void)
{
	printk(KERN_INFO DRIVER_NAME ":Unregister MajorNum[%d]\n",MAJOR(aat_dev.cdev.dev));	
	unregister_chrdev_region(aat_dev.cdev.dev,1);	
	cdev_del(&(aat_dev.cdev));
	printk(KERN_INFO DRIVER_NAME ":unloaded\n");
	return 0;
}


/**
 * Module/ driver initialization. Creates the linux network
 * devices.
 *
 * @return Zero on success
 */
static int __init aat_init_module(void)
{
	int i = 0;
	printk("any-addr-translate: %s\n", AAT_SDK_VERSION_STRING);

	spin_lock_init(&allif_splock);

	aat_ko_rx_hook = aat_kernel_rx;
	aat_ko_tx_hook = aat_kernel_tx;
	aat_dev_init();

	printk("init allif\n");
	memset(&allif, 0, sizeof(struct aat_info));
	allif.sta_list = NULL;
	for (i = 0; i < HASH_SIZE; i++)
	{
		allif.sta_hash[i] = NULL;
		allif.sta_vip_hash[i] = NULL;
	}

	/*
	   struct io_info tmp;
	   memset(&tmp , 0 , sizeof(struct io_info));
	   tmp.acmac[0]=0x00;
	   tmp.acmac[1]=0x1f;
	   tmp.acmac[2]=0x64;
	   tmp.acmac[3]=0x11;
	   tmp.acmac[4]=0x02;
	   tmp.acmac[5]=0xc6;
	   memcpy(tmp.ifname, "wlan0-1", 7);
	   tmp.stamac[0]=0x00;
	   tmp.stamac[1]=0x24;
	   tmp.stamac[2]=0xd6;
	   tmp.stamac[3]=0x50;
	   tmp.stamac[4]=0x11;
	   tmp.stamac[5]=0xde;
	   tmp.staip = 503382372;
	   aat_add_sta(&tmp);*/
	/*netlink to asd*/
	/*	printk("kthread_create\n");

		my_netlink_test = (struct task_struct *)kthread_create(netlink_with_asd, NULL, "%s", "test_netlink");

		if (IS_ERR(my_netlink_test))
		{
		printk("kthread_create failed\n");
		}

		if (my_netlink_test)
		{
		printk("kthread_create successful\n");
		wake_up_process(my_netlink_test);
		}		
		*/

	return 0;
}


/**
 * Module / driver shutdown
 *
 * @return Zero on success
 */
static void __exit aat_cleanup_module(void)
{	
	aat_dev_cleanup();

	aat_ko_rx_hook = NULL;
	aat_ko_tx_hook = NULL;

	schedule_timeout_interruptible(3 * HZ);
	if (aat_debug >= AAT_DEBUG)
	{
		printk("aat now cleanup done\n");
	}
}

#if 0
//test netlink begin
int kernel_to_asd(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	nlh = (struct nlmsghdr *)skb->data;
	memset(nlh,0,sizeof(struct nlmsghdr));
	nlh->nlmsg_len = skb->len;
	NETLINK_CB(skb).pid = 0; /* from kernel */
	NETLINK_CB(skb).dst_pid = asd_pid;

	if(aat_debug >= AAT_DEBUG)
		printk("%s skb->len:%d.\n",__func__,skb->len);
	netlink_unicast(nl_sk, skb, asd_pid, MSG_DONTWAIT);
	return 0;
}

void handle_data_msg(struct sk_buff *skb)
{
	return;
}

void nl_data_ready (struct sock *sk, int len)
{
	wake_up_interruptible(sk->sk_sleep);
}

void netlink_with_asd(void)
{
	struct sk_buff * skb = NULL;
	struct nlmsghdr * nlh = NULL;
	int err;

	nl_sk = netlink_kernel_create(NETLINK_WITH_ASD, 0,nl_data_ready,THIS_MODULE);/*creat netlink socket in kernel*/
	if(nl_sk == NULL)
	{
		printk("netlink_kernel_create failed.\n");
		return;
	}

	/* wait for message coming down from user-space */
	while(1)
	{
		skb = skb_recv_datagram(nl_sk, 0, 0, &err);
		if(skb == NULL)
			continue;
		nlh = (struct nlmsghdr *)skb->data;
		asd_pid = nlh->nlmsg_pid; /*pid of sending process */		
		if(aat_debug >= AAT_DEBUG)
			printk("Received netlink message len:%d\n", nlh->nlmsg_len);

		handle_data_msg(skb);

	}

	asd_pid = 0;
	nlh = NULL;	
	sock_release(nl_sk->sk_socket);
}
//test netlink end
#endif

MODULE_LICENSE("Autelan");
MODULE_AUTHOR("Autelan");
MODULE_DESCRIPTION("any addr translate driver.");
module_init(aat_init_module);
module_exit(aat_cleanup_module);
EXPORT_SYMBOL(aat_get_sta_info);

