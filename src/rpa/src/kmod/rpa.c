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
* rpa.c
*
*
* CREATOR:
* autelan.software.bsp&boot. team
*
* DESCRIPTION:
* rpa dev operation
*
********************************************************************************
*
* MODIFY:     revision <1.0>  luoxun@autelan.com
* 2008-8-13	Mapping memory to wsm.
* 2008-8-25	ping, DHCP
* 2008-8-28	Shared memory lock was replaced by queue.
* 2008-9-10	Thread was binded to core.	
* 2008-9-12	Queue was binded to core.	
* MODIFY:     revision <1.1>
* 2009-11-16 <guoxb> Add multi-instance for AC N+1 backup.
* 2010-01-08 <guoxb> Add ioctl WIFI_IOC_GET_V6ADDR for wid get IPv6 addr by 
*                              interface name.
*                                                  
*******************************************************************************/


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/netdevice.h>
#include <linux/mii.h>
#include <linux/netlink.h>
#include <net/sock.h> 

#include "rpa.h"
#include "rpa-includes.h"

int rpa_major_num = 245;
module_param(rpa_major_num, int, 0444);
int rpa_minor_num = 0;
module_param(rpa_minor_num,int,0444);
int rpa_rx_eth_debug = 0;
module_param(rpa_rx_eth_debug,int,0644);
int rpa_tx_eth_debug = 0;
module_param(rpa_tx_eth_debug,int,0644);
int rpa_tx_debug = 0;
module_param(rpa_tx_debug,int,0644);
int rpa_rx_debug = 0;
module_param(rpa_rx_debug,int,0644);
int rpa_arp_eth_debug = 0;
module_param(rpa_arp_eth_debug,int,0644);


rpa_tx_dev_info_t rpa_tx_dev_index[RPA_SLOT_NUM][RPA_SLOT_NUM];/* tx_dev when rpa send packet out of board */

int netdev_number[RPA_SLOT_NUM];
int rpa_local_slot_num;
int first_mcb_slot_num;
int second_mcb_slot_num;

struct sock *rpa_nl_sk = NULL;

struct rpa_dev_s {
	struct cdev cdev;
};
static struct rpa_dev_s rpa_dev;

extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
extern product_info_t autelan_product_info;
extern cvmx_bootinfo_t *octeon_bootinfo;
extern unsigned int (*cvm_rpa_rx_hook)(struct net_device *dev, struct sk_buff *skb, unsigned int flag);
extern int (*cvm_rpa_tx_hook)(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag);
extern int (*cvm_rpa_arp_bc_hook)(struct sk_buff *);
extern rpa_callback_t rpa_intercept_cb;
extern unsigned int rpa_rcv_skb_data_offset;
extern mac_addr_stored_t stored_base_mac;
extern unsigned int rpa_broadcast_mask;
extern unsigned int rpa_default_broadcast_mask;

extern int dynamic_registe_rpa(struct rpa_interface_info_s *rpa_info);
extern int dynamic_unregiste_rpa(struct rpa_interface_info_s *rpa_info);	
extern int cvm_rpa_dev_index_add(int slotNum,int netdevNum,int flag,int ifindex);
extern int cvm_rpa_dev_index_del(int slotNum,int netdevNum);
extern int cvm_rpa_dev_index_modify(int slotNum,int netdevNum,int new_flag,int new_ifindex);
extern int rpa_netif_carrier_set(char * if_name,int carrier_flag);
extern int rpa_if_modify(struct rpa_interface_info_s * rpa_info);
extern int dev_refcnt_decrement(atomic_t *v);
extern int cvm_rpa_dev_index_print(unsigned int slot);
extern int rpa_tx_dev_index_init(product_type_t product_type, board_type_t board_type);
extern int rpa_tx_dev_index_clear(void);
extern int rpa_arp_broadcast(struct sk_buff * skb);
extern int rpa_xmit_with_4094_vlan(struct sk_buff *skb, struct net_device *dev);
extern int rpa_xmit_with_4093_vlan(struct sk_buff *skb, struct net_device *dev);
extern int rpa_send_xmit_with_default_vlan(struct sk_buff *skb, struct net_device *dev, unsigned int portNum, int flag);
extern int rpa_xmit(struct sk_buff *skb, struct net_device *dev);
extern int rpa_send_xmit(struct sk_buff *skb, struct net_device *dev, unsigned int portNum, int flag);
extern int rpa_send_xmit_ax81_1x12g12s(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag);
extern int rpa_send_xmit_ax71_crsmu(struct sk_buff *skb, struct net_device *netdev, unsigned int netdevNum, int flag);
extern unsigned int rpa_recv_cb(struct net_device *dev, struct sk_buff *skb, unsigned int flag);
extern int rpa_tx_dev_index_mac_info_update(product_type_t product_type);
extern int rpa_set_broadcast_mask(unsigned int mask);
extern int rpa_notify_broadcast_mask(unsigned int action_type, unsigned int mask);


static int rpa_intf_update_mac_addr(void *addr);

/**
 * Check rpa function available on local board
 *
 * @param board_type board type defined by octeon driver
 * 
 * @return Zero on success
 */
int rpa_unavailable_board_check(board_type_t board_type)
{	
	switch (board_type) {
		/* Rpa function available on these boards */
		case AUTELAN_BOARD_AX71_CRSMU:
		case AUTELAN_BOARD_AX81_SMU:
		case AUTELAN_BOARD_AX81_SMUE:
		case AUTELAN_BOARD_AX81_AC8C:
		case AUTELAN_BOARD_AX81_AC12C:
		case AUTELAN_BOARD_AX81_1X12G12S:
		case AUTELAN_BOARD_AX81_AC_4X:
			return 0;
		/* Rpa function unavailable on these boards */
		case AUTELAN_BOARD_AX71_2X12G12S:
		case AUTELAN_BOARD_AX81_2X12G12S:
		case AUTELAN_BOARD_AX81_AC_12X:
		default:
			return -1;
	}

	return 0;
}

/**
 * send netlink message to user-space
 *
 * @param sendBuf: message farmat
 * @param msgLen : message length
 * modify:      <caojia@autelan.com>
 */
void rpa_netlink_notifier(char *sendBuf, int msgLen)     
{   
    struct sk_buff *skb;   
    struct nlmsghdr *nlh;  
	int size = NLMSG_SPACE(msgLen);
	
    if(!sendBuf || !rpa_nl_sk){   
        return;   
    }   

    /* Allocate a new sk_buffer */  
    skb = alloc_skb(size, GFP_ATOMIC);   
    if(!skb){   
        printk(KERN_ERR "netlink: alloc_skb Error.\n");   
        return;   
    }   

    /* Initialize the header of netlink message */  
    nlh = NLMSG_PUT(skb, 0, 0, 0, size-sizeof(*nlh));   
  
    NETLINK_CB(skb).pid = 0;   
    NETLINK_CB(skb).dst_group = 1;   
    
    memcpy(NLMSG_DATA(nlh), sendBuf, msgLen);
	
    /* send message by broadcast */  
    netlink_broadcast(rpa_nl_sk, skb, 0, 1, GFP_ATOMIC);   
	
    return; 
	
nlmsg_failure: 
    if(skb)
        kfree_skb(skb);
    return;
}

static void rpa_netlink_rcv(struct sk_buff *skb)
{
	int nlmsglen, skblen;
	struct nlmsghdr *nlh;
	nl_msg_head_t *head;
	netlink_msg_t *nl_msg;
	int ret;

	skblen = skb->len;
	if (skblen < sizeof(*nlh))
		return;

	nlh = nlmsg_hdr(skb);
	nlmsglen = nlh->nlmsg_len;
	if (nlmsglen < sizeof(*nlh) || skblen < nlmsglen)
		return;

	head = (nl_msg_head_t *)NLMSG_DATA(nlh);
	nl_msg = (netlink_msg_t *)(NLMSG_DATA(nlh) + sizeof(nl_msg_head_t));

	switch (nl_msg->msgType) {
		case BASE_MAC_ADDRESS_CHANGE_EVENT:
			rpa_info("Base mac address has changed, update MAC of RPA interfaces.\n");
			ret = rpa_intf_update_mac_addr(nl_msg->msgData.base_mac_addr);
			if (ret < 0) {
				rpa_warning("rpa_intf_update_mac_addr failed.\n");
			}
			break;
		case RPA_BROADCAST_MASK_ACTION_EVENT:
			if (nl_msg->msgData.broadcast_mask_action.action_type == 1) {
				rpa_info("Setting new broadmask %#x in RPA module.\n", nl_msg->msgData.broadcast_mask_action.mask);
				rpa_set_broadcast_mask(nl_msg->msgData.broadcast_mask_action.mask);
			}
			else {
				rpa_info("Request broadmask %#x from rpa-daemon.\n", rpa_broadcast_mask);
				rpa_notify_broadcast_mask(DEFAULT_MASK_NOTIFY, rpa_default_broadcast_mask);
				rpa_notify_broadcast_mask(MASK_NOTIFY, rpa_broadcast_mask);
			}
			break;
		case RPA_DEBUG_CMD_EVENT:
			if (nl_msg->msgData.debug_cmd_type == RPA_SHOW_DEV_INDEX_TABLE){
				rpa_info("RPA_DEBUG_CMD ===> RPA_SHOW_DEV_INDEX_TABLE\n");
				cvm_rpa_dev_index_print(0);
			}
			break;
		default:
			rpa_info("Unsupport netlink message type(%d) received.\n", nl_msg->msgType);
			break;
	}
		

	return ;
}


/**
 * Change the link MTU. 
 *
 * @param dev     Device to change
 * @param new_mtu The new MTU
 * @return Zero on success
 */
static int rpa_common_change_mtu(struct net_device *dev, int new_mtu)
{

    /* Limit the MTU to make sure the ethernet packets are between 64 bytes
        and 65535 bytes */
    if ((new_mtu + 14 + 4 < 64) || (new_mtu + 14 + 4 > 65392))
    {
        rpa_warning("MTU must be between %d and %d.\n", 64-14-4, 65392-14-4);
        return -EINVAL;
    }
    dev->mtu = new_mtu;
	return 0;
}

static int rpa_intf_change_mtu(int ifindex, int new_mtu)
{
	struct net_device *dev;

	dev = dev_get_by_index(&init_net, ifindex);
	if (NULL == dev)
	{
		rpa_warning("Can not find device by ifindex.\n");
        return -EINVAL;
	}
	else
	{
		dev_refcnt_decrement(&(dev)->refcnt);
	}

	return rpa_common_change_mtu(dev, new_mtu);
}

/**
 * rpa_common_set_mac_address - set the hardware MAC address for a device
 * @dev:    The device in question.
 * @addr:   Address structure to change it too.

 * Returns Zero on success
 */
static int rpa_common_set_mac_address(struct net_device *dev, void *addr)
{
	memcpy(dev->dev_addr, addr + 2, 6);

	return 0;
}

static int rpa_intf_update_mac_addr(void *addr)
{
	int slot_num, netdev_num;
	struct sockaddr sa = {0};
	unsigned char mac_addr[6];

	if (addr == NULL) {
		return -1;
	}

	for (slot_num = 0; slot_num < RPA_SLOT_NUM; slot_num++) {
		if (slot_num == rpa_local_slot_num) {
			continue;
		}

		memcpy(mac_addr, addr, 6);
		mac_addr[5] += slot_num;
		memcpy(sa.sa_data, mac_addr, 6);
		for (netdev_num = 0; netdev_num < RPA_NETDEV_NUM; netdev_num++) {
			if (cvm_rpa_dev_index[slot_num][netdev_num].netdev) {
				rpa_common_set_mac_address(cvm_rpa_dev_index[slot_num][netdev_num].netdev, &sa);
			}
		}
	}

	memcpy(stored_base_mac.mac, addr, 6);
	stored_base_mac.store_flag = 1;

	rpa_tx_dev_index_mac_info_update(autelan_product_info.product_type);

	return 0;
}

/**
 * Get the rpa-ethernet statistics
 *
 * @param dev    Device to get the statistics from
 * @return Pointer to the statistics
 */
static struct net_device_stats *rpa_common_get_stats(struct net_device *dev)
{
	//rpa_dev_private_t * priv = (rpa_dev_private_t *)dev->priv;
    return &(dev->stats);
}

/**
 * Per network device initialization
 *
 * @param dev    Device to initialize
 * @return Zero on success
 */
int rpa_common_init(struct net_device *dev)
{
	//Need MAC address?
	//char mac[6];
	//another mac base?? use octeon bootinfo temporarily
	//mac[0] = octeon_bootinfo->mac_addr_base[0];
	//mac[1] = octeon_bootinfo->mac_addr_base[1];
	//mac[2] = octeon_bootinfo->mac_addr_base[2];
	//mac[3] = octeon_bootinfo->mac_addr_base[3];
	//mac[4] = octeon_bootinfo->mac_addr_base[4];
	//mac[5] = octeon_bootinfo->mac_addr_base[5];

	//memcpy(dev->dev_addr, mac, 6);
	
	//rpa_dev_private_t *priv = (rpa_dev_private_t*)dev->priv;
	
    dev->netdev_ops->ndo_change_mtu(dev, dev->mtu);

	/* Zero out stats */
	memset(dev->netdev_ops->ndo_get_stats(dev), 0, sizeof(struct net_device_stats));

    return 0;
}

/*
*ioctl of rpa device 
*
*cmd : 
*	RPA_IOC_REG_IF : register a rpa net device with arg rpa_info
*	RPA_IOC_UNREG_IF : unregister a rpa net device with arg rpa_info
*/
int rpa_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0, op_ret = 0;
	int slotNum, netdevNum, flag, ifindex, mtu;
	struct rpa_interface_info_s rpa_if_info;
	index_table_info ioc_index_table_info;
	rpa_intf_states_update_info_t intf_states_update_info;
	rpa_netif_carrier_ioc_t rpa_netif_carrier_info;

	if (_IOC_TYPE(cmd) != RPA_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > RPA_IOC_MAXNR) return -ENOTTY;

	if (rpa_unavailable_board_check(autelan_product_info.board_type)) {
		retval = -1;
		return  retval;
	}

	memset(&rpa_if_info, 0, sizeof(struct rpa_interface_info_s));
	
	switch (cmd)
	{
		case RPA_IOC_REG_IF: 
			op_ret = copy_from_user(&rpa_if_info, (struct rpa_interface_info_s *)arg, sizeof(struct rpa_interface_info_s));
			rpa_info("Rpa dynamic registration.\n");
			retval = dynamic_registe_rpa(&rpa_if_info);
			break;
		case RPA_IOC_UNREG_IF: 
			op_ret = copy_from_user(&rpa_if_info, (struct rpa_interface_info_s *)arg, sizeof(struct rpa_interface_info_s));
			rpa_info("Rpa dynamic unregistration.\n");
			retval = dynamic_unregiste_rpa(&rpa_if_info);
			break;
		case RPA_IOC_INDEX_TABLE_ADD:
			op_ret = copy_from_user(&ioc_index_table_info, (struct cvm_rpa_dev_index_info *)arg, sizeof(struct cvm_rpa_dev_index_info));
			rpa_info("Global device index table add.\n");
			slotNum = ioc_index_table_info.slotNum;
			netdevNum = ioc_index_table_info.netdevNum;
			flag = ioc_index_table_info.flag;
			ifindex = ioc_index_table_info.ifindex;
			retval = cvm_rpa_dev_index_add(slotNum, netdevNum, flag, ifindex);
			break;
		case RPA_IOC_INDEX_TABLE_DEL:
			op_ret = copy_from_user(&ioc_index_table_info, (struct cvm_rpa_dev_index_info *)arg, sizeof(struct cvm_rpa_dev_index_info));
			rpa_info("Global device index table delete.\n");
			slotNum = ioc_index_table_info.slotNum;
			netdevNum = ioc_index_table_info.netdevNum;
			flag = ioc_index_table_info.flag;
			ifindex = ioc_index_table_info.ifindex;
			retval = cvm_rpa_dev_index_del(slotNum, netdevNum);
			break;
		case RPA_IOC_INDEX_TABLE_MDF:
			op_ret = copy_from_user(&ioc_index_table_info, (struct cvm_rpa_dev_index_info *)arg, sizeof(struct cvm_rpa_dev_index_info));
			rpa_info("Global device index table modify.\n");
			slotNum = ioc_index_table_info.slotNum;
			netdevNum = ioc_index_table_info.netdevNum;
			flag = ioc_index_table_info.flag;
			ifindex = ioc_index_table_info.ifindex;
			retval = cvm_rpa_dev_index_modify(slotNum, netdevNum, flag, ifindex);
			break;	
		case RPA_IOC_IF_CHANGE_MTU:
			op_ret = copy_from_user(&intf_states_update_info, (struct rpa_intf_states_update_info_s *)arg, sizeof(struct rpa_intf_states_update_info_s));
			rpa_info("Rpa interface change mtu.\n");
			ifindex = intf_states_update_info.ifindex;
			mtu = intf_states_update_info.mtu;
			retval = rpa_intf_change_mtu(ifindex, mtu);
			break;
		case RPA_IOC_NETIF_CARRIER_SET:
			op_ret = copy_from_user(&rpa_netif_carrier_info, (struct rpa_netif_carrier_ioc_s *)arg, sizeof(struct rpa_netif_carrier_ioc_s));
			rpa_info("Rpa set net carrier flag.\n");
			retval = rpa_netif_carrier_set(rpa_netif_carrier_info.if_name, rpa_netif_carrier_info.carrier_flag);
			break;
		case RPA_IOC_IF_MDF:
			op_ret = copy_from_user(&rpa_if_info, (struct rpa_interface_info_s *)arg, sizeof(struct rpa_interface_info_s));
			rpa_info("Rpa interface modify.\n");
			retval = rpa_if_modify(&rpa_if_info);
			break;
		case RPA_IOC_INDEX_TABLE_SHOW:
			break;
		default:
			rpa_warning("Error: invalid cmd.\n");
			retval = -1;
			break;
	}
	
	return retval;
}

  
static long rpa_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg) 
{
	int rval;

	rval = rpa_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);
	return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;
}

struct net_device_ops rpa_netdev_ops = {
	.ndo_init		= rpa_common_init,
	//.ndo_uninit		= ,
	//.ndo_open		= ,
	//.ndo_stop		= ,
	.ndo_start_xmit		= rpa_xmit,
	//.ndo_set_multicast_list	= ,
	.ndo_set_mac_address	= rpa_common_set_mac_address,
	//.ndo_do_ioctl		= ,
	.ndo_change_mtu		= rpa_common_change_mtu,
	.ndo_get_stats		= rpa_common_get_stats,
	//.ndo_poll_controller	= ,
};

struct net_device_ops rpa_netdev_on_ax81_1x12g12s_ops = {
	.ndo_init		= rpa_common_init,
	//.ndo_uninit		= ,
	//.ndo_open		= ,
	//.ndo_stop		= ,
	.ndo_start_xmit		= rpa_xmit_with_4094_vlan,
	//.ndo_set_multicast_list	= ,
	.ndo_set_mac_address	= rpa_common_set_mac_address,
	//.ndo_do_ioctl		= ,
	.ndo_change_mtu		= rpa_common_change_mtu,
	.ndo_get_stats		= rpa_common_get_stats,
	//.ndo_poll_controller	= ,
};

struct net_device_ops rpa_netdev_on_ax71_crsmu_ops = {
	.ndo_init		= rpa_common_init,
	//.ndo_uninit		= ,
	//.ndo_open		= ,
	//.ndo_stop		= ,
	.ndo_start_xmit		= rpa_xmit_with_4093_vlan,
	//.ndo_set_multicast_list	= ,
	.ndo_set_mac_address	= rpa_common_set_mac_address,
	//.ndo_do_ioctl		= ,
	.ndo_change_mtu		= rpa_common_change_mtu,
	.ndo_get_stats		= rpa_common_get_stats,
	//.ndo_poll_controller	= ,
};

static int rpa_open(struct inode *inode, struct file *filp)
{
	struct rpa_dev_s *dev;

	dev = container_of(inode->i_cdev, struct rpa_dev_s, cdev);
	filp->private_data = dev;
	
	return 0;
}
 
 
static int rpa_release(struct inode *inode, struct file *file)
{
	return 0;
}
 
static struct file_operations rpa_fops = 
{
	.owner	= THIS_MODULE,
	.compat_ioctl = rpa_compat_ioctl,
	.ioctl	= rpa_ioctl,
	.open	= rpa_open,
	.release= rpa_release
};



int rpa_dev_init(void)	
{
	 int result;
	 dev_t rpa_devt;
 
	 rpa_devt = MKDEV(rpa_major_num, rpa_minor_num);
	 rpa_info(": register major dev [%d].\n",MAJOR(rpa_devt));
	 
	 result = register_chrdev_region(rpa_devt,1,DRIVER_NAME);
	 if (result < 0) {
		 rpa_err(": register_chrdev_region err[%d].\n",result);
		 return 0;
	 } 

	 cdev_init(&(rpa_dev.cdev),&rpa_fops);
	 rpa_dev.cdev.owner = THIS_MODULE; 
	 result = cdev_add(&(rpa_dev.cdev),rpa_devt,1);
	 if (result < 0) {
		 rpa_err(": cdev_add err[%d].\n",result);
	 } else {
		 rpa_info(": loaded successfully.\n");
	 }
	 
	 return 0;
 }


int rpa_dev_cleanup(void)
{
	rpa_info(": Unregister MajorNum[%d].\n",MAJOR(rpa_dev.cdev.dev));	
	unregister_chrdev_region(rpa_dev.cdev.dev,1);	
	cdev_del(&(rpa_dev.cdev));
	rpa_info(": unloaded\n");
	return 0;
}

/**
 * Module/ driver initialization. Creates the linux network
 * devices.
 *
 * @return Zero on success
 */
static int __init rpa_init_module(void)
{	
	int rpa_slot, rpa_netdev_num;

	rpa_info(": RPA Interface Driver, " DRIVER_VERSION "\n");

	rpa_local_slot_num = autelan_product_info.board_slot_id - 1;
	if (rpa_local_slot_num < 0) {
		rpa_crit(": Get Local Slot Number Failed!\n");
	}
	else {
		rpa_info("Local Slot ID : %d\n", rpa_local_slot_num);
	}

	/* Init netlink socket in RPA module */
	rpa_nl_sk = netlink_kernel_create(&init_net,  NETLINK_RPA, 0, rpa_netlink_rcv, NULL, THIS_MODULE);   
	if(!rpa_nl_sk) {   
		rpa_err("Failed to create netlink socket.\n");   
	}
	else {
		rpa_info("RPA create netlink socket OK.\n");
	}

	/* init the cvm_rpa_dev_index table */
	for (rpa_slot = 0; rpa_slot < RPA_SLOT_NUM; rpa_slot++)
	{
		netdev_number[rpa_slot] = 1;
		for (rpa_netdev_num = 0; rpa_netdev_num < RPA_NETDEV_NUM; rpa_netdev_num++)
		{
			cvm_rpa_dev_index[rpa_slot][rpa_netdev_num].flag = -1;
			cvm_rpa_dev_index[rpa_slot][rpa_netdev_num].ifindex = -1;
			cvm_rpa_dev_index[rpa_slot][rpa_netdev_num].netdev = NULL;
		}
		rpa_arp_send_dev[rpa_slot].netdev = NULL;
		rpa_arp_send_dev[rpa_slot].netdevNum= -1;
	}
	
	rpa_dev_init(); 
	rpa_tx_dev_index_init(autelan_product_info.product_type, autelan_product_info.board_type);

	/* register the intercept_cb function */
	if (NULL == cvm_rpa_rx_hook)
	{
		cvm_rpa_rx_hook = rpa_recv_cb;
		//rpa_intercept_cb = cvm_rpa_rx_hook;
		if  (autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU)
			rpa_rcv_skb_data_offset = 0x4;
	}

	if (NULL == cvm_rpa_tx_hook)
	{
		if (autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU) {
			cvm_rpa_tx_hook = rpa_send_xmit_ax71_crsmu;
		}
		else if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S) {
			if (autelan_product_info.product_type == AUTELAN_PRODUCT_AX8800) {
				first_mcb_slot_num = 6;
				second_mcb_slot_num = 7;
			}
			else if (autelan_product_info.product_type == AUTELAN_PRODUCT_AX8610) {
				first_mcb_slot_num = 4;
				second_mcb_slot_num = 5;
			}
			else if (autelan_product_info.product_type == AUTELAN_PRODUCT_AX8606) {
				first_mcb_slot_num = 2;
				second_mcb_slot_num = 3;
			}
			else {
				first_mcb_slot_num = 0xffff;
				second_mcb_slot_num = 0xffff;
			}
			cvm_rpa_tx_hook = rpa_send_xmit_ax81_1x12g12s;
		}
		else {
			cvm_rpa_tx_hook = rpa_send_xmit;
		}
	}

	if (NULL == cvm_rpa_arp_bc_hook)
	{
		cvm_rpa_arp_bc_hook = rpa_arp_broadcast;
	}

	return 0;
}


/**
 * Module / driver shutdown
 *
 * @return Zero on success
 */
static void __exit rpa_cleanup_module(void)
{
	unsigned int slot = 0, rpa_netdev_num = 0; 

	/* release netlink socket in rpa */
	if(rpa_nl_sk != NULL) {
		sock_release(rpa_nl_sk->sk_socket);
	}

	/* Free the ethernet devices */
    for (slot = 0; (slot < RPA_SLOT_NUM) && (slot != rpa_local_slot_num); slot++)
	{	
		for (rpa_netdev_num = 0; rpa_netdev_num < RPA_NETDEV_NUM; rpa_netdev_num++)
		{
			if (cvm_rpa_dev_index[slot][rpa_netdev_num].netdev)
			{ 
				if (slot != rpa_local_slot_num)
				{
					unregister_netdev(cvm_rpa_dev_index[slot][rpa_netdev_num].netdev);
					kfree(cvm_rpa_dev_index[slot][rpa_netdev_num].netdev);
				}
				/* clean all table item in cvm_rpa_dev_index table */
				cvm_rpa_dev_index[slot][rpa_netdev_num].netdev = NULL;
				cvm_rpa_dev_index[slot][rpa_netdev_num].flag = -1;
				cvm_rpa_dev_index[slot][rpa_netdev_num].ifindex = -1;
			}
		}
		/* clean all table item in rpa_arp_send_dev table */
		rpa_arp_send_dev[slot].netdev = NULL;
		rpa_arp_send_dev[slot].netdevNum= -1;
    }

	cvm_rpa_rx_hook = NULL;
	cvm_rpa_tx_hook = NULL;
	cvm_rpa_arp_bc_hook = NULL;
 
	rpa_dev_cleanup();
	rpa_tx_dev_index_clear();
}


MODULE_LICENSE("Autelan");
MODULE_AUTHOR("Autelan");
MODULE_DESCRIPTION("Rpa ethernet driver.");
module_init(rpa_init_module);
module_exit(rpa_cleanup_module);
