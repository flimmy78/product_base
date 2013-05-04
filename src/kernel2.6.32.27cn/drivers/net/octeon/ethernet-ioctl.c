/*
 *
 *	filename:ethernet-ioctl.c
 *	copyright:Autelan.Co.Ltd
 *	auther:sush@autelan.com
 *	13/May/2008
 *	
 * */


#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kmod.h>

#include <linux/rtnetlink.h>
#include <linux/netdevice.h>

#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/time.h>
/*#include "ethernet-defines.h"*/
#include "ethernet-ioctl.h"
/*#include "ethernet-mdio.h"*/

#include <asm/octeon/cvmx-smix-defs.h>
#include <asm/octeon/cvmx-mdio.h>
#include <linux/smp_lock.h>
#include "ethernet-common.h"

/*#include "ethernet-table-display.h"*/

int oct_major_num = 242;
module_param(oct_major_num, int, 0444);
int oct_minor_num = 0;
module_param(oct_minor_num,int,0444);
int oct_ve_debug = 0;/*luoxun ve debug*/
module_param(oct_ve_debug,int,0644);
int oct_rpa_debug = 0;/*caojia rpa debug*/
module_param(oct_rpa_debug,int,0644);
//int oct_rx_debug = 1;
//module_param(oct_rx_debug,int,0644);

/* for debug linyi error, 2012-10-01 */
int oct_length_debug = 1;   
module_param(oct_length_debug,int,0644);

#define DRIVER_NAME "oct"
#if 0
extern struct net_device *cvm_oct_device[];
extern int dynamic_registe_dev(int portNum, int devNum, char *name);
extern int dynamic_unregiste_dev(int portNum, int devNum, char *name);
extern int dynamic_adv_routing_enable_dev(int portNum, int devNum, char *name);
extern int dynamic_adv_routing_disable_dev(int portNum, int devNum, char *name);

extern int dynamic_registe_if(ve_vlan_param_data_s* vlan_data);
extern int dynamic_unregiste_if(ve_vlan_param_data_s* vlan_data);
extern int dynamic_adv_routing_enable_if(ve_vlan_param_data_s* vlan_data);
extern int dynamic_adv_routing_disable_if(ve_vlan_param_data_s* vlan_data);
extern int dynamic_update_if(ve_vlan_param_data_s* vlan_data);
extern int cvm_get_local_mac_addr(unsigned char *mac_addr);

#endif
extern unsigned int up_times[];
extern unsigned int down_times[];
extern struct timespec timestamp[];
//extern int rxmax_in_an_interrupt;
extern int rx_napi_weight;
extern int cvm_phy_enable(void);
extern int cvm_phy_disable(void);
extern int cvm_read_phy_QT(int regaddr,unsigned int  location);
extern int cvm_write_phy_QT(int regaddr,int location,int val);
extern int reset_if_statistics(if_sta_statistics_str *tmp_ptr);

struct oct_dev_s {
	long quantum;
	long qset;
	unsigned long size;
	unsigned long access_key;
	struct cdev cdev;
};
//static ve_netdev_priv ve_priv; 
cvm_port_updown_times port_updown_times;
cvm_link_timestamp linkstamps;
/*added by huxuefeng 20100205 start*/
phy_read_write_t phy_read_val;
phy_read_write_t phy_write_val;
/*added by huxuefeng 20100205 end*/
static struct oct_dev_s oct_dev;

port_counter_t cvm_stats[TOTAL_NUMBER_OF_PORTS]; /* added by Jia Lihui 2011.7.28 */
port_counter_t eth_port_stats;

#if 0
int ve_vlan_status_change_up(ve_vlan_param_data_s* vlan_data)
{
	int index ;
	int ret = 0;
	index = vlan_data->vid + DEV_VLAN_START_NUM;
	if(cvm_oct_device[index])
	{
	    netif_carrier_on(cvm_oct_device[index]);
		rtnl_lock();
		if(dev_change_flags(cvm_oct_device[index],(cvm_oct_device[index]->flags |IFF_UP)) < 0)
			{
				printk("%s: set flags error!\n",cvm_oct_device[index]->name);
				
				ret = -1;
			}
		else
			{
				printk("Interface %s UP.\n",cvm_oct_device[index]->name);
			}
		rtnl_unlock();
	}
	return ret;


}

int ve_vlan_status_change_down(ve_vlan_param_data_s* vlan_data)
{
	int index ;
	int ret = 0;
	index = vlan_data->vid + DEV_VLAN_START_NUM;
	if(cvm_oct_device[index])
	{
		if (netif_carrier_ok(cvm_oct_device[index]))
			netif_carrier_off(cvm_oct_device[index]);
		rtnl_lock();
		if(dev_change_flags(cvm_oct_device[index],(cvm_oct_device[index]->flags & (~IFF_UP))) < 0)
			{
				printk("%s: set flags error!\n",cvm_oct_device[index]->name);
				
				ret = -1;
			}
		else
			{
				printk("Interface %s DOWN.\n",cvm_oct_device[index]->name);
			}
		rtnl_unlock();

	}

	return ret;

}

int ve_dev_status_change_up(ve_netdev_priv* ve_dev_data)
{
	int index ,portNum,devNum;
	int ret = 0;

	portNum = ve_dev_data->portNum;
	devNum = ve_dev_data->devNum;
	index = INDEX_VE(portNum,devNum);
	if(cvm_oct_device[index])
	{
	    netif_carrier_on(cvm_oct_device[index]);
		rtnl_lock();
		if(dev_change_flags(cvm_oct_device[index],(cvm_oct_device[index]->flags |IFF_UP)) < 0)
			{
				printk("%s: set flags error!\n",cvm_oct_device[index]->name);
				ret = -1;
			}
		else
			{
				printk("Interface %s UP.\n",cvm_oct_device[index]->name);
			}
		rtnl_unlock();
	}
	return ret;


}

int ve_dev_status_change_down(ve_netdev_priv* ve_dev_data)
{
	int index,portNum,devNum;
	int ret = 0;
	portNum = ve_dev_data->portNum;
	devNum = ve_dev_data->devNum;
	index = INDEX_VE(portNum,devNum);
	if(cvm_oct_device[index])
	{
		if (netif_carrier_ok(cvm_oct_device[index]))
			netif_carrier_off(cvm_oct_device[index]);
		rtnl_lock();
		if(dev_change_flags(cvm_oct_device[index],(cvm_oct_device[index]->flags & (~IFF_UP))) < 0)
			{
				printk("%s: set flags error!\n",cvm_oct_device[index]->name);
				
				ret = -1;
			}
		else
			{
				printk("Interface %s DOWN.\n",cvm_oct_device[index]->name);
			}
		rtnl_unlock();

	}

	return ret;

}
#endif
/**
  * Variable for param data passing to ioctl.
  */
//static ve_vlan_param_data_s  ve_vlan_data;


/**
  * Macros for private use.
  * add by baolc
  */
#define  ve_kmalloc(x)  kmalloc(x, GFP_ATOMIC)
//#define  DEBUG_CN_ETH  1


/**
  * VLAN table (or Interface info table) for ethernet driver use.
  * add by baolc
  * 2008-06-11
  */
//static struct ve_vlan_s**  ve_vlan_table_priv = NULL; /* vlan table */
struct ve_vlan_s*  ve_vlan_table_priv[VE_VLAN_TABLE_MAX_NUM];
//static spinlock_t   ve_vlan_table_lock;        /* lock for entire vlan table */ /* no use, by baolc */



/**
  * Deprecated!! No use!!
  * Interface index bitmap.
  * add by baolc
  */
//static int ve_intf_idle_idx_bitmap = 0x00000000;
//static spinlock_t   ve_intf_idle_idx_bitmap_lock = SPIN_LOCK_UNLOCKED;        /* lock for index bitmap */



/**
  * Deprecated!!No use!!
  * Get idle index.
  * add by baolc
  *
int ve_get_intf_idle_index(void)
{
	int i;
	spin_lock_irq(&ve_intf_idle_idx_bitmap_lock);
	for (i=0; i<32; i++)
	{
		if ((ve_intf_idle_idx_bitmap & ((unsigned int)0x1 << i))  ==  0)
		{
			break; //find an idle index
		}
	}
	ve_intf_idle_idx_bitmap |= ((unsigned int)0x1 << i);
	spin_unlock_irq(&ve_intf_idle_idx_bitmap_lock);
	return 40+i;
}
*/


/**
 *This function may break "npd" down.
 *delete by luoxun  2008-06-19
 */
 /*
void ve_display_intf_info_table_debug(void)
{
	int i;
	printk("AX_VE: interface info table: \n");
	for (i=0; i<VE_VLAN_TABLE_MAX_NUM; i++) {
		spin_lock_irq(&ve_vlan_table_priv[i]->lock);
		if (ve_vlan_table_priv[i])
		{
			
			printk("    item[%d] addr: %p\n", i, ve_vlan_table_priv[i]);
			printk("        content: vid: %d, intfName: %s, dev0 untagbmp: %x, dev0 tagbmp: %x, dev1 untagbmp: %x, dev1 tagbmp: %x, intfIdx: %d\n", 
						ve_vlan_table_priv[i]->vid, 
						ve_vlan_table_priv[i]->intfName, 
						ve_vlan_table_priv[i]->untagPortBitmap[0], 
						ve_vlan_table_priv[i]->tagPortBitmap[0], 
						ve_vlan_table_priv[i]->untagPortBitmap[1], 
						ve_vlan_table_priv[i]->tagPortBitmap[1], 
						ve_vlan_table_priv[i]->ifIndex);
			spin_unlock_irq(&ve_vlan_table_priv[i]->lock);
		}
	}
}*/


/**
  * Init vlan/interface info table.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_init_intf_info_table(void) {
/*
    if (ve_vlan_table_priv == NULL) {
		spin_lock_init(&ve_vlan_table_lock);
	    spin_lock_irq(&ve_vlan_table_lock);
		ve_vlan_table_priv = (ve_vlan_s**)ve_kmalloc((sizeof(struct ve_vlan_s*) * (VE_VLAN_TABLE_MAX_NUM)));
		

		if (ve_vlan_table_priv == NULL) {
			printk("ve_init_vlan_table: memory alloc error for vlan table init!!");
			spin_unlock_irq(&ve_vlan_table_lock);
			return VE_FAILED;
		}
		printk("ve_init_intf_info_table debug info: ve_vlan_table_priv addr %p\n", ve_vlan_table_priv);
		memset(ve_vlan_table_priv, 0, VE_VLAN_TABLE_MAX_NUM * sizeof(struct ve_vlan_s*));
		spin_unlock_irq(&ve_vlan_table_lock);
    }
*/
	memset(ve_vlan_table_priv, 0, sizeof(ve_vlan_table_priv));

	return VE_SUCCESS;
}



/**
  * Destroy vlan/interface info table.
  * add by baolc
  * 2008-06-12
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
 #if 0 
int ve_destroy_intf_info_table(void)
{
	if (ve_vlan_table_priv) {
		int i;
	//	spin_lock_irq(&ve_vlan_table_lock);
		for (i=0; i<VE_VLAN_TABLE_MAX_NUM; i++) {
			if (ve_vlan_table_priv[i])
			{
//				spin_lock_irq(&(ve_vlan_table_priv[i]->lock));
//				spin_lock(&(ve_vlan_table_priv[i]->lock));
				kfree(ve_vlan_table_priv[i]);
				ve_vlan_table_priv[i] = NULL;
			}
		}
			
	//	spin_unlock_irq(&ve_vlan_table_lock);
	}
	return VE_SUCCESS;
}

#endif

/**
  * Create vlan/interface info(add data into vlan/interface info table).
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_add_intf_info_by_vid(unsigned short vlanId, struct ve_vlan_param_data_s* vlan_data, unsigned int intfIdx)
{
	struct ve_vlan_s* node = NULL;
	node = (struct ve_vlan_s *)ve_kmalloc(sizeof(struct ve_vlan_s));
	if (node == NULL) {
		printk("ve_create_vlan_by_vid error: null memory allocated!\n");
		return VE_FAILED;
	}
	memset(node, 0, sizeof(struct ve_vlan_s));

	//populate the node struct
	node->vid = vlanId;
	node->ifIndex = intfIdx; //interface index
	memcpy(node->tagPortBitmap, vlan_data->tagPortBitmap, sizeof(node->tagPortBitmap[0]) * VE_MARVELL_DEV_COUNT);
	memcpy(node->untagPortBitmap, vlan_data->untagPortBitmap, sizeof(node->untagPortBitmap[0]) * VE_MARVELL_DEV_COUNT);
	memcpy(node->intfName, vlan_data->intfName, VE_VLAN_IFNAME_SIZE);
	spin_lock_init(&node->lock); //init node spin lock

	if ((vlanId == 0) || (vlanId > VE_VLAN_TABLE_MAX_NUM))
	{
		printk("cavium-ethernet: (create interface): vid range over %d\n", VE_VLAN_TABLE_MAX_NUM);
		kfree(node);
		node = NULL;
		return VE_FAILED;
	}

	ve_vlan_table_priv[vlanId - 1] = node;

#ifdef DEBUG_CN_ETH
	//ve_display_intf_info_table_debug(); //debug
#endif
	return VE_SUCCESS;
}



/**
  * Find vlan/interface by id in vlan table.
  * Attention: find a node and lock it!! remember unlock it when no used!!
  * add by baolc
  * 2008-06-11
  */
struct ve_vlan_s*  ve_find_intf_by_vid(unsigned short vlanId)
{
	struct ve_vlan_s *vlanNode = NULL;
	if (ve_vlan_table_priv == NULL)
	{
		return NULL;
	}
	//vlanId should be in range of (0,4096]
	if ((vlanId <= 0)||(vlanId > VE_VLAN_TABLE_MAX_NUM))
		vlanNode = NULL;
	else {
		if (ve_vlan_table_priv[vlanId-1] == NULL)
			return NULL;
	    spin_lock(&ve_vlan_table_priv[vlanId-1]->lock);//sushaohua for gengdan
	    vlanNode = ve_vlan_table_priv[vlanId-1];
	    //spin_unlock_irq(&ve_vlan_table_priv[vlanId-1]->lock); //do not unlock
	}

	return vlanNode;
}



/**
  * Delete vlan/interface info from vlan/interface info table.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_delete_intf_info_by_vid(unsigned short vlanId)
{
	struct ve_vlan_s* node = NULL;

	node = ve_find_intf_by_vid(vlanId); //add a lock

	if (NULL == node) {
		return VE_FAILED;
	}
	else {
		ve_vlan_table_priv[vlanId - 1] = NULL;
		spin_unlock(&node->lock);
		kfree(node);
		node = NULL;
	}

#ifdef DEBUG_CN_ETH
	//ve_display_intf_info_table_debug(); //debug
#endif
	return VE_SUCCESS;
}



/**
  * Update vlan's port map.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_update_intf_port_map(unsigned short vlanId, struct ve_vlan_param_data_s* vlan_data)
{
	struct ve_vlan_s* node = NULL;
	node = ve_find_intf_by_vid(vlanId);//add a lock
	if (NULL == node) {
		return VE_FAILED;
	}
	memcpy(node->tagPortBitmap, vlan_data->tagPortBitmap, sizeof(node->tagPortBitmap[0]) * VE_MARVELL_DEV_COUNT);
	memcpy(node->untagPortBitmap, vlan_data->untagPortBitmap, sizeof(node->untagPortBitmap[0]) * VE_MARVELL_DEV_COUNT);
	spin_unlock(&node->lock); //sushaohua add for gengdan
#ifdef DEBUG_CN_ETH	
	//ve_display_intf_info_table_debug(); //debug
#endif
	return VE_SUCCESS;
}



/**
  * Function for query tag or untag  by portnum and vid.
  * param(out):  intfIdx    interface's index, use 0 if no need.
  * return:  VE_IS_TAG_PORT or VE_IS_UNTAG_PORT or VE_ERR_PORT
  */
int ve_query_port_istag(unsigned short vlanId, int devNum, int portNum, int* intfIdx)
{
	struct ve_vlan_s*  node = NULL;

	if (devNum < 0 || devNum >= VE_MARVELL_DEV_COUNT || portNum < 0 || portNum > VE_MAX_PORT_NUM) {
		return VE_ERR_PORT;
	}
	
	node = ve_find_intf_by_vid(vlanId);//add a lock
	if (NULL == node) {
		return VE_ERR_PORT;
	}
	if (((node->untagPortBitmap[devNum]) & ((unsigned int)0x1 << portNum)) != 0)
	{
		if (intfIdx)
			*intfIdx = node->ifIndex;
		spin_unlock(&node->lock);//sushaohua add for gengdan
		return VE_IS_UNTAG_PORT;
	}
	if (((node->tagPortBitmap[devNum]) & ((unsigned int)0x1 << portNum)) != 0)
	{
		if (intfIdx)
			*intfIdx = node->ifIndex;
		spin_unlock(&node->lock);//sushaohua add for gengdan
		return VE_IS_TAG_PORT;
	}
	spin_unlock(&node->lock);//sushaohua add for gengdan
	return VE_ERR_PORT; //no find result
}



/**
  * Function for query interface index  by portnum and vid.
  * return:  interface index or VE_ERR_PORT
  */
int ve_query_port_intf_index(unsigned short vlanId, int devNum, int portNum)
{
	struct ve_vlan_s*  node = NULL;
	int tmpIdx = 0;
	if (devNum < 0 || devNum >= VE_MARVELL_DEV_COUNT || portNum < 0 || portNum > VE_MAX_PORT_NUM) {
		return VE_ERR_PORT;
	}
	node = ve_find_intf_by_vid(vlanId);//add a lock
	if (NULL == node) {
		return VE_ERR_PORT;
	}
	tmpIdx = node->ifIndex;
	spin_unlock(&node->lock);//sushaohua add for gengdan
	return tmpIdx;
}


void ve_display_all_port(FDB_BROADCAST_ARRAY *b_array, unsigned short vid)
{
	int i;
//	printk("Display the broadcast array:\n");
	printk("=======VID=%d=======BROADCAST=START====================\n",vid);
	printk("\ttag/untag\t\tportNum\t\tdevNum\n");
		for(i=0;i<b_array->tagcount;i++)
			{
				printk("\ttag\t\t\t%d\t\t%d\n",b_array->tag[i].portNum,b_array->tag[i].devNum);
			}
		
		for(i=0;i<b_array->untagcount;i++)
			{
				printk("\tuntag\t\t\t%d\t\t%d\n",b_array->untag[i].portNum,b_array->untag[i].devNum);
			}
	printk("=======VID=%d=======BROADCAST=END======================\n",vid);

}

int ve_query_all_port(FDB_BROADCAST_ARRAY *b_array, unsigned short vlanId)
{
	struct ve_vlan_s * node = NULL;
	int devNum,portNum;
	memset(b_array,0,sizeof(FDB_BROADCAST_ARRAY));
	
	if(oct_ve_debug) 
		printk("vlanID:0x%x\n",vlanId);
	node = ve_find_intf_by_vid(vlanId);
	if(NULL == node)
		{
			return -1;
		}
	for (devNum = 0;devNum < VE_MARVELL_DEV_COUNT; devNum++)
		{
			unsigned int tagmap = node->tagPortBitmap[devNum];
			unsigned int untagmap = node->untagPortBitmap[devNum];
//			printk("tagmap:0x%x\n",tagmap);
//			printk("untagmap:0x%x\n",untagmap);
			for(portNum = 0;portNum < 32; portNum ++)
				{
					if(tagmap & ((unsigned int)0x1 << portNum))
						{
							b_array->tag[b_array->tagcount].devNum = devNum;
							b_array->tag[b_array->tagcount].portNum = portNum;
							b_array->tagcount +=1;
						}

				}
			for(portNum = 0; portNum < 32; portNum++)
				{
					if(untagmap & ((unsigned int)0x1 << portNum))
						{
							b_array->untag[b_array->untagcount].devNum = devNum;
							b_array->untag[b_array->untagcount].portNum = portNum;
							b_array->untagcount +=1;
											
						}
				}
		}
	spin_unlock(&node->lock);//sushaohua add for gengdan
#ifdef DEBUG_CN_ETH

	if(oct_ve_debug) 
		{
			ve_display_all_port(b_array,vlanId);
		}


#endif
	return 1;

}

unsigned int cvm_get_port_link_timestamps(int pip_port,unsigned long *time_stamp_sec)
{

	*time_stamp_sec = timestamp[pip_port].tv_sec;
	return 0;
}

#if 1
unsigned int cvm_get_port_updown_times(int pip_port, int * uptimes, int *downtimes)
{

	if((pip_port < 0)||(pip_port > 32))
		{
			printk("Get port updown times error! PIP port = %d.\n",pip_port);
			return 0;
		}
//	printk("Get port updown times PIP port = %d.\n",pip_port);
	*uptimes = up_times[pip_port];
	*downtimes = down_times[pip_port];
	return 0;

}
#endif
/*added by huxuefeng 20100205start*/
int cvm_phy_read(phy_read_write_t *value)
{
	value->read_reg_val = cvmx_mdio_read(0, value->phy_addr, value->location);
	return 0;
}

int cvm_phy_write(phy_read_write_t *value)
{
	if(cvmx_mdio_write(0, value->phy_addr, value->location, value->write_reg_val) < 0)
		return -1;
	return 0;
}
/*added by huxuefeng 20100205end*/
 static int oct_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
 	{
	int retval = 0;
	int op_ret = 0;
	int result_en;
/*	ve_fdb_param_data_s  fdb_param; //add by baolc, for manage fdb table
	*/
	int result_dis;
	macAddr_data_t macAddr_data;
	readphy_QT RD_QT;
	writephy_QT WR_QT;
	struct net_device *dev = NULL;
	if_sta_statistics_str if_sta_change;

	if (_IOC_TYPE(cmd) != CVM_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > CVM_IOC_MAXNR) return -ENOTTY;
#if 0
	memset(&ve_priv,0,sizeof(ve_netdev_priv));
	memset(&ve_vlan_data, 0, sizeof(ve_vlan_param_data_s)); //add by baolc
	memset(&eth_port_stats, 0, sizeof(port_counter_t));
#endif	
	memset(&macAddr_data, 0, sizeof(macAddr_data_t));
	memset(&RD_QT, 0, sizeof(readphy_QT));
	memset(&WR_QT, 0, sizeof(writephy_QT));
	switch (cmd) {
#if 0

		case BM_IOC_G_:
			op_ret = copy_from_user(&bm_ioc_op_args,(bm_op_args *)arg,sizeof(bm_op_args));
			
			/*send message to SE core.*/

			msg_send(bm_ioc_op_args.msg.type,bm_ioc_op_args.core,bm_ioc_op_args.msg.pad,bm_ioc_op_args.msg.length);
			break;

		case BM_IOC_X_:
			op_ret = copy_from_user(&bm_ioc_op_args,(bm_op_args *)arg,sizeof(bm_op_args));
			
			DBG(debug_ioctl,"Write addr[0x%016llx] [%d]bits with [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			do_write_wide((void *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			mdelay(10);	
			do_read_wide((uint64_t *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			DBG(debug_ioctl,"R &W  addr[0x%016llx] [%d]bits got  [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			
			op_ret = copy_to_user((bm_op_args *)arg,&bm_ioc_op_args,sizeof(bm_op_args));
			break;	
#endif
#if 0
	case CVM_IOC_REG_:
		op_ret = copy_from_user(&ve_priv,(ve_netdev_priv *)arg,sizeof(ve_netdev_priv));

		retval = dynamic_registe_dev(ve_priv.portNum,ve_priv.devNum,ve_priv.name);
		break;
	case CVM_IOC_ADV_EN_:
		op_ret = copy_from_user(&ve_priv,(ve_netdev_priv *)arg,sizeof(ve_netdev_priv));
		retval = dynamic_adv_routing_enable_dev(ve_priv.portNum,ve_priv.devNum,ve_priv.name);
		break;
	case CVM_IOC_UNREG_:
		op_ret = copy_from_user(&ve_priv,(ve_netdev_priv *)arg,sizeof(ve_netdev_priv));
		retval = dynamic_unregiste_dev(ve_priv.portNum,ve_priv.devNum,ve_priv.name);
		break;
	case CVM_IOC_ADV_DIS_:
		op_ret = copy_from_user(&ve_priv,(ve_netdev_priv *)arg,sizeof(ve_netdev_priv));
		retval = dynamic_adv_routing_disable_dev(ve_priv.portNum,ve_priv.devNum,ve_priv.name);
		break;
	case CVM_IOC_REG_INTF_: //add by baolc, register interface
		op_ret = copy_from_user(&ve_vlan_data, (ve_vlan_param_data_s *)arg, sizeof(ve_vlan_param_data_s));
		retval = dynamic_registe_if(&ve_vlan_data);
		break;
	case CVM_IOC_UNREG_INTF_: //add by baolc, unregister interface
		op_ret = copy_from_user(&ve_vlan_data, (ve_vlan_param_data_s *)arg, sizeof(ve_vlan_param_data_s));
		retval = dynamic_unregiste_if(&ve_vlan_data);
		break;
	case CVM_IOC_ADV_EN_INTF_:
		op_ret = copy_from_user(&ve_vlan_data,(ve_vlan_param_data_s *)arg,sizeof(ve_vlan_param_data_s));
		retval = dynamic_adv_routing_enable_if(&ve_vlan_data);
		break;
	case CVM_IOC_ADV_DIS_INTF_:
		op_ret = copy_from_user(&ve_vlan_data,(ve_vlan_param_data_s *)arg,sizeof(ve_vlan_param_data_s));
		retval = dynamic_adv_routing_disable_if(&ve_vlan_data);
		break;
	case CVM_IOC_UPDATE_INTF_: //add by baolc, update interface info
		op_ret = copy_from_user(&ve_vlan_data, (ve_vlan_param_data_s *)arg, sizeof(ve_vlan_param_data_s));
		retval = dynamic_update_if(&ve_vlan_data);
		break;

	case CVM_IOC_DISPLAY_INTF_TABLE: //add by baolc
		ve_show_intf_info_table();
		break;
		
	case CVM_IOC_DISPLAY_FDB_TABLE: //add by baolc
		ve_show_fdb_table();
		break;
		
	case CVM_IOC_FDB_ADD_ITEM: //add by baolc
		copy_from_user(&fdb_param, (ve_fdb_param_data_s*)arg, sizeof(ve_fdb_param_data_s));
		retval = ve_add_item_to_fdb_table(fdb_param.vid, fdb_param.devNum, fdb_param.portNum, fdb_param.DMAC);
		break;
	case CVM_IOC_FDB_DEL_ITEM: //add by baolc
		copy_from_user(&fdb_param, (ve_fdb_param_data_s*)arg, sizeof(ve_fdb_param_data_s));
		retval = ve_delete_item_from_fdb_table(fdb_param.vid, fdb_param.devNum, fdb_param.portNum, fdb_param.DMAC);
		break;
	case CVM_IOC_INTF_UP:
		op_ret = copy_from_user(&ve_vlan_data, (ve_vlan_param_data_s *)arg, sizeof(ve_vlan_param_data_s));
		retval = ve_vlan_status_change_up(&ve_vlan_data);
		break;
		
	case CVM_IOC_INTF_DOWN:
		op_ret = copy_from_user(&ve_vlan_data, (ve_vlan_param_data_s *)arg, sizeof(ve_vlan_param_data_s));
		retval = ve_vlan_status_change_down(&ve_vlan_data);
		break;
	case CVM_IOC_DEV_UP:
		op_ret = copy_from_user(&ve_priv, (ve_netdev_priv*)arg, sizeof(ve_netdev_priv));
		retval = ve_dev_status_change_up(&ve_priv);
		break;

	case CVM_IOC_DEV_DOWN:
		op_ret = copy_from_user(&ve_priv, (ve_netdev_priv *)arg, sizeof(ve_netdev_priv));
		retval = ve_dev_status_change_down(&ve_priv);
		break;
#endif		
	case CVM_IOC_GET_UPDOWN_TIMES:
		op_ret = copy_from_user(&port_updown_times, (cvm_port_updown_times *)arg, sizeof(cvm_port_updown_times));
		retval = cvm_get_port_updown_times(port_updown_times.pip_port, &(port_updown_times.up_times), &(port_updown_times.down_times));
		copy_to_user((cvm_port_updown_times *)arg, &port_updown_times, sizeof(cvm_port_updown_times));
		break;
		
	/*added by huxuefeng20100205start*/
	case CVM_IOC_PHY_READ:
		op_ret = copy_from_user(&phy_read_val, (phy_read_write_t *)arg, sizeof(phy_read_write_t));
		retval = cvm_phy_read(&phy_read_val);
		copy_to_user((phy_read_write_t *)arg, &phy_read_val, sizeof(phy_read_write_t));
		break;
	case CVM_IOC_PHY_WRITE:
		op_ret = copy_from_user(&phy_write_val, (phy_read_write_t *)arg, sizeof(phy_read_write_t));
		retval = cvm_phy_write(&phy_write_val);
		copy_to_user((phy_read_write_t *)arg, &phy_write_val, sizeof(phy_read_write_t));
		break;
		
	/*added by huxuefeng20100205end*/
	
	case CVM_IOC_ETH_LINK_TIMESTAMP:
		op_ret = copy_from_user(&linkstamps, (cvm_link_timestamp *)arg, sizeof(cvm_link_timestamp));
		retval = cvm_get_port_link_timestamps(linkstamps.pip_port, (unsigned long *)(&(linkstamps.timestamp)));
		copy_to_user((cvm_link_timestamp *)arg, &linkstamps, sizeof(cvm_link_timestamp));
		break;
		
	case CVM_IOC_GET_ETH_STAT:		
		op_ret = copy_from_user(&eth_port_stats, (port_counter_t *)arg, sizeof(port_counter_t));		
		dev = dev_get_by_name(&init_net,eth_port_stats.ifName);
		if (eth_port_stats.clear == 0)
		{
		    memcpy(&eth_port_stats, &cvm_stats[dev->ifindex], sizeof(port_counter_t));
		}
		else
			memset(&cvm_stats[dev->ifindex], 0, sizeof(port_counter_t));
		copy_to_user((port_counter_t *)arg, &eth_port_stats, sizeof(port_counter_t));
		break;

	case CVM_IOC_GET_ETHER_HWADDR:
		op_ret = copy_from_user(&macAddr_data, (macAddr_data_t *)arg, sizeof(macAddr_data_t));
		retval = cvm_get_slot_mac_addr(macAddr_data.hwaddr, macAddr_data.slot_id);
		copy_to_user((macAddr_data_t *)arg, &macAddr_data, sizeof(macAddr_data_t));
		break;	
	
    case CVM_IOC_READ_INTERRUPT_RXMAX:
		copy_to_user((int *)arg, &rx_napi_weight, sizeof(int));
		break;
	
    case CVM_IOC_SET_INTERRUPT_RXMAX:
        op_ret = copy_from_user(&rx_napi_weight, (int *)arg, sizeof(int));
        printk("rxmax_in_an_interrupt is set to %d\n", rx_napi_weight);
		break;
	case CVM_IOC_PHY_MIIX_ENABLE:
		result_en = cvm_phy_enable();
		copy_to_user((int *)arg, &result_en, sizeof(int));
		break;
	case CVM_IOC_PHY_MIIX_DISABLE:
		result_dis = cvm_phy_disable();
		copy_to_user((int *)arg, &result_dis, sizeof(int));
		break;
	case CVM_IOC_readphy_QT:
		op_ret = copy_from_user(&RD_QT, (readphy_QT *)arg, sizeof(readphy_QT));
		RD_QT.val = cvm_read_phy_QT(RD_QT.regaddr,RD_QT.location);
		copy_to_user((readphy_QT *)arg, &RD_QT, sizeof(readphy_QT));
		break;	
	case CVM_IOC_writephy_QT:
		op_ret = copy_from_user(&WR_QT, (writephy_QT *)arg, sizeof(writephy_QT));
		retval = cvm_write_phy_QT(WR_QT.regaddr,WR_QT.location,WR_QT.val);
		//copy_to_user((writephy_QT *)arg, &WR_QT, sizeof(writephy_QT));
		break;
	case CVM_IOC_RESET_IF_STA:
		op_ret = copy_from_user(&if_sta_change, (if_sta_statistics_str*)arg, sizeof(if_sta_statistics_str));
		retval = reset_if_statistics(&if_sta_change);
		break;
	default:
		retval = -1;
		break;
	}
	
	return retval;
}

 static int oct_open(struct inode *inode, struct file *filp)
 	{
	struct oct_dev_s *dev;

	dev = container_of(inode->i_cdev, struct oct_dev_s, cdev);
	filp->private_data = dev;
	dev->size = 0;
	return 0;
	}
 
 
 
 long oct_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg) 
 {
	 int rval;
 
	 lock_kernel();
	 rval = oct_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);
	 unlock_kernel();
 
	 return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;
 
 }
 static int oct_release(struct inode *inode, struct file *file)
 {
	 return 0;
 }
 
 static struct file_operations bm_fops = 
 {
	 .owner  = THIS_MODULE,
	 //.llseek	 = bm_llseek,
	 //.read	 = bm_read,
	 //.write	 = bm_write,
	 .compat_ioctl = oct_compat_ioctl,
	 .ioctl  = oct_ioctl,
	 .open	 = oct_open,
	 .release= oct_release
 
 };

int oct_init(void)
 	
 {
 //  static struct file_operations fops;
	 int result;
	 dev_t bm_devt;
 
 //  result = alloc_chrdev_region(&bm_devt, 232,1,DRIVER_NAME);
 //  in order to use the major number as majic number of ioctl, we use a fixed major num.
	 bm_devt = MKDEV(oct_major_num,oct_minor_num);
	 result = register_chrdev_region(bm_devt,1,DRIVER_NAME);
	 printk(KERN_INFO DRIVER_NAME ":register major dev [%d]\n",MAJOR(bm_devt));
 	 printk(KERN_INFO"#############:register major dev [%d]\n",MAJOR(bm_devt));
	 if (result < 0) {
		 printk(KERN_WARNING DRIVER_NAME ":register_chrdev_region err[%d]\n",result);
		 return 0;
	 } 
	 cdev_init(&(oct_dev.cdev),&bm_fops);
	 oct_dev.cdev.owner = THIS_MODULE;
		 result = cdev_add(&(oct_dev.cdev),bm_devt,1);
	 if (result < 0) {
		 printk(KERN_WARNING DRIVER_NAME ":cdev_add err[%d]\n",result);
	 } else {
		 printk(KERN_INFO DRIVER_NAME ":loaded successfully.\n");
	 }
	 return 0;
 }
int oct_cleanup(void)
	{
		printk(KERN_INFO DRIVER_NAME ":Unregister MajorNum[%d]\n",MAJOR(oct_dev.cdev.dev));	
		unregister_chrdev_region(oct_dev.cdev.dev,1);	
		cdev_del(&(oct_dev.cdev));
		printk(KERN_INFO DRIVER_NAME ":unloaded\n");
       // ve_destroy_intf_info_table(); add by baolc, destroy interface info table
		return 0;
	}
//	module_init(oct_init);
//	module_exit(oct_cleanup);

