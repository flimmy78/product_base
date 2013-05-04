/*
 *	filename: ethernet-ioctl.h
 *	Auther :sush@autelan.com
 *	13/May/2008
 *
 * */

#ifndef __ETHERNET_IOCTL_H__
#define __ETHERNET_IOCTL_H__
#include <linux/spinlock.h>
/*#include "wrapper-cvmx-includes.h"*/




/***********************wc add***************/
#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-app-init.h>
#include <asm/octeon/cvmx-fau.h>
#include <asm/octeon/cvmx-dfa-defs.h>
#include <asm/octeon/cvmx-pow.h>
/*#include <asm/octeon/cvmx-gmx.h>*/
#include <asm/octeon/cvmx-spi.h>
#include <asm/octeon/cvmx-bootmem.h>
#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>

#define TOTAL_NUMBER_OF_PORTS       (CVMX_PIP_NUM_INPUT_PORTS+1)


/*********************************************/

/**
  * Structure for encapsulating param to ioctl.
  */
typedef struct ve_dev_priv
{
	int portNum;
	int devNum;
	char name[15];
}ve_netdev_priv;



typedef struct cvm_updown_times
{
	int 		 pip_port;
	unsigned int up_times;
	unsigned int down_times;
	int 		 reseved_1;
	int 		 reseved_2;
}cvm_port_updown_times;



typedef struct cvm_link_timestamp
{
	int 			pip_port;
	unsigned long long	timestamp;
	int 			reseved_1;
	

}cvm_link_timestamp;

/*added by huxuefeng 20100205start*/
typedef struct
{
	int phy_addr;
	int location;
	int read_reg_val;
	int write_reg_val;
}phy_read_write_t;

/*added by huxuefeng 20100205end*/
 /**
  * Structure for encapsulating param passing to ioctl.
  * add by baolc
  */
#define VE_VLAN_IFNAME_SIZE      16
#define VE_MARVELL_DEV_COUNT     32
typedef struct ve_vlan_param_data_s
{
	unsigned short 				vid;		/* VLAN id */
	char						intfName[VE_VLAN_IFNAME_SIZE];	/* interface name */
	unsigned int                untagPortBitmap[VE_MARVELL_DEV_COUNT];  /* untagged bit map, 2 device */
	unsigned int                tagPortBitmap[VE_MARVELL_DEV_COUNT];    /* tagged bit map, 2 device */
}ve_vlan_param_data_s; /* vlan node */



/**
  * Structures for vlan-port table.Or described it as interface info table.
  * add by baolc
  */
typedef struct ve_vlan_s
{
	unsigned short 				vid;		/* VLAN id */
	char						intfName[VE_VLAN_IFNAME_SIZE];	/* interface name */
	unsigned int                untagPortBitmap[VE_MARVELL_DEV_COUNT];  /* untagged bit map, 2 device */
	unsigned int                tagPortBitmap[VE_MARVELL_DEV_COUNT];    /* tagged bit map, 2 device */
	
	unsigned int                ifIndex;    /* interface index */
	spinlock_t                  lock;       /* spinlock for this vlan node */
}ve_vlan_s; /* vlan/interface info node */




/**
  * Param structure for manage fdb tables.
  */
typedef struct ve_fdb_param_data_s
{
	unsigned short    vid;      /* vlan id */
	int               devNum;   /* device number */
	int               portNum;  /* port number */
	unsigned char     DMAC[6];   /* mac */
}ve_fdb_param_data_s;



/*
  * Structure of the FDB broadcast array.
  * luoxun 2008-06-13
  */
typedef struct{
		int devNum;
		int portNum;
}DevPort;

typedef struct FDB_broadcast_array{
	int tagcount;
	int untagcount;
	DevPort tag[32];
	DevPort untag[32];
}FDB_BROADCAST_ARRAY;


typedef struct port_counter_s
{
	char                   ifName[21];
	int                    clear;
    cvmx_pip_port_status_t pip_stats;
    cvmx_pko_port_status_t pko_stats;
}port_counter_t;

typedef struct macAddr_data_s
{
    unsigned int slot_id;
	unsigned char hwaddr[6];
}macAddr_data_t;
/**
  * read write phy_QT Command parameter
.
  */
typedef struct readphy_QT
{
	int regaddr;
	unsigned int location;
	int val;
}readphy_QT;

typedef struct writephy_QT
{
	int regaddr;
	unsigned int location;
	int val;
}writephy_QT;


typedef struct sta_reset {
	char dev_name[32];
	unsigned int param_count;
	struct name_value {
		char name[32];
		unsigned long long value; 
	}name_value_arry[20];
}if_sta_statistics_str;

/**
  * Command number for ioctl.
  */
  #if 1
#define CVM_IOC_MAGIC 242
#define CVM_IOC_MAXNR 32

#define CVM_IOC_REG_                _IOWR(CVM_IOC_MAGIC,1,ve_netdev_priv) // read values
#define CVM_IOC_UNREG_              _IOWR(CVM_IOC_MAGIC,2,ve_netdev_priv) // read values

//add by baolc
#define CVM_IOC_REG_INTF_           _IOWR(CVM_IOC_MAGIC, 3, ve_vlan_param_data_s) // read values
#define CVM_IOC_UNREG_INTF_         _IOWR(CVM_IOC_MAGIC, 4, ve_vlan_param_data_s) // read values
#define CVM_IOC_UPDATE_INTF_        _IOWR(CVM_IOC_MAGIC, 5, ve_vlan_param_data_s) // read values


//cmd for manage tables, add by baolc
#define CVM_IOC_DISPLAY_INTF_TABLE  _IOWR(CVM_IOC_MAGIC, 6, int) 
#define CVM_IOC_DISPLAY_FDB_TABLE   _IOWR(CVM_IOC_MAGIC, 7, int) 
#define CVM_IOC_FDB_ADD_ITEM        _IOWR(CVM_IOC_MAGIC, 8, ve_fdb_param_data_s) 
#define CVM_IOC_FDB_DEL_ITEM        _IOWR(CVM_IOC_MAGIC, 9, ve_fdb_param_data_s) 

//cmd for manage tables, add by sush
#define CVM_IOC_INTF_UP 		    _IOWR(CVM_IOC_MAGIC, 10, ve_vlan_param_data_s) 
#define CVM_IOC_INTF_DOWN 			_IOWR(CVM_IOC_MAGIC, 11, ve_vlan_param_data_s) 
#define CVM_IOC_DEV_UP 				_IOWR(CVM_IOC_MAGIC, 12, ve_netdev_priv) 
#define CVM_IOC_DEV_DOWN 			_IOWR(CVM_IOC_MAGIC, 13, ve_netdev_priv) 
#define CVM_IOC_GET_UPDOWN_TIMES 	_IOWR(CVM_IOC_MAGIC, 14, cvm_port_updown_times) 

/*added by huxuefeng20100205start*/
#define CVM_IOC_PHY_READ			_IOWR(CVM_IOC_MAGIC, 15, phy_read_write_t)
#define CVM_IOC_PHY_WRITE			_IOWR(CVM_IOC_MAGIC, 16, phy_read_write_t)
/*added by huxuefeng 20100205end*/

/*cmd for get link duration*/
#define CVM_IOC_ETH_LINK_TIMESTAMP 	_IOWR(CVM_IOC_MAGIC, 17, cvm_link_timestamp)

#define CVM_IOC_ADV_EN_             _IOWR(CVM_IOC_MAGIC, 18, ve_netdev_priv) 
#define CVM_IOC_ADV_DIS_            _IOWR(CVM_IOC_MAGIC, 19, ve_netdev_priv) 

#define CVM_IOC_ADV_EN_INTF_        _IOWR(CVM_IOC_MAGIC, 20, ve_vlan_param_data_s) 
#define CVM_IOC_ADV_DIS_INTF_       _IOWR(CVM_IOC_MAGIC, 21, ve_vlan_param_data_s) 

#define CVM_IOC_GET_ETH_STAT        _IOWR(CVM_IOC_MAGIC, 22, port_counter_t)
#define CVM_IOC_GET_ETHER_HWADDR    _IOWR(CVM_IOC_MAGIC, 23, macAddr_data_t)

#define CVM_IOC_SET_INTERRUPT_RXMAX    _IOWR(CVM_IOC_MAGIC, 24, int) 
#define CVM_IOC_READ_INTERRUPT_RXMAX   _IOWR(CVM_IOC_MAGIC, 25, int) 
#define CVM_IOC_PHY_MIIX_ENABLE   _IOWR(CVM_IOC_MAGIC, 26, int) 
#define CVM_IOC_PHY_MIIX_DISABLE   _IOWR(CVM_IOC_MAGIC, 27, int)
#define CVM_IOC_readphy_QT   _IOWR(CVM_IOC_MAGIC, 28, int) 
#define CVM_IOC_writephy_QT   _IOWR(CVM_IOC_MAGIC, 29, int)
#define CVM_IOC_RESET_IF_STA	_IOWR(CVM_IOC_MAGIC, 30, if_sta_statistics_str)
/*#define VE_VLAN_IFNAME_SIZE      32*/
#define VE_VLAN_TABLE_MAX_NUM    (4*1024)
#define VE_MAX_PORT_NUM          27



#define VE_TRUE   1
#define VE_FALSE  0

#define VE_SUCCESS   0
#define VE_FAILED    (-1)

#define VE_IS_TAG_PORT   1
#define VE_IS_UNTAG_PORT 0
#define VE_ERR_PORT    (-1)

#endif
/**
  * Function for query tag or untag  by portnum and vid.
  * param(out):  intfIdx    interface's index, use 0 if no need.
  * return:  VE_IS_TAG_PORT or VE_IS_UNTAG_PORT or VE_ERR_PORT
  */
int ve_query_port_istag(unsigned short vid, int devNum, int portNum, int* intfIdx);


/**
  * Function for query interface index  by portnum and vid.
  * return:  interface index or VE_ERR_PORT
  */
int ve_query_port_intf_index(unsigned short vid, int devNum, int portNum);



/*********************************************************************/
/** for ioctl use ********************************************************/


/**
  * Init vlan/interface info table.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_init_intf_info_table(void);


/**
  * Destroy vlan/interface info table.
  * add by baolc
  * 2008-06-12
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_destroy_intf_info_table(void);



/**
  * Create vlan/interface info(add data into vlan/interface info table).
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_add_intf_info_by_vid(unsigned short vlanId, struct ve_vlan_param_data_s* vlan_data, unsigned int intfIdx);


/**
  * Delete vlan/interface info from vlan/interface info table.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_delete_intf_info_by_vid(unsigned short vlanId);


/**
  * Update vlan's port map.
  * add by baolc
  * 2008-06-11
  * return: VE_FAILED for failure, VE_SUCCESS for success
  */
int ve_update_intf_port_map(unsigned short vlanId, struct ve_vlan_param_data_s* vlan_data);


/**
  * Deprecated!!No use!!
  * Get idle index.
  * add by baolc
  */
int ve_get_intf_idle_index(void);

int ve_query_all_port(FDB_BROADCAST_ARRAY* b_array, unsigned short vlanId);
void ve_display_all_port(FDB_BROADCAST_ARRAY* b_array, unsigned short vid);


#endif
