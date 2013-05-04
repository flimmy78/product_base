#ifndef __NPD_INTF_H__
#define __NPD_INTF_H__

#include <dbus/dbus.h>
#include <pthread.h>
#include <net/if.h>
/* user or app part */
#include "npd_vlan.h"
/* include header file end */

#define NPD_AX7i_alpha_MAX_SLOTNO 1
#define NPD_AX7i_alpha_MAX_INTCONN_XPORT 2

struct asic_port_info_s {
	unsigned char devNum;
	unsigned char portNum;
};
extern struct asic_port_info_s ax7i_xg_conn_port_mapArr[NPD_AX7i_alpha_MAX_SLOTNO][NPD_AX7i_alpha_MAX_INTCONN_XPORT];

/* MACRO definition begin */
#ifndef MIN
#define MIN(a,b) ( (a)<(b) ? (a):(b) ) 
#endif

typedef struct{

 unsigned char arEther[6];
}GT_ETHERADDR;

#define NPD_INTF_SAVE_STATIC_ARP_MEM (65*1024)

#define NPD_INTF_SAVE_INTERRUPT_RXMAX_MEM  128

/* Ioctl defines */
#define KAPSETNOCSUM  _IOW('T', 210, struct if_cfg_struct) 
#define KAPSETDEBUG   _IOW('T', 211, struct if_cfg_struct) 
#define KAPSETIFF     _IOWR('T', 212, struct if_cfg_struct) 
#define KAPSETPERSIST _IOW('T', 213, struct if_cfg_struct) 
#define KAPSETOWNER   _IOW('T', 214, struct if_cfg_struct)
#define KAPSETLINK    _IOW('T', 215, struct if_cfg_struct)
#define KAPADDIFF     _IOW('T',216,struct if_cfg_struct)
#define KAPWAITING       _IOW('T',217,int)
#define KAPSETIPADDR _IOW('T',218,struct if_cfg_struct)
#define KAPGETIPADDR _IOWR('T',219,struct if_cfg_struct)
#define KAPRETURNPKGNO _IOW('T',220,int)
#define KAPSETDEVINFO  _IOWR('T',221,struct if_cfg_struct)
#define KAPGETDEVINFO _IOWR('T',225,struct if_cfg_struct)
#define KAPSETMACADDR _IOW('T',222,struct if_cfg_struct)
#define KAPGETMACADDR _IOWR('T',223,struct if_cfg_struct)
#define KAPDELIFF _IOWR('T',224,struct if_cfg_struct)
#define KAPGETIPADDRS _IOWR('T',226,struct if_cfg_struct)
#define KAPSETADVDIS     _IOW('T',227,struct if_cfg_struct)
#define KAPSETADVEN     _IOW('T',228,struct if_cfg_struct)

#define KAP_MAXNR	255
/* KAPSETIFF if flags */
#define KAP_IFF_TUN		0x0001
#define KAP_IFF_TAP		0x0002
#define KAP_IFF_NO_PI	0x1000
#define KAP_IFF_ONE_QUEUE	0x2000

#define NPD_INTF_CHECK_IP_ERROR 4
/* add by jinpc@autelan.com for check the legality of mac address */
#define NPD_INTF_CHECK_MAC_ERROR 5
#define L3_INTF_ERROR_BASE 18
#define L3_INTF_DIS_ROUTING_ERR 19
#define L3_INTF_EN_ROUTING_ERR 20
#define L3_INTF_EXIST 21
#define L3_INTF_NOTEXIST 22
#define NPD_DBUS_NOT_CREATE_ROUTE_PORT_SUB_INTF 23
#define NPD_DBUS_NOT_CREATE_VLAN_INTF_SUB_INTF 24
#define NPD_DBUS_ONLY_RUN_IN_VLAN 25
#define NPD_DBUS_ALREADY_ADVANCED 26
#define NPD_DBUS_NOT_ADVANCED 27
#define NPD_DBUS_PARENT_INTF_NOTEXSIT 28
#define NPD_DBUS_PROMI_SUBIF_EXIST 29
#define NPD_DBUS_PROMI_SUBIF_NOTEXIST 30
#define NPD_DBUS_PORT_NOT_IN_VLAN 31
#define NPD_MAC_MATCHED_BASE_MAC 32
#define L3_INTF_NOT_ACTIVE 33
#define INTF_NO_HAVE_ANY_IP 34
#define INTF_HAVE_THE_IP 35
#define INTF_NOT_SAME_SUB_NET 36
#define L3_INTF_STATUS_CHECK_ERR 37
#define INTF_GET_SYSMAC_ERR      38
#define NPD_INTF_VLAN_CONTAIN_PROMI_PORT 39
#define NPD_SUBIF_CREATE_FAILED 40
#define NPD_SUBIF_ADD_PORT_TO_VLAN_FAILED  41
#define NPD_ARP_SNOOPING_PORT_ADVANCED_ROUTING 42
#define INTF_ARP_SNOOPING_ADVANCED_ROUTING 43
#define NPD_INTERFACE_TRUNK_NOT_EXISTS     44
#define NPD_INTERFACE_TRUNK_NOT_IN_VLAN    45
#define NPD_INTERFACE_MAC_MATCHED_INTERFACE_MAC  46
#define NPD_INTF_PORT_NEED_VLAN 				47



#define INTF_NO_HAVE_THE_IP 0



#define MAX_IP_COUNT 8
#define INVALID_HOST_IP 0xFFFFFFFF
#define INVALID_L3_INTF_VID 0xffffffff
#define INVALID_PORT_INTF 0xffffffff

#define MAC_ADDRESS_LEN 6
#define MAC_ADDR_LEN MAC_ADDRESS_LEN
#define MAX_IFNAME_LEN 20

#define MAX_QINQ_TYPE_LEN 16

#define NUD_VALID	(NUD_PERMANENT|NUD_NOARP|NUD_REACHABLE|NUD_PROBE|NUD_STALE|NUD_DELAY)


/* mutex to protect packet received */
extern pthread_mutex_t 	semPktRxMutex;
/* mutex to protect KAP ioctl(except delete operation) */
extern pthread_mutex_t semKapIoMutex;
/* mutex to protect route table */
extern pthread_mutex_t semRtRwMutex;

struct usr_sockaddr {
	unsigned short 	sa_family;	/* address family, AF_xxx	*/
	char		sa_data[14];	/* 14 bytes of protocol address	*/
};

typedef enum type{

    KAP_INTERFACE_PORT_E = 0,
    KAP_INTERFACE_TRUNK_E,
    KAP_INTERFACE_VIDX_E,
    KAP_INTERFACE_VID_E
}KAP_DEV_TYPE;

typedef enum link_status{
	DEV_LINK_DOWN = 0,
	DEV_LINK_UP,
	DEV_LINK_MAX
}KAP_DEV_LINK_STATUS;


typedef enum NPD_INTF_OP_CODE{
	NPD_INTF_OP_NONE,
	NPD_INTF_CREATE_INTF,
	NPD_INTF_ADV_EN,
	NPD_INTF_ADV_DIS,
	NPD_INTF_DEL_INTF
} NPD_INTF_OP_CODE_E;


typedef struct if_cfg_struct
{
#define IFHWADDRLEN	6

	union
	{
		char	if_name[16];		/* if name, e.g. "en0" */
	} if_ifn;
	
	union {
		struct	usr_sockaddr ifu_addr[MAX_IP_COUNT];
		struct	usr_sockaddr ifu_dstaddr;
		struct	usr_sockaddr ifu_broadaddr;
		struct	usr_sockaddr ifu_netmask;
		struct  usr_sockaddr ifu_hwaddr;

		unsigned int	ifu_ivalue;
		unsigned int	ifu_flag;
	}if_ifu;

	KAP_DEV_TYPE dev_type;
	KAP_DEV_LINK_STATUS dev_state;
	unsigned int l3_index;
	unsigned int l2_index;
	unsigned int vId;
	unsigned int netmask[MAX_IP_COUNT];

	unsigned char mac_addr[6];

} VIRT_DEVICE_PARAM;

#define	if_name	if_ifn.if_name	/* interface name 	*/
#define	if_hwaddr	if_ifu.ifu_hwaddr	/* MAC address 		*/
#define	if_addr	if_ifu.ifu_addr	/* address		*/
#define	if_dstaddr	if_ifu.ifu_dstaddr	/* other end of p-p lnk	*/
#define	if_broadaddr	if_ifu.ifu_broadaddr	/* broadcast address	*/
#define	if_netmask	if_ifu.ifu_netmask	/* interface net mask	*/
#define	if_flag	if_ifu.ifu_flag	/* flags		*/
#define	if_owner if_ifu.ifu_ivalue /*set dev owner*/
#define	if_type if_ifu.ifu_flag      /*set dev type*/
#define	if_debug if_ifu.ifu_ivalue   /*set dev debug*/
/*structure for kap end*/


/* structure definition end */

/**********************************************************************************
 *  npd_create_intf_by_vId
 *
 *	DESCRIPTION:
 * 		this routine construct param of creating dev and call associated API
 *
 *	INPUT:
 *		vid -- mirror profile
 *		name -- dev name
 *	
 *	OUTPUT:
 *		ifindex -- dev index
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
 int npd_intf_create_by_vId
 (
 	unsigned int vId,
 	unsigned int* ifIndex,
 	char* name,
    unsigned int opCode
);

/**********************************************************************************
 *  npd_intf_create_vlan_l3intf
 *
 *	DESCRIPTION:
 * 		this routine create vlan virtual dev and set static FDB
 *
 *	INPUT:
 *		vid	-- vlan id	
 *		name  -- dev name
 *		addr	  --  system mac address	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_create_vlan_l3intf
(
	unsigned short 		vid,
	char* 	name,
	unsigned char* 	addr,
    unsigned int opCode
);

/**********************************************************************************
 *  npd_intf_create_port_index
 *
 *	DESCRIPTION:
 * 		this routine construct param of creating dev and call associated API
 *
 *	INPUT:
 *		port_index -- port index
 *		pvid 			  -- vid 
 *		name		  --  dev name	
 *	
 *	OUTPUT:
 *		ifindex       -- dev index
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 **********************************************************************************/
int npd_intf_create_by_port_index
(
	unsigned int port_index,
	unsigned short pvid,
	unsigned int* ifIndex,
	unsigned char* name,
	unsigned int opCode
);

/**********************************************************************************
 *  npd_intf_del_by_ifindex
 *
 *	DESCRIPTION:
 * 		this routine delete virtual dev 
 *
 *	INPUT:
 *		ifindex -- dev index	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
 int npd_intf_del_by_ifindex
(
	unsigned int ifindex,
	unsigned int opCode
);

/**********************************************************************************
 *  npd_intf_config_basemac
 *
 *	DESCRIPTION:
 * 		this routine set route mac
 *
 *	INPUT:
 *		devNum 
 *		port
 *		mac
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
 int npd_intf_config_basemac
(
	unsigned char  devNum,
	unsigned short port,
	unsigned char* mac
);


/**********************************************************************************
 *  npd_intf_get_intf_mac
 *
 *	DESCRIPTION:
 * 		this routine get dev mac address
 *
 *	INPUT:
 *		ifindex 			--  dev index
 *	
 *	OUTPUT:
 *		addr     		-- mac address
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_DBUS_ERROR
 *		
 *
 **********************************************************************************/
int npd_intf_get_intf_mac
(
	unsigned int ifIndex,
	unsigned char* addr
);

/**********************************************************************************
 *  npd_intf_set_vlan_l3intf_status
 *
 *	DESCRIPTION:
 * 		this routine set vlan dev status
 *
 *	INPUT:
 *		cmd  --  param 
 *		event -- up or down
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_set_vlan_l3intf_status
(
	unsigned short vid,
	enum vlan_status_e	event
);

/**********************************************************************************
 *  npd_intf_set_port_l3intf_status
 *
 *	DESCRIPTION:
 * 		this routine set port dev status
 *
 *	INPUT:
 *		eth_g_index -- destination port index
 *		event			--  up or down
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_set_port_l3intf_status
(
	unsigned int eth_g_index,
	enum ETH_PORT_NOTIFIER_ENT event
);

/**********************************************************************************
 *	npd_intf_get_ip_addrs
 *
 * 	This method is used to get L3 interface ip address and mask.
 *
 *	INPUT:
 *		ifindex - L3 interface index
 *
 *	OUTPUT:
 *		ipAddr - ip address
 *		mask - ip mask
 *
 * 	RETURN:
 *		NPD_SUCCESS 
 *		NPD_FAIL
 *
 **********************************************************************************/
int npd_intf_get_ip_addrs
(
	unsigned int   ifindex,
	unsigned int  *ipAddr,
	unsigned int  *mask
);

/**********************************************************************************
 *  npd_intf_execute_cmd
 *
 *	DESCRIPTION:
 * 		this routine call shell command 
 *
 *	INPUT:
 *		command -- command name
 *		cmd_param	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_execute_cmd
(
	const char* command, 
	const char** cmd_param
);

/**********************************************************************************
 *	npd_trans_ports_on_vlan
 *
 * 	trans all ports in the valn and enable ports router and arp trap.
 *
 *	INPUT:
 *		vid - vlan id
 *		enable - open/close
 *
 *	OUTPUT:
 *		NULL	
 *
 * 	RETURN:
 *		NPD_SUCCESS 
 *		NPD_FAIL
 *
 **********************************************************************************/
int npd_intf_trans_ports_on_vlan
(
	unsigned int vid,
	unsigned int enable,
	unsigned char * mac
);


/**********************************************************************************
 *	npd_intf_vlan_check
 *
 * 	check vlan member wether have port interface.
 *
 *	INPUT:
 *		vid - vlan id
 *
 *	OUTPUT:
 *		
 *		ifindex - l3 interface index	
 *
 * 	RETURN:
 *		NPD_TRUE - find  the vlan has port l3 interface 
 *		NPD_FALSE - don't find
 *
 **********************************************************************************/
int npd_intf_vlan_check
(
	unsigned short vid,
	unsigned int* ifindex
);

/**********************************************************************************
 *	npd_intf_port_check
 *
 * 	check port  wether has l3 interface or pvid has l3 interface.
 *
 *	INPUT:
 *		vid - vlan id
 *		eth_g_index - port global index
 *
 *	OUTPUT:
 *		
 *		ifindex - l3 interface index	
 *
 * 	RETURN:
 *		NPD_TRUE - find  the vlan has port l3 interface 
 *		NPD_FALSE - don't find
 *
 **********************************************************************************/
int npd_intf_port_check
(
	unsigned short vid,
	unsigned int eth_g_index,
	unsigned int* ifindex
);

/**********************************************************************************
 *	npd_intf_check_by_index
 *
 * 	check port  wether has l3 interface or the vlan that port take part in has l3 interface
 *
 *	INPUT:
 *		
 *		eth_g_index - port global index
 *     
 *
 *	OUTPUT:
 *		
 *		ifindex - l3 interface index	
 *
 * 	RETURN:
 *		NPD_DBUS_SUCCESS - find  the vlan has port l3 interface 
 *		NPD_DBUS_ERROR - don't find
 *
 **********************************************************************************/
int npd_intf_check_by_index
(
	unsigned int eth_g_index,
	unsigned int* isIfindex
);

/**********************************************************************************
 *  npd_intf_vlan_set_attr_for_cscd
 *
 *	DESCRIPTION:
 * 		this routine set attribute of port added the l3intf vlan 
 *
 *	INPUT:
 *		ifindex  -- dev index
 *		eth_g_index -- port index
 *		devNum			
 *		portNum			
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_vlan_set_attr_for_cscd
(
	unsigned int  ifindex,
	unsigned char devNum,
	unsigned char portNum
);

/**********************************************************************************
 *  npd_intf_vlan_set_attr_for_newport
 *
 *	DESCRIPTION:
 * 		this routine set attribute of port added the l3intf vlan 
 *
 *	INPUT:
 *		ifindex  -- dev index
 *		eth_g_index -- port index
 *		devNum			
 *		portNum			
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_vlan_set_attr_for_newport
(
	unsigned int    ifindex,
	unsigned int    eth_g_index,
	unsigned char devNum,
	unsigned char portNum
);

/**********************************************************************************
 *  npd_intf_vlan_set_default_attr_for_oldport
 *
 *	DESCRIPTION:
 * 		this routine set default attribute for port that deleted from l3intf vlan
 *
 *	INPUT:
 *		eth_g_index -- destination port index
 *		devNum			
 *		portNum	
 *
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 **********************************************************************************/
unsigned int npd_intf_vlan_set_default_attr_for_oldport
(
	unsigned int    eth_g_index,
	unsigned char devNum,
	unsigned char portNum
);


/**********************************************************************************
 *  npd_intf_update_vlan_info_to_advanced_routing_l3intf
 *
 *	DESCRIPTION:
 * 		this routine update ports info to kernel dev
 *
 *	INPUT:
 *		vid -- vlan id	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_update_vlan_info_to_advanced_routing_l3intf
(
	unsigned short vid
);

/**********************************************************************************
 *  npd_intf_get_route_mode_mac
 *
 *	DESCRIPTION:
 * 		this routine get specific mac address for route mode eth-port
 *		as default, L3 eth-port interface in route mode has same system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		eth_g_index  -  global eth-port index 
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		macAddr - mac address to set*		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_get_route_mode_mac
(
	unsigned int eth_g_index,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_set_route_mode_mac
 *
 *	DESCRIPTION:
 * 		this routine set specific mac address for route mode eth-port
 *		as default, L3 eth-port interface in route mode has same system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		eth_g_index  -  global eth-port index 
 *		ifIndex - L3 interface index
 *		macAddr - mac address to set
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_set_route_mode_mac
(
	unsigned int eth_g_index,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_get_l3_vlan_mac
 *
 *	DESCRIPTION:
 * 		this routine get specific mac address for vlan interface
 *		as default, vlan interface has system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		vid  -  vlan id
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		macAddr - mac address got		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_get_l3_vlan_mac
(
	unsigned short vid,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_set_l3_vlan_mac
 *
 *	DESCRIPTION:
 * 		this routine set specific mac address for vlan interface
 *		as default, vlan interface has system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		vid  -  vlan id
 *		ifIndex - L3 interface index
 *		macAddr - mac address to set
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_set_l3_vlan_mac
(
	unsigned short vid,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_get_promisc_mode_mac
 *
 *	DESCRIPTION:
 * 		this routine get specific mac address for promiscuous mode eth-port
 *		as default, L3 eth-port interface in promiscuous mode has same system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		eth_g_index  -  global eth-port index 
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		macAddr - mac address to set*		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_get_promisc_mode_mac
(
	unsigned int eth_g_index,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_set_promisc_mode_mac
 *
 *	DESCRIPTION:
 * 		this routine set specific mac address for promiscuous mode eth-port
 *		as default, L3 eth-port interface in promiscuous mode has same system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		eth_g_index  -  global eth-port index 
 *		ifIndex - L3 interface index
 *		macAddr - mac address to set
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_set_promisc_mode_mac
(
	unsigned int eth_g_index,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_get_advanced_route_vlan_mac
 *
 *	DESCRIPTION:
 * 		this routine get specific mac address for advanced-route vlan interface
 *		as default, advanced-route vlan interface has system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		vid  -  vlan id
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		macAddr - mac address got	
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_get_advanced_route_vlan_mac
(
	unsigned short vid,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_set_advanced_route_vlan_mac
 *
 *	DESCRIPTION:
 * 		this routine set specific mac address for advanced-route vlan interface
 *		as default, advanced-route vlan interface has system mac, and
 *		value saved in sw is bcast address FF:FF:FF:FF:FF:FF
 *
 *	INPUT:
 *		vid  -  vlan id
 *		ifIndex - L3 interface index
 *		macAddr - mac address to set
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL	- if error occurred
 *		NPD_SUCCESS - if all ok
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_set_advanced_route_vlan_mac
(
	unsigned short vid,
	unsigned int ifIndex,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_intf_udp_bc_trap_enable
 *
 *	DESCRIPTION:
 * 		this routine set udp bc trap
 *
 *	INPUT:
 *		vlanid 
 *		enable  
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
unsigned int npd_intf_udp_bc_trap_enable
(
	unsigned int vlanId,
	unsigned int enable
);

/**********************************************************************************
 *	npd_intf_create_router_mode
 *
 * 	create intf based on ports,support l3 transmit.
 *
 *	INPUT:
 *		port_index -port index 
 *		pname - dev name
 *
 *	OUTPUT:
 *		NULL	
 *
 * 	RETURN:
 *		NPD_SUCCESS 
 *		NPD_FAIL
 *
 **********************************************************************************/
int npd_intf_create_router_mode
(
	unsigned int port_index,
	unsigned char* pname,
    unsigned int opCode
);

/**********************************************************************************
 *	npd_intf_del_route_mode
 *
 * 	delete intf based on ports.
 *
 *	INPUT:
 *		port_index -port index 
 *
 *	OUTPUT:
 *		NULL	
 *
 * 	RETURN:
 *		NPD_SUCCESS 
 *		NPD_FAIL
 *
 **********************************************************************************/
int npd_intf_del_route_mode
(
	unsigned int port_index,
    unsigned int opCode
);

/**********************************************************************************
 *  npd_intf_get_info
 *
 *	DESCRIPTION:
 * 		this routine get dev infor
 *
 *	INPUT:
 *		name -- dev name
 *	
 *	OUTPUT:
 *		ifindex -- dev index
 *		vid		-- vlan id
 *		eth-g-index -- port index
 *		type 	-- dev type
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_get_info
(
   char * name,
   unsigned int * ifIndex,
   unsigned int * vid,
   unsigned int * eth_g_index,
   KAP_DEV_TYPE * type
);

/**********************************************************************************
 *  npd_intf_set_mac_address
 *
 *	DESCRIPTION:
 * 		this routine set mac address for a specified l3 interface
 *
 *	INPUT:
 *		name -- dev name
 *	
 *	OUTPUT:
 *		ifindex -- dev index
 *		vid		-- vlan id
 *		eth-g-index -- port index
 *		type 	-- dev type
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_intf_set_mac_address
(
  	unsigned int ifIndex,
 	unsigned char * macAddr
);

void npd_intf_init();

extern int nam_static_fdb_entry_mac_vlan_port_set
(	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
);

extern int nam_cscd_route_on_vlan_enable
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int enable
);

extern int nam_ports_route_on_vlan_enable
(
	unsigned int port_index, 
	unsigned int enable
);

extern int nam_static_fdb_entry_mac_vlan_port_del
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
) ;

extern unsigned int cpssDxChBrgGenArpTrapEnable
(
    unsigned char        dev,
    unsigned char        port,
    unsigned int      	enable
);

extern  int nam_l3_intf_on_port_enable
(
	unsigned int port_index,
	unsigned short vid,
	unsigned int enable
);

extern int  nam_l3_intf_on_vlan_enable
(
	unsigned char devNum,
	unsigned int vid, 
	unsigned char* addr,
	unsigned int enable,
	unsigned int ifindex
);

extern int nam_route_config();

extern int nam_rip_send_to_cpu
(
	unsigned int enable
);

extern unsigned int cpssDxChBrgVlanUnkUnregFilterSet
(
    unsigned char                                devNum,
    unsigned short                               vlanId,
    unsigned int   									packetType,
    unsigned int					                cmd

);

extern int cpssDxChBrgVlanIpCntlToCpuSet
(
    unsigned char                               devNum,
    unsigned short                              vlanId,
    unsigned int      							  ipCntrlType
);

extern int  cpssDxChBrgGenArpBcastToCpuCmdSet
(
    unsigned char                dev,
    unsigned int				  cmd
);

extern unsigned int nam_asic_vlan_get_drv_port_bmp
(
	unsigned short	vlanId,
	struct DRV_VLAN_PORT_BMP_S *mbrBmp,
	struct DRV_VLAN_PORT_BMP_S *tagBmp
);

extern void npd_syslog_dbg
(
	char *format,
	...
);

extern int nam_static_fdb_entry_mac_set_for_l3
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
);
extern unsigned int nam_asic_udp_bc_trap_en
(
    unsigned short vid,
    unsigned int enable
);
extern unsigned int npd_arp_clear_by_vid_ifIndex
(
	unsigned short   vid,
	unsigned int  ifIndex
);
extern unsigned int npd_all_arp_clear_by_port
(
	unsigned int   eth_g_index
);
extern int nam_no_static_fdb_entry_mac_vlan_set
(
    unsigned char macAddr[],
    unsigned short vlanId
); 

extern unsigned long cpssDxChBrgVlanMemberAdd
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned char   portNum,
    unsigned long   isTagged
);

extern unsigned long cpssDxChBrgVlanPortDelete
(
    unsigned char           devNum,
    unsigned short          vlanId,
    unsigned char           port
);

/*******************************************************************************
* osSemWait
*
* DESCRIPTION:
*       Wait on semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long osSemWait
(
    void *smid,
    unsigned long timeOut
);

/*******************************************************************************
* osSemSignal
*
* DESCRIPTION:
*       Signal a semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long osSemSignal
(
    void *smid
);

extern unsigned int npd_eth_port_get_slotno_portno_by_eth_g_index
(
    unsigned int eth_g_index,
    unsigned char *slot_no,
    unsigned char *port_no
);

/***********************************************************************************
 *		NPD dbus operation
 *
 ***********************************************************************************/
DBusMessage * npd_dbus_create_intf_by_vid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_del_intf_by_vid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_create_sub_intf
	(	
		DBusConnection *conn,
		DBusMessage *msg,
		void *user_data
	);

DBusMessage * npd_dbus_del_sub_intf
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_advanced_route
	(
		DBusConnection *conn,
		DBusMessage *msg, 
		void *user_data
	);

unsigned int npd_promis_port_add2_vlan_intf
(
	unsigned int permit
);
DBusMessage * npd_dbus_create_intf_by_vlan_ifname
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
);
DBusMessage * npd_dbus_vlan_interface_advanced_routing_enable
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
);
unsigned int npd_intf_ifindex_get_by_ifname
(
    unsigned char *ifName,
    unsigned int *ifIndex
);

/*get macAddr by ifIndex and vid or ifIndex by eth_g_index*/
unsigned int npd_intf_get_l3_intf_mac
(
    unsigned int ifIndex,
    unsigned short vid,
    unsigned int eth_g_index,
    unsigned char * macAddr
);

/*************************************************
  * npd_intf_netlink_get_ip_addrs
 *
 *DESCRIPTION:
 *    get ip addresses by netlink
 *
 *INPUT:
 *    ifname -- the interface name we want to get its ip
 *OUTPUT:
 *    ifIndex -- the interface ifIndex
 *    ipAddrs -- the pointer of ip addresses we got
 *    masks  -- the pointer of masks for ip address above
 *RETURN:
 *    NPD_FAIL -- get ip addresses FAILED
 *    NPD_SUCCESS -- get ip address success
 *NOTE:
 *
 ************************************************/
int npd_intf_netlink_get_ip_addrs
(
    unsigned char* ifname,
    unsigned int* ifIndex,
    unsigned int* ipAddrs,
    unsigned int* masks
);

/****************************************************
 *npd_intf_l3_mac_legality_check
 *	Description:
 *		to check the mac  of L3 interface legality, 
 *		multicast mac , broadcast mac, and all zero mac is in vaild 
 *
 *	Input:
 *		ethmac		- source mac
 *
 *	Output:
 *		NULL
 *
 *	ReturnCode:
 *		NPD_TRUE		- the L3 macs  legal
 *		NPD_FALSE		- the L3 macs  illegal
 *		
 ****************************************************/
unsigned int npd_intf_l3_mac_legality_check
(
    unsigned char * ethmac
);
DBusMessage * npd_dbus_interface_advanced_routing_show_running
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_interface_static_arp_show_running
    (
        DBusConnection *conn, 
        DBusMessage *msg, 
        void *user_data
    );

int npd_intf_get_vlan_advanced_routing_count
(
	void
);
void npd_intf_vlan_advanced_routing_save_cfg
(
	char ** showStr,
	int* avalidLen
);
void npd_intf_eth_port_advanced_routing_save_cfg
(
	char ** showStr,
	int* avalidLen,
	unsigned int includeRgmii
);

int npd_intf_change_intf_name
	(
		unsigned char * oldName,
		unsigned char * newName,
		unsigned int 	state
	);

int npd_intf_set_intf_state
(
	unsigned char * ifname,
	unsigned int state
);

extern DBusMessage * npd_dbus_eth_interface_enable_set_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data);


extern DBusMessage * npd_dbus_intf_eth_interface_enable_set
	 (
		   DBusConnection *conn, 
		   DBusMessage *msg, 
		   void *user_data
	 ) ;
 


#endif
