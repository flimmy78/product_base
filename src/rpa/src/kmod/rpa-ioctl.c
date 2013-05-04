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
* rpa-ioctl.c
*
* CREATOR:
* autelan.software.bsp&boot. team
*
* DESCRIPTION:
* rpa-ethernet module ioctl interface
*
*******************************************************************************/

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/if.h>
#include <net/if_inet6.h>
#include <linux/etherdevice.h>
#include <linux/vmalloc.h>
#include <linux/mii.h>
#include <linux/inetdevice.h>
#include <asm/atomic.h>

#include "rpa.h"
#include "rpa-includes.h"

#define PRODUCT_AX7605I 0x1
#define PRODUCT_AX8610 0x4
#define PRODUCT_AX8606 0x5
#define PRODUCT_AX8800 	0x7

static unsigned char obc_mac_base[6] = {0x00, 0x1f, 0x64, 0xff, 0xff, 0xf0};
mac_addr_stored_t stored_base_mac;

extern struct net_device *cvm_oct_device[];
extern cvm_rpa_netdev_index_t cvm_rpa_dev_index[RPA_SLOT_NUM][RPA_NETDEV_NUM];
extern rpa_arp_send_netdev_t rpa_arp_send_dev[RPA_SLOT_NUM];
extern int rpa_local_slot_num;
extern int first_mcb_slot_num;
extern int second_mcb_slot_num;
extern rpa_tx_dev_info_t rpa_tx_dev_index[RPA_SLOT_NUM][RPA_SLOT_NUM];
extern product_info_t autelan_product_info;
extern cvmx_bootinfo_t *octeon_bootinfo;
extern product_info_t  product_info;
extern int netdev_number[RPA_SLOT_NUM];
extern struct net_device_ops rpa_netdev_ops;
extern struct net_device_ops rpa_netdev_on_ax81_1x12g12s_ops;
extern struct net_device_ops rpa_netdev_on_ax71_crsmu_ops;
extern unsigned int rpa_broadcast_mask;
extern unsigned int rpa_default_broadcast_mask;

/* Defined in ethernet.c */
extern uint32_t (*cvm_rpa_rx_hook) (struct sk_buff *);
extern int rpa_common_init(struct net_device *dev);
extern int cvm_get_slot_mac_addr(unsigned char * mac_addr,unsigned int slot_num);
extern void rpa_netlink_notifier(char *sendBuf, int msgLen);

int dev_refcnt_decrement(atomic_t *v)
{
	v->counter -= 1;

	return 0;
}

int rpa_netif_carrier_set(char *if_name, int carrier_flag)
{
	struct net_device *dev = NULL;

	dev = dev_get_by_name(&init_net, if_name);
	if (dev)
	{
		if (!(dev->priv_flags & IFF_RPA))
		{
			printk("Not rpa interface, can't set carrier flag by rpa.\n");
			dev_refcnt_decrement(&(dev)->refcnt);

			return -1;
		}
		if (carrier_flag == RPA_NETIF_CARRIER_ON)
		{
			netif_carrier_on(dev);

			dev_refcnt_decrement(&(dev)->refcnt);

			return 0;
		}
		else if (carrier_flag == RPA_NETIF_CARRIER_OFF)
		{
			if (netif_carrier_ok(dev))
			{
				netif_carrier_off(dev);
			}

			dev_refcnt_decrement(&(dev)->refcnt);

			return 0;
		}
		else
		{
			printk("Wrong carrier flag.\n");
			dev_refcnt_decrement(&(dev)->refcnt);

			return -1;
		}
	}
	else
	{
		printk("Invalid interface name.\n");

		return -1;
	}
	
	return 0;
}

int rpa_get_slot_mac_addr(unsigned char *mac_addr, unsigned int slotNum)
{
#if 0
	if (stored_base_mac.store_flag) {
		mac_addr[0] = stored_base_mac.mac[0];
		mac_addr[1] = stored_base_mac.mac[1];
		mac_addr[2] = stored_base_mac.mac[2];
		mac_addr[3] = stored_base_mac.mac[3];
		mac_addr[4] = stored_base_mac.mac[4];
		mac_addr[5] = stored_base_mac.mac[5] + slotNum;
	}
	else if(0 == cvm_get_slot_mac_addr(mac_addr, slotNum + 1)) {
       		return 0;
	}
	else if (((autelan_product_info.product_series_num == 7) && (rpa_local_slot_num == 0 || rpa_local_slot_num == 1)) ||
		((autelan_product_info.product_type == AUTELAN_PRODUCT_AX8610) && (rpa_local_slot_num == 4 || rpa_local_slot_num == 5)) ||
		((autelan_product_info.product_type == AUTELAN_PRODUCT_AX8800) && (rpa_local_slot_num == 6 || rpa_local_slot_num == 7))) {
		mac_addr[0] = octeon_bootinfo->mac_addr_base[0];
		mac_addr[1] = octeon_bootinfo->mac_addr_base[1];
		mac_addr[2] = octeon_bootinfo->mac_addr_base[2];
		mac_addr[3] = octeon_bootinfo->mac_addr_base[3];
		mac_addr[4] = octeon_bootinfo->mac_addr_base[4];
		mac_addr[5] = octeon_bootinfo->mac_addr_base[5] + slotNum;

		return 0;
	}
	else {
		mac_addr[0] = obc_mac_base[0];
		mac_addr[1] = obc_mac_base[1];
		mac_addr[2] = obc_mac_base[2];
		mac_addr[3] = obc_mac_base[3];
		mac_addr[4] = obc_mac_base[4];
		mac_addr[5] = obc_mac_base[5] + slotNum;
		
		return 0;
	}
#endif

	mac_addr[0] = obc_mac_base[0];
	mac_addr[1] = obc_mac_base[1];
	mac_addr[2] = obc_mac_base[2];
	mac_addr[3] = obc_mac_base[3];
	mac_addr[4] = obc_mac_base[4];
	mac_addr[5] = obc_mac_base[5] + slotNum;

	return 0;
}

int rpa_tx_dev_index_print(void)
{
	int s_slot, d_slot, i;

	for (s_slot = 0; s_slot < RPA_SLOT_NUM; s_slot++)
	{
		for (d_slot = 0; d_slot < RPA_SLOT_NUM; d_slot++)
		{
			if (rpa_tx_dev_index[s_slot][d_slot].ifindex > -1)
			{
				printk("rpa_tx_dev_index[%d][%d].tx_dev : %s\n", s_slot, d_slot, rpa_tx_dev_index[s_slot][d_slot].tx_dev->name);
				printk("Dest_mac_addr : ");
				for (i = 0; i < 6; i++)
				{
					printk("%x", rpa_tx_dev_index[s_slot][d_slot].dest_mac_addr[i]);
				}
				printk("\n");
			}
		}
	}

	return 0;
}

/**
 * rpa_tx_dev_index_mac_info_update - mac info in struct rpa_tx_dev_index update
 * @product_type:    Local product type inited by octeon driver
 *
 * Returns Zero on success
 */
int rpa_tx_dev_index_mac_info_update(product_type_t product_type)
{
	int d_slot;
	int effective_slot_num;

	switch (product_type) {
		case AUTELAN_PRODUCT_AX7605I:
			effective_slot_num = 3;
			break;
		case AUTELAN_PRODUCT_AX8800:
			effective_slot_num = 14;
			break;
		case AUTELAN_PRODUCT_AX8610:
			effective_slot_num = 10;
			break;
		case AUTELAN_PRODUCT_AX8606:
			effective_slot_num = 6;
			break;
		case AUTELAN_PRODUCT_AX8603:
			effective_slot_num = 3;
			break;
		default:
			return -1;
			break;
	}

	for (d_slot = 0; d_slot < effective_slot_num; d_slot++) {
		rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][d_slot].dest_mac_addr, d_slot);
	}
	
	return 0;
}

/**
 * rpa_tx_dev_index_init_ax7605i - struct rpa_tx_dev_index initialization for ax7605i
 * @product_type:    Local product type inited by octeon driver
 * @board_type:    Local board type inited by octeon driver
 *
 * Returns Zero on success
 */
int rpa_tx_dev_index_init_ax7605i(product_type_t product_type, board_type_t board_type)
{
	struct net_device *dev;
	int d_slot;
	int result = 0;
	
	rpa_info(" rpa_tx_dev_index_init......(AX7605I)\n");
	switch (board_type) {
		case AUTELAN_BOARD_AX71_CRSMU:
			dev = dev_get_by_name(&init_net, "obc0");
			if (dev) {
				for (d_slot = 0; d_slot < 3; d_slot++) {
					if (d_slot == rpa_local_slot_num)
						continue;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].ifindex = dev->ifindex;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].tx_dev = dev;
					rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][d_slot].dest_mac_addr, d_slot);
				}
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}
			break;
		case AUTELAN_BOARD_AX71_2X12G12S:
		default:
			break;
	}

	return result;
}

/**
 * rpa_tx_dev_index_init_ax8603 - struct rpa_tx_dev_index initialization for ax8603
 * @product_type:    Local product type inited by octeon driver
 * @board_type:    Local board type inited by octeon driver
 *
 * Returns Zero on success
 */
int rpa_tx_dev_index_init_ax8603(product_type_t product_type, board_type_t board_type)
{
	struct net_device *dev;
	char ve[16];
	int d_slot;
	int result = 0;
	
	rpa_info(" rpa_tx_dev_index_init......(AX8603)\n");
	switch (board_type) {
		case AUTELAN_BOARD_AX81_AC12C:
			sprintf(ve, "ve%02df1", rpa_local_slot_num + 1);
			dev = dev_get_by_name(&init_net, ve);
			if (dev) {
				for (d_slot = 0; d_slot < 3; d_slot++) {
					if (d_slot == rpa_local_slot_num)
						continue;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].ifindex = dev->ifindex;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].tx_dev = dev;
					rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][d_slot].dest_mac_addr, d_slot);
				}
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}
			break;
		case AUTELAN_BOARD_AX81_2X12G12S:
		default:
			break;
	}

	return result;
}

/**
 * rpa_tx_dev_index_init_ax86xx - struct rpa_tx_dev_index initialization for ax8610 and ax8606
 * @product_type:    Local product type inited by octeon driver
 * @board_type:    Local board type inited by octeon driver
 *
 * Returns Zero on success
 */
int rpa_tx_dev_index_init_ax86xx(product_type_t product_type, board_type_t board_type)
{
	struct net_device *dev;
	char ve[16];
	int d_slot;
	int result = 0;
	int effective_slot_num, first_mcb_slot, second_mcb_slot;
	if (product_type == AUTELAN_PRODUCT_AX8800) {
		effective_slot_num = 14;
		first_mcb_slot = 6;
		second_mcb_slot = 7;
		rpa_info(" rpa_tx_dev_index_init......(AX8800)\n");
	}
	else if (product_type == AUTELAN_PRODUCT_AX8610) {
		effective_slot_num = 10;
		first_mcb_slot = 4;
		second_mcb_slot = 5;
		rpa_info(" rpa_tx_dev_index_init......(AX8610)\n");
	}
	else if (product_type == AUTELAN_PRODUCT_AX8606) {
		effective_slot_num = 6;
		first_mcb_slot = 2;
		second_mcb_slot = 3;
		rpa_info(" rpa_tx_dev_index_init......(AX8606)\n");
	}
	else {
		result = -1;
		return result;
	}

	switch (board_type) {
		case AUTELAN_BOARD_AX81_AC8C:
		case AUTELAN_BOARD_AX81_AC12C:
		case AUTELAN_BOARD_AX81_1X12G12S:
		case AUTELAN_BOARD_AX81_AC_4X:
			sprintf(ve, "ve%02df1", rpa_local_slot_num + 1);
			dev = dev_get_by_name(&init_net, ve);
		
			if (dev) {
				for (d_slot = 0; d_slot < effective_slot_num; d_slot++) {
					if (d_slot == rpa_local_slot_num ||
						d_slot == first_mcb_slot ||
						d_slot == second_mcb_slot)
						continue;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].ifindex = dev->ifindex;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].tx_dev = dev;
					rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][d_slot].dest_mac_addr, d_slot);
				}
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}
			dev = dev_get_by_name(&init_net, "obc0");
			if (dev) {
				rpa_tx_dev_index[rpa_local_slot_num][first_mcb_slot].ifindex = dev->ifindex;
				rpa_tx_dev_index[rpa_local_slot_num][first_mcb_slot].tx_dev = dev;
				rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][first_mcb_slot].dest_mac_addr, first_mcb_slot);
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}

			dev = dev_get_by_name(&init_net, "obc1");
			if (dev) {
				rpa_tx_dev_index[rpa_local_slot_num][second_mcb_slot].ifindex = dev->ifindex;
				rpa_tx_dev_index[rpa_local_slot_num][second_mcb_slot].tx_dev = dev;
				rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][second_mcb_slot].dest_mac_addr, second_mcb_slot);
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}
			break;
		case AUTELAN_BOARD_AX81_SMU:
			dev = dev_get_by_name(&init_net, "obc0");
			if (dev) {
				for (d_slot = 0; d_slot < effective_slot_num; d_slot++) {
					if (d_slot == rpa_local_slot_num)
						continue;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].ifindex = dev->ifindex;
					rpa_tx_dev_index[rpa_local_slot_num][d_slot].tx_dev = dev;
					rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][d_slot].dest_mac_addr, d_slot);
				}
				dev_refcnt_decrement(&(dev)->refcnt);
			}
			else {
				result = -1;
			}
			break;
		case AUTELAN_BOARD_AX81_2X12G12S:
		case AUTELAN_BOARD_AX81_AC_12X:
		default:
			break;
	}

	return result;
}

/**
 * rpa_tx_dev_index_init - struct rpa_tx_dev_index initialization
 * @product_type:    Local product type inited by octeon driver
 * @board_type:    Local board type inited by octeon driver
 *
 * Returns Zero on success
 */
int rpa_tx_dev_index_init(product_type_t product_type, board_type_t board_type)
{
	int s_slot, d_slot;
	int ret;

	for (s_slot = 0; s_slot < RPA_SLOT_NUM; s_slot++)
	{
		for (d_slot = 0; d_slot < RPA_SLOT_NUM; d_slot++)
		{
			rpa_tx_dev_index[s_slot][d_slot].ifindex = -1;
			rpa_tx_dev_index[s_slot][d_slot].tx_dev = NULL;
		}
	}

	if (rpa_local_slot_num < 0)
	{
		return -1;
	}

	rpa_info("rpa_local_slot_num : %d\n", rpa_local_slot_num);

	ret = rpa_get_slot_mac_addr(rpa_tx_dev_index[rpa_local_slot_num][rpa_local_slot_num].dest_mac_addr, rpa_local_slot_num);
	if (ret != 0)
	{
		rpa_tx_dev_index[rpa_local_slot_num][rpa_local_slot_num].ifindex = -1;

		return -1;
	}

	switch (product_type) {
		case AUTELAN_PRODUCT_AX7605I:
			ret = rpa_tx_dev_index_init_ax7605i(product_type, board_type);
			break;
		case AUTELAN_PRODUCT_AX8800:
		case AUTELAN_PRODUCT_AX8610:
		case AUTELAN_PRODUCT_AX8606:
			ret = rpa_tx_dev_index_init_ax86xx(product_type, board_type);
			break;
		case AUTELAN_PRODUCT_AX8603:
			ret = rpa_tx_dev_index_init_ax8603(product_type, board_type);
			break;
		default:
			break;
	}

	return ret;
}

/*
*clean the rpa tx_dev index table
*
*/
int rpa_tx_dev_index_clear(void)
{
	int s_slot, d_slot;

	for (s_slot = 0; s_slot < RPA_SLOT_NUM; s_slot++)
	{
		for (d_slot = 0; d_slot < RPA_SLOT_NUM; d_slot++)
		{
			rpa_tx_dev_index[s_slot][d_slot].ifindex = -1;
			rpa_tx_dev_index[s_slot][d_slot].tx_dev = NULL;
		}
	}

	return 0;
}

int cvm_rpa_dev_index_print(unsigned int slot)
{
	int slotNum = 0, netdevNum = 0;

	if (slot == 0)
	{
		printk("slot\tport\tindex\n");
		for (slotNum = 0; slotNum < RPA_SLOT_NUM; slotNum++)
		{
			for (netdevNum = 0; netdevNum < RPA_NETDEV_NUM; netdevNum++)
			{
				if (cvm_rpa_dev_index[slotNum][netdevNum].netdev)
				{
					printk("%4d\t%4d\t%4d\n", slotNum, netdevNum, cvm_rpa_dev_index[slotNum][netdevNum].netdev->ifindex);
				}
			}
		}
	}
	else if (slot > 0)
	{
		slotNum = slot - 1;
		printk("slot\tport\tindex\n");
		for (netdevNum = 0; netdevNum < RPA_NETDEV_NUM; netdevNum++)
		{
			if (cvm_rpa_dev_index[slotNum][netdevNum].netdev)
			{
				printk("%4d\t%4d\t%4d\n", slotNum, netdevNum, cvm_rpa_dev_index[slotNum][netdevNum].netdev->ifindex);
			}
		}
	}
	else
	{
	}

	return 0;
}

/*
*add table item
*
*/
int cvm_rpa_dev_index_add(int slotNum, int netdevNum, int flag, int ifindex)
{
	struct net_device *dev = NULL;
	int i = 0, j = 0, full_count = 0;
	int retval = 0;
	
	if ((flag < 0) || (ifindex < 0))
	{
		return -1;
	}

	if ((slotNum < 0) || (slotNum >= RPA_SLOT_NUM))
	{
		printk("Illegal slot number.\n");
		return -1;
	}

	for (j = 0; j < RPA_SLOT_NUM; j++)
	{
		for (i = 0; i < RPA_NETDEV_NUM; i++)
		{
			if (cvm_rpa_dev_index[j][i].ifindex == ifindex)
			{
				printk("The ifindex already exist in table.\n");
				return -1;
			}
		}
	}
	
	if (netdevNum == 0)
	{
		dev = dev_get_by_index(&init_net, ifindex);
		if (!(dev))
		{
			printk("Can not found net device by ifindex.\n");
			return -1;
		}

		while (cvm_rpa_dev_index[slotNum][netdev_number[slotNum]].netdev)
		{
			if (netdev_number[slotNum] == RPA_NETDEV_NUM)
			{
				if (full_count > 0)
				{
					printk("RPA Netdev Index Table is out of range!\n");
					return -1;
				}
				netdev_number[slotNum] = 1;
				full_count++;
				continue;
			}
			netdev_number[slotNum] += 1;
		}
		printk("netdev_number[%d] now is %d\n", slotNum, netdev_number[slotNum]);

		cvm_rpa_dev_index[slotNum][netdev_number[slotNum]].flag = flag;
		cvm_rpa_dev_index[slotNum][netdev_number[slotNum]].ifindex = ifindex;
		cvm_rpa_dev_index[slotNum][netdev_number[slotNum]].netdev = dev;
		retval = netdev_number[slotNum];
		netdev_number[slotNum] += 1;
		dev_refcnt_decrement(&(dev)->refcnt);
		printk("%s add in this table.\n", dev->name);
		cvm_rpa_dev_index_print(0);
		
		return retval;
	}
	else if (netdevNum > 0)
	{
		if (netdevNum >= RPA_NETDEV_NUM)
		{
			printk("Illegal netdevice number.\n");
			return -1;
		}

		dev = dev_get_by_index(&init_net, ifindex);
		if (!(dev))
		{
			printk("Can not found net device by ifindex.\n");
			return -1;
		}

		if (NULL == cvm_rpa_dev_index[slotNum][netdevNum].netdev)
		{
			cvm_rpa_dev_index[slotNum][netdevNum].flag = flag;
			cvm_rpa_dev_index[slotNum][netdevNum].ifindex = ifindex;
			cvm_rpa_dev_index[slotNum][netdevNum].netdev = dev;
			dev_refcnt_decrement(&(dev)->refcnt);
			printk("%s add in this table.\n", dev->name);
			cvm_rpa_dev_index_print(0);
			return netdevNum;
		}
		else
		{
			printk("The entry has been occupied.\n");
			dev_refcnt_decrement(&(dev)->refcnt);
			return -1;
		}
	}
	else
	{
		return -1;
	}

}

/*
*delete table item
*
*/
int cvm_rpa_dev_index_del(int slotNum, int netdevNum)
{
	cvm_rpa_dev_index[slotNum][netdevNum].flag = -1;
	cvm_rpa_dev_index[slotNum][netdevNum].ifindex = -1;
	cvm_rpa_dev_index[slotNum][netdevNum].netdev = NULL;

	cvm_rpa_dev_index_print(0);

	return 0;
}

/*
*change table item
*
*/
int cvm_rpa_dev_index_modify(int slotNum, int netdevNum, int new_flag, int new_ifindex)
{
	struct net_device *dev = NULL;
	if ((new_flag < 0) || (new_ifindex < 0))
	{
		return -1;
	}

	dev = dev_get_by_index(&init_net, new_ifindex);
	if (!(dev))
	{
		return -1;
	}
	
	cvm_rpa_dev_index[slotNum][netdevNum].flag = new_flag;
	cvm_rpa_dev_index[slotNum][netdevNum].ifindex = new_ifindex;
	cvm_rpa_dev_index[slotNum][netdevNum].netdev = dev;

	dev_refcnt_decrement(&(dev)->refcnt);
	
	return 0;
}

int rpa_notify_broadcast_mask(unsigned int action_type, unsigned int mask)
{
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	char sendBuf[512] = {0};
	int msgLen = 0;

	if ((action_type != MASK_NOTIFY) && (action_type != DEFAULT_MASK_NOTIFY)) {
		pr_info("error action type for notifying broadcast mask.\n");
	}
	
	head = (nl_msg_head_t*)sendBuf;
	head->type = LOCAL_BOARD;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = 0;

	nl_msg = (netlink_msg_t *)(sendBuf + sizeof(nl_msg_head_t));
	nl_msg->msgType = RPA_BROADCAST_MASK_ACTION_EVENT;
	nl_msg->msgData.broadcast_mask_action.action_type = action_type;
	nl_msg->msgData.broadcast_mask_action.mask = mask;	
	msgLen = sizeof(netlink_msg_t) + sizeof(nl_msg_head_t);

	rpa_netlink_notifier(sendBuf, msgLen);

	return 0;
}

int rpa_set_broadcast_mask(unsigned int mask)
{
	rpa_broadcast_mask = mask;
	rpa_notify_broadcast_mask(MASK_NOTIFY, mask);

	return 0;
}

int dynamic_registe_rpa(struct rpa_interface_info_s *rpa_if_info)
{
	unsigned short netdevNum = 0, slot = 0;
	struct net_device *dev = NULL;
	struct net_device *temp_dev = NULL;
	int full_count = 0;
	char mac[6];
	unsigned char mac_addr[6];

	/*
	int i = 0;
	for (i = 0; i < 128; i++)
	{
		if (cvm_oct_device[i])
			printk("index : %d name : %s\n", cvm_oct_device[i]->ifindex, cvm_oct_device[i]->name);
	}
	*/
	
	if (NULL == rpa_if_info) {
		return -1;
	}
	rpa_info("dynamic_registe_rpa: name=%s.\n", rpa_if_info->if_name);

	temp_dev = dev_get_by_name(&init_net, rpa_if_info->if_name);
	if (temp_dev != NULL)
	{
		rpa_info("the interface name already exists.\n");
		dev_refcnt_decrement(&(temp_dev)->refcnt);
		return -1;
	}

	slot = rpa_if_info->slotNum;
	netdevNum = rpa_if_info->netdevNum;
	if (netdevNum >= RPA_NETDEV_NUM || slot >= RPA_SLOT_NUM) {
		printk(KERN_INFO "Invalid interface: slot(%d), netdevNum(%d).\n", slot, netdevNum);
		return -1;
	}
	if (netdevNum == 0)
	{
		while (cvm_rpa_dev_index[slot][netdev_number[slot]].netdev)
		{
			if (netdev_number[slot] == RPA_NETDEV_NUM)
			{
				if (full_count > 0)
				{
					rpa_warning("RPA Netdev Index Table is out of range!\n");
					return -1;
				}
				netdev_number[slot] = 1;
				full_count++;
				continue;
			}
			netdev_number[slot] += 1;
		}
		netdevNum = netdev_number[slot];
		netdev_number[slot] += 1;
	}
	
	if (cvm_rpa_dev_index[slot][netdevNum].netdev) {
		rpa_info("The rpa-device has been already registered. slot(%d), port(%d).\n", slot, netdevNum);
		return -1;
	}

	/* update mac info */
	rpa_tx_dev_index_mac_info_update(autelan_product_info.product_type);
		
	rpa_tx_dev_index_print();

	if ((autelan_product_info.product_series_num == 0x7) && (!memcmp(rpa_if_info->if_name, "ve", 2)) && \
		(rpa_if_info->if_name[2] > '0') && (rpa_if_info->if_name[2] < '4') && (rpa_if_info->if_name[3] == '\0'))
	{
		mac[0] = obc_mac_base[0];
		mac[1] = obc_mac_base[1];
		mac[2] = obc_mac_base[2];
		mac[3] = obc_mac_base[3];
		mac[4] = obc_mac_base[4];
		mac[5] = obc_mac_base[5] + rpa_if_info->slotNum;
	}
	else if (stored_base_mac.store_flag) {
		mac[0] = stored_base_mac.mac[0];
		mac[1] = stored_base_mac.mac[1];
		mac[2] = stored_base_mac.mac[2];
		mac[3] = stored_base_mac.mac[3];
		mac[4] = stored_base_mac.mac[4];
		mac[5] = stored_base_mac.mac[5] + rpa_if_info->slotNum;
	}
	else if(0 == cvm_get_slot_mac_addr(mac_addr, slot+1))
	{
		mac[0] = mac_addr[0];
		mac[1] = mac_addr[1];
		mac[2] = mac_addr[2];
		mac[3] = mac_addr[3];
		mac[4] = mac_addr[4];
		mac[5] = mac_addr[5];
	}
	else
	{
		mac[0] = octeon_bootinfo->mac_addr_base[0];
		mac[1] = octeon_bootinfo->mac_addr_base[1];
		mac[2] = octeon_bootinfo->mac_addr_base[2];
		mac[3] = octeon_bootinfo->mac_addr_base[3];
		mac[4] = octeon_bootinfo->mac_addr_base[4];
		mac[5] = octeon_bootinfo->mac_addr_base[5] + slot;
	}
	
	dev = alloc_etherdev(sizeof(rpa_dev_private_t));
	if(dev)
	{
		rpa_dev_private_t* priv = (rpa_dev_private_t *)netdev_priv(dev);
		memset(priv, 0, sizeof(rpa_dev_private_t));
	
		SET_MODULE_OWNER(dev);
		if ((rpa_if_info->mtu + 14 + 4 < 64) || (rpa_if_info->mtu + 14 + 4 > 65392))
	    {
	        printk("MTU must be between %d and %d.\n", 64-14-4, 65392-14-4);
			dev->mtu = 1500;
	    }
		else
		{
			dev->mtu = rpa_if_info->mtu;
		}
		dev->tx_queue_len = 0;
		dev->priv_flags |= IFF_RPA;
		
		memcpy(dev->name, rpa_if_info->if_name, ETH_LEN);

		memcpy(dev->dev_addr, rpa_if_info->mac_addr, 6);
		memcpy(dev->dev_addr, mac, 6);

		priv->dslotNum = slot;
		priv->dnetdevNum = netdevNum;
		priv->sslotNum = rpa_local_slot_num;
		priv->type = rpa_if_info->type;

		

		/*add number of the device which is responsible for send and receive RPA package
		 *add number of the port which is responsible for send and receive RPA package
		 *
		 *when type is 3
		 *be used to add DSAtag when sending package
		 */
		if (priv->type == 0)
		{
			priv->ddevNum = rpa_if_info->devNum;
			priv->ddev_portNum = rpa_if_info->dev_portNum;
		}

		if (autelan_product_info.board_type == AUTELAN_BOARD_AX71_CRSMU) {
			dev->netdev_ops = &rpa_netdev_on_ax71_crsmu_ops;
		}
		else if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S && 
			(slot != first_mcb_slot_num || slot != second_mcb_slot_num)) {
			dev->netdev_ops = &rpa_netdev_on_ax81_1x12g12s_ops;
		}
		else {
			dev->netdev_ops = &rpa_netdev_ops;
		}

		/*
		if (NULL != cvm_rpa_rx_hook)
		{
        	priv->intercept_cb = cvm_rpa_rx_hook;
		}
		*/
	
		if (register_netdev(dev)<0) {
			printk(KERN_INFO "Failed to register ethernet device for rpa.\n");
			kfree(dev);
			return -1;
		}
		else 
		{
			rtnl_lock();
			dev_open(dev);  //for RUNNING state.
			rtnl_unlock();
			if (netif_carrier_ok(dev))
			{
			 if((strncmp(dev->name,"r",1)== 0)||(strncmp(dev->name,"wlan",4)== 0)||(strncmp(dev->name,"ebr",3)== 0))
			 	{
			 		printk(KERN_INFO " %s not go to unset running flags .\n",dev->name);
			 	}
			 else
			 	{
					netif_carrier_off(dev);
			 		printk(KERN_INFO " %s go to unset running flags .\n",dev->name);
			 	}
			}
			cvm_rpa_dev_index[slot][netdevNum].netdev = dev;
			cvm_rpa_dev_index[slot][netdevNum].ifindex = dev->ifindex;
			if (!rpa_arp_send_dev[slot].netdev && dev)
			{
				printk("rpa_arp_send_dev!\n");
				rpa_arp_send_dev[slot].netdev = dev;
				rpa_arp_send_dev[slot].netdevNum = netdevNum;
				rpa_default_broadcast_mask |= (1<<slot);
				rpa_broadcast_mask = rpa_default_broadcast_mask;
				rpa_info("rpa_broadcast_mask now is %#x\n", rpa_broadcast_mask);
			}
			cvm_rpa_dev_index_print(0);
		}
	}else {
		printk(KERN_INFO "Failed to allocate ethernet device for rpa.\n");
		return -1;
	}

	return netdevNum;
}


int dynamic_unregiste_rpa(struct rpa_interface_info_s *rpa_if_info)
{
	unsigned short slot = 0, netdevNum = 0;
	int tempport = 0;

	if (NULL == rpa_if_info) {
		return -1;
	}
	
	slot = rpa_if_info->slotNum;
	netdevNum = rpa_if_info->netdevNum;

	if ((slot >= RPA_SLOT_NUM) || (netdevNum >= RPA_NETDEV_NUM))
	{
		printk(KERN_INFO "Invalid interface: slot(%d), netdevNum(%d).\n", slot, netdevNum);
		return -1;
	}

	/* clean the table item in cvm_rpa_dev_index table */
	if (cvm_rpa_dev_index[slot][netdevNum].netdev) {
		unregister_netdev(cvm_rpa_dev_index[slot][netdevNum].netdev);
		kfree(cvm_rpa_dev_index[slot][netdevNum].netdev);
		cvm_rpa_dev_index[slot][netdevNum].netdev = NULL; 
		cvm_rpa_dev_index[slot][netdevNum].flag = -1;
		cvm_rpa_dev_index[slot][netdevNum].ifindex = -1;
	}
	else 
	{
		printk(KERN_INFO "The device is not exist. slot(%d), netdevNum(%d).\n", slot, netdevNum);
		return -1;
	}

	/* if the dev is set as arp-reply send device , clean it and check other device on the slot to replace the old one */
	if (rpa_arp_send_dev[slot].netdev)
	{
		if (rpa_arp_send_dev[slot].netdevNum == netdevNum)
		{
			rpa_arp_send_dev[slot].netdev = NULL;
			rpa_arp_send_dev[slot].netdevNum = -1;
			for (tempport = 0; tempport < RPA_NETDEV_NUM; tempport++)
			{
				if (cvm_rpa_dev_index[slot][tempport].netdev)
				{
					rpa_arp_send_dev[slot].netdev = cvm_rpa_dev_index[slot][tempport].netdev;
					rpa_arp_send_dev[slot].netdevNum = tempport;
					break;
				}
			}

			if (tempport == RPA_NETDEV_NUM) {
				rpa_broadcast_mask &= (~(1 << slot));
				rpa_default_broadcast_mask &= (~(1 << slot));
			}
		}
	}
	cvm_rpa_dev_index_print(0);
	
	return 0;
}

/*
* change slotNum or netdevNum of rpa interface
* caojia, 20110907
*/
int rpa_if_modify(struct rpa_interface_info_s *rpa_if_info)
{
	struct net_device *dev = NULL;
	rpa_dev_private_t *priv = NULL;
	int full_count = 0;
	int netdevNum, slot;
	int tempNum;

	if (NULL == rpa_if_info)
	{
		return -1;
	}

	if (rpa_if_info->slotNum >= RPA_SLOT_NUM || rpa_if_info->netdevNum >= RPA_NETDEV_NUM)
	{
		rpa_info("Illegal slotNum(%d) or netdevNum(%d).\n", rpa_if_info->slotNum, rpa_if_info->netdevNum);
		return -1;
	}

	slot = rpa_if_info->slotNum;
	netdevNum = rpa_if_info->netdevNum;

	dev = dev_get_by_name(&init_net, rpa_if_info->if_name);
	if (!dev)
	{
		rpa_info("Can not get dev by if_name(%s).\n", rpa_if_info->if_name);
		return -1;
	}
	else
	{
		dev_refcnt_decrement(&(dev)->refcnt);
	}

	priv = (rpa_dev_private_t *)netdev_priv(dev);

	if ((slot == priv->dslotNum) && (netdevNum == priv->dnetdevNum))
	{
		rpa_info("slotNum(%d) and netdevNum(%d) not change.\n", slot, netdevNum);
		return -1;
	}

	/* if netdevNum is 0, need to allocate a available netdevNum */
	if (netdevNum == 0)
	{
		while (cvm_rpa_dev_index[slot][netdev_number[slot]].netdev)
		{
			if (netdev_number[slot] == RPA_NETDEV_NUM)
			{
				if (full_count > 0)
				{
					rpa_info("RPA Netdev Index Table is out of range!\n");
					return -1;
				}
				netdev_number[slot] = 1;
				full_count++;
				continue;
			}
			netdev_number[slot] += 1;
		}
		netdevNum = netdev_number[slot];
		netdev_number[slot] += 1;
	}

	if (cvm_rpa_dev_index[slot][netdevNum].netdev) {
		rpa_warning("The rpa-device has been already registered. slot(%d), port(%d).\n", slot, netdevNum);
		return -1;
	}

	/* delete old table index */
	cvm_rpa_dev_index[priv->dslotNum][priv->dnetdevNum].netdev = NULL;
	cvm_rpa_dev_index[priv->dslotNum][priv->dnetdevNum].ifindex = -1;
	cvm_rpa_dev_index[priv->dslotNum][priv->dnetdevNum].flag = -1;

	/* if it is a arp send device, change a another one */
	if ((priv->dslotNum != slot) || (rpa_arp_send_dev[priv->dslotNum].netdevNum == priv->dnetdevNum))
	{
		rpa_arp_send_dev[priv->dslotNum].netdev = NULL;
		rpa_arp_send_dev[priv->dslotNum].netdevNum = -1;
		for (tempNum = 0; tempNum < RPA_NETDEV_NUM; tempNum++)
		{
			if (cvm_rpa_dev_index[priv->dslotNum][tempNum].netdev)
			{
				rpa_arp_send_dev[priv->dslotNum].netdev = cvm_rpa_dev_index[priv->dslotNum][tempNum].netdev;
				rpa_arp_send_dev[priv->dslotNum].netdevNum = tempNum;
				rpa_info("rpa_arp_send_dev[%d].netdev now is : %s\n", priv->dslotNum, rpa_arp_send_dev[priv->dslotNum].netdev->name);
				break;
			}
		}
	}

	/* add new table index */
	cvm_rpa_dev_index[slot][netdevNum].netdev = dev;
	cvm_rpa_dev_index[slot][netdevNum].ifindex = dev->ifindex;
	cvm_rpa_dev_index[slot][netdevNum].flag = 1;

	priv->dslotNum = slot;
	priv->dnetdevNum = netdevNum;

	if (!rpa_arp_send_dev[slot].netdev && dev)
	{
		rpa_info("rpa_arp_send_dev now\n");
		rpa_arp_send_dev[slot].netdev = dev;
		rpa_arp_send_dev[slot].netdevNum = netdevNum;
	}

	return netdevNum;
}
