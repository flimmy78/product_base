
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <linux/netlink.h>

#include "aat.h"
#include "aat_ioctl.h"
#include "aat_rx.h"
#include "aat_tx.h"

/*netlink to asd,ht add 110226*/
#define NETLINK_WITH_ASD	17
unsigned int  asd_pid = 0;
struct sock *nl_sk = NULL;
struct task_struct *my_netlink_test;
struct aat_info allif;

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

	if (aat_debug >= AAT_DEBUG)
	{
		printk(KERN_DEBUG "get sta info, get type %s\n",
				   (type == AAT_STA_GET_TYPE_MAC) ? "MAC" : 
				   ((type == AAT_STA_GET_TYPE_VIP) ? "VIP" : "no-support-type"));
	}

	switch (type)
	{
		case AAT_STA_GET_TYPE_MAC:
		{
			if (aat_debug >= AAT_DEBUG)
			{
				printk(KERN_DEBUG "get sta by mac %02x:%02x:%02x:%02x:%02x:%02x vip/rip\n",
						   sta->stamac[0], sta->stamac[1], sta->stamac[2],
						   sta->stamac[3], sta->stamac[4], sta->stamac[5]);
			}
			
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
					printk(KERN_DEBUG "no get sta %02x:%02x:%02x:%02x:%02x:%02x vip/rip\n",
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
			if (aat_debug >= AAT_DEBUG)
			{
				printk(KERN_DEBUG "get sta by vip %d.%d.%d.%d\n",
									((sta->staip & 0xff000000) >> 24),
									((sta->staip & 0xff0000) >> 16),
									((sta->staip & 0xff00) >> 8),
									(sta->staip & 0xff));
			}
			
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
					printk(KERN_DEBUG "no get sta by vip %d.%d.%d.%d\n",
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
				printk(KERN_DEBUG "get sta info not support the type %d\n", type);
			}
			
			return 0;
		}
	}

	if (aat_debug >= AAT_DEBUG)
	{
		printk(KERN_DEBUG "get sta %02x:%02x:%02x:%02x:%02x:%02x vip %d.%d.%d.%d rip %d.%d.%d.%d\n",
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

int aat_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{   
	 int retval = 0;
	 int op_ret = 0;
	 struct sta_info *tmp = NULL;	 
	 struct io_info stainfo;
	 if (_IOC_TYPE(cmd) != AAT_IOC_MAGIC) return -ENOTTY;
	 if (_IOC_NR(cmd) > AAT_IOC_MAXNR) return -ENOTTY;
	 memset(&stainfo, 0, sizeof(struct io_info));
	 switch (cmd)
	 {
		 case AAT_IOC_ADD_STA: 
		 	if(aat_debug >= AAT_DEBUG){
		        printk("%s, case AAT_IOC_ADD_STA.\n",__func__);
	        }
			 op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			 aat_add_sta(&stainfo);			 
			 break;
			 
		 case AAT_IOC_DEL_STA:
		 	if(aat_debug >= AAT_DEBUG){
		        printk("%s, case AAT_IOC_DEL_STA.\n",__func__);
	        }
			 op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			 aat_del_sta(&stainfo);			 
			 break;

		 case AAT_IOC_GET_STA_IP:
			 op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

			 tmp = aat_get_stav2(&allif, stainfo.stamac, stainfo.vrrid);
			 if (NULL != tmp)
			 {
				 stainfo.staip = tmp->realsip;
			 }
			 else
			 {
				 stainfo.staip = 0;
			 }

			 if(aat_debug >= AAT_DEBUG)
			 {
				 printk(KERN_INFO "%s: sta %02x:%02x:%02x:%02x:%02x:%02x %x\n", __func__,
				 	stainfo.stamac[0], stainfo.stamac[1], stainfo.stamac[2], 
				 	stainfo.stamac[3], stainfo.stamac[4], stainfo.stamac[5], stainfo.staip);
			 }
			 copy_to_user((struct io_info*)arg, &stainfo, sizeof(struct io_info));
			 break;			 
		 case AAT_IOC_CLEAN_STAS: 
			 op_ret = copy_from_user(&stainfo, (struct io_info *)arg, sizeof(struct io_info));

		 	 printk(KERN_INFO "AAT get message to clean hansi %d STAs\n", stainfo.vrrid);
			 retval = aat_clean_hansi_stas(&(allif.sta_list), stainfo.vrrid);
		 	 printk(KERN_INFO "AAT get message to clean hansi %d STAs %s\n",
		 	 					stainfo.vrrid, retval == 0 ? "success" : "fail");
			 
			 break;

		 default:
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
	printk("any-addr-translate: %s\n", AAT_SDK_VERSION_STRING);

	aat_ko_rx_hook = aat_kernel_rx;
	aat_ko_tx_hook = aat_kernel_tx;
	aat_dev_init();
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
/*
//test netlink begin
int kernel_to_asd(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	nlh = (struct nlmsghdr *)skb->data;
	memset(nlh,0,sizeof(struct nlmsghdr));
	nlh->nlmsg_len = skb->len;
	NETLINK_CB(skb).pid = 0; //from kernel 
	NETLINK_CB(skb).dst_pid = asd_pid;
	
	if(aat_debug >= AAT_DEBUG)
		printk("%s skb->len:%d.\n",__func__,skb->len);
	netlink_unicast(nl_sk, skb, asd_pid, MSG_DONTWAIT);
}
*/
void handle_data_msg(struct sk_buff *skb)
{

}

void nl_data_ready (struct sock *sk, int len)
{
	wake_up_interruptible(sk->sk_sleep);
}

/*
void netlink_with_asd(void)
{
	struct sk_buff * skb = NULL;
	struct nlmsghdr * nlh = NULL;
	int err;
	
	nl_sk = netlink_kernel_create(NETLINK_WITH_ASD, 0,nl_data_ready,THIS_MODULE);//creat netlink socket in kernel
	if(nl_sk == NULL)
	{
		printk("netlink_kernel_create failed.\n");
		return;
	}
	
	//wait for message coming down from user-space 
	while(1)
	{
		skb = skb_recv_datagram(nl_sk, 0, 0, &err);
		if(skb == NULL)
			continue;
		nlh = (struct nlmsghdr *)skb->data;
		asd_pid = nlh->nlmsg_pid; //pid of sending process 
		if(aat_debug >= AAT_DEBUG)
			printk("Received netlink message len:%d\n", nlh->nlmsg_len);
		
		handle_data_msg(skb);

	}
	
	asd_pid = 0;
	nlh = NULL;	
	sock_release(nl_sk->sk_socket);
}

//test netlink end
*/

MODULE_LICENSE("Autelan");
MODULE_AUTHOR("Autelan");
MODULE_DESCRIPTION("any addr translate driver.");
module_init(aat_init_module);
module_exit(aat_cleanup_module);
EXPORT_SYMBOL(aat_get_sta_info);

