#ifndef __NPD_ETH_PORT_H__
#define __NPD_ETH_PORT_H__

#include <dbus/dbus.h>
#include <linux/if_vlan.h>

#define CONFIG_INTF_ETH  0
#define CONFIG_INTF_E    1
#define CONFIG_INTF_CSCD 2
#define CONFIG_INTF_OBC  3
#define CONFIG_INTF_VE   4
#define CONFIG_INTF_MNG  5
#define CSCD_IP_MASK 24

#define IN
#define OUT

extern struct eth_port_s **g_eth_ports;
extern unsigned int *g_fiber_port_poll;
extern NPD_ETH_PORT_NOTIFIER_FUNC	portNotifier;
extern unsigned int promis_port_add2_intf;

#define PROMISCUOUS_TARGET_DEV_NUM 1
#define PROMISCUOUS_TARGET_PORT_NUM  26

#define PROMISCUOUS_AU5612_TARGET_DEV 0
#define PROMISCUOUS_AU5612_TARGET_PORT 0

#define PROMISCUOUS_AU5612I_TARGET_DEV 0
#define PROMISCUOUS_AU5612I_TARGET_PORT 24

#define ETH_PORT_ALREADY_RUN_THIS_MODE 55
#define NPD_PORT_NOT_SUPPORT_MEDIA_SEL 0xFF
#define ETH_PORT_ALREADY_IN_L3_VLAN 56 
#define ETH_PORT_DEFAULT_VLAN_IS_L3_VLAN  57
#define ETH_PORT_HAVE_SUBIF 58
#define HYPERG_STACK_PORT1 26
#define HYPERG_STACK_PORT2 27
#define NPD_AX7KI_OBC0_PORT_DEV_NUM  0
#define NPD_AX7KI_OBC0_PORT_PORT_NUM  12

#define NPD_ETHPORT_SHOWRUN_CFG_SIZE	(3*1024) /* for all 24GE ports configuration */

#define ACL_DIRECTION_INGRESS 0
#define ACL_DIRECTION_EGRESS  1
#define ACL_DIRECTION_TWOWAY  2

#define MAX_ETH_GLOBAL_INDEX ((CHASSIS_SLOT_COUNT)*(MAX_ETHPORT_PER_BOARD))

#define ETH_PORT_TYPE_FROM_LOCAL_INDEX(slot_index,local_eth_index) g_eth_ports[ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_index),(local_eth_index))]->port_type
#define ETH_PORT_ATTRBITMAP_FROM_LOCAL_INDEX(slot_index,local_eth_index) g_eth_ports[ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_index),(local_eth_index))]->attr_bitmap
#define ETH_PORT_MTU_FROM_LOCAL_INDEX(slot_index,local_eth_index) g_eth_ports[ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_index),(local_eth_index))]->mtu
#define ETH_PORT_IPG_FROM_LOCAL_INDEX(slot_index,local_eth_index) g_eth_ports[ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_index),(local_eth_index))]->ipg
#define ETH_PORT_LINK_TIME_CHANGE_FROM_LOCAL_INDEX(slot_index,local_eth_index) g_eth_ports[ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX((slot_index),(local_eth_index))]->lastLinkChange

#define ETH_PORT_TYPE_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->port_type
#define ETH_PORT_ATTRBITMAP_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->attr_bitmap
#define ETH_PORT_MTU_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->mtu
#define ETH_PORT_IPG_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->ipg
#define ETH_PORT_LINK_TIME_CHANGE_FROM_GLOBAL_INDEX(eth_g_index) g_eth_ports[(eth_g_index)]->lastLinkChange



#define ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno) ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(CHASSIS_SLOT_NO2INDEX(slotno),ETH_LOCAL_NO2INDEX(CHASSIS_SLOT_NO2INDEX(slotno),(portno)))
#define NPD_SHOW_ARP_DETAIL_INFO 10
/*define QOS_POLICY_MAP_BIND		   7*/


#define SET_VLAN_QINQ_TYPE_CMD (GET_VLAN_VID_CMD+1)      /* for add a member to enum vlan_ioctl_cmds of compiler and kernel */
#define GET_VLAN_QINQ_TYPE_CMD  (SET_VLAN_QINQ_TYPE_CMD + 1) /* for get dev's qinq-type */

enum port_type
{
	CPU,
	ASIC,
	UNKNOWN
};

#define	ETH_OCTEON_PORT 0
#define	ETH_ASIC_PORT       1 
typedef enum {
	PORT_FULL_DUPLEX_E,
	PORT_HALF_DUPLEX_E
} PORT_DUPLEX_ENT;

typedef enum {
	PORT_SPEED_10_E,
	PORT_SPEED_100_E,
	PORT_SPEED_1000_E,
	PORT_SPEED_10000_E,
	PORT_SPEED_12000_E,
	PORT_SPEED_2500_E,
	PORT_SPEED_5000_E
}PORT_SPEED_ENT;


typedef enum{
	NPD_PORT_TYPE_NONE=0,
	NPD_PORT_TYPE_SWITCH,
	NPD_PORT_TYPE_ROUTE,
	NPD_PORT_TYPE_PROMISCUOUS
}NPD_PORT_MODE;

typedef enum {	
	AX51_XFP = 1,
	AX51_GTX = 2,
	AX51_SFP = 3
}SUBCARD_PORT_TYPE;

typedef enum {
	PORT_TYPE_SGMII = 0,
	PORT_TYPE_1000BASEX,
	PORT_TYPE_MAX	
}CPSS_DXCH_PORT_TYPE_E;

typedef enum {	
	PORT_INBAND_AN_SGMII_MODE = 0,
	PORT_INBAND_AN_1000BASEX_MODE
}CPSS_DXCH_PORT_INBAND_AN_MODE;

typedef enum
{
    CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E,
    CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E
}CPSS_DXCH_PORT_BUFFERS_MODE_ENT;

typedef CPSS_DXCH_PORT_BUFFERS_MODE_ENT NPD_ETHPORT_BUFFER_MODE_E;

typedef enum
{
	CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MIN_E = 0,
	CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MAX_E
} CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT;

typedef enum 
{
	CPSS_DXCH_XG_PORT_IPG_LAN_MODE_E = 0,
	CPSS_DXCH_XG_PORT_IPG_WAN_MODE_E ,
	CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E ,
} CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT;

typedef struct
{
    unsigned char       arEther[6];
}FC_ETHERADDR;

/*
 * ASIC port info
 */
typedef struct _asic_port_s_{
	unsigned char devNum;	/* asic device # */
	unsigned char portNum;  /* asic port # */
}asic_port_t;



typedef struct _eth_port_sc_cfg_s_ {
	unsigned char ppsValid:1,
				  bpsValid:1,
				  rsvd:6;
	union{
		unsigned int pps;
		unsigned int bps;
	}value;
} eth_port_sc_cfg_t;

typedef struct _eth_port_sc_ctrl_s_ {
	eth_port_sc_cfg_t dlf;
	eth_port_sc_cfg_t bcast;
	eth_port_sc_cfg_t mcast;
} eth_port_sc_ctrl_t;

typedef enum{
   ETH_PORT_STREAM_FE_E,
   ETH_PORT_STREAM_GE_E
}ETH_PORT_STREAM_TYPE_E;
typedef enum{
   ETH_PORT_STREAM_PPS_E,
   ETH_PORT_STREAM_BPS_E,
   ETH_PORT_STREAM_INVALID_E
}ETH_PORT_STREAM_MODE_E;

#define	PORT_STORM_CONTROL_STREAM_DLF            0x01
#define	PORT_STORM_CONTROL_STREAM_MCAST          0x02
#define	PORT_STORM_CONTROL_STREAM_BCAST          0x04
#define	PORT_STORM_CONTROL_STREAM_UCAST          0x08  
#define	PORT_STORM_CONTROL_STREAM_ALL          (PORT_STORM_CONTROL_STREAM_DLF | \
	                                            PORT_STORM_CONTROL_STREAM_MCAST | \
	                                            PORT_STORM_CONTROL_STREAM_MCAST| \
	                                            PORT_STORM_CONTROL_STREAM_UCAST) 

/**********************************************************************************
 *
 *		set port default attribute value
 *
 **********************************************************************************/
 #define ETH_PORT_ADMIN_STATUS_DEFAULT	ETH_ATTR_ENABLE
 #define CRSMU_RGMII_PORT_MRU_DEFAULT	1518
 #define ETH_PORT_MRU_DEFAULT	  		1536
 #define ETH_PORT_DUPLEX_DEFAULT 		PORT_FULL_DUPLEX_E
 #define ETH_PORT_SPEED_DEFAULT			PORT_SPEED_1000_E
 #define ETH_PORT_AN_DEFAULT			ETH_ATTR_ON 	/* all Auto-Nego options  */
 #define ETH_PORT_AN_DUPLEX_DEFAULT		ETH_ATTR_ON		/* duplex mode Auto-Nego*/	
 #define ETH_PORT_AN_FC_DEFAULT			ETH_ATTR_ON		/* flow control Auto-Nego*/
 #define ETH_PORT_AN_SPEED_DEFAULT		ETH_ATTR_ON		/* port speed Auto-Nego*/
 #define ETH_PORT_FC_STATE_DEFAULT		ETH_ATTR_ENABLE /* flow control state*/
 #define ETH_PORT_BP_STATE_DEFAULT		ETH_ATTR_DISABLE /* back-pressure state*/
 #define ETH_PORT_MEDIA_DEFAULT			ETH_ATTR_MEDIA_NOT_EXIST_PRIO /* media status*/
 #define ETH_PORT_LED_INTF_DEFAULT		ETH_ATTR_ENABLE
 #define ETH_PORT_IPG                   12
void npd_dbus_save_buffer_mode_cfg
(
	char ** showStr,
	int* avalidLen
);
void npd_dbus_ethport_vct_cfg
(
	char ** showStr,
	int* avalidLen
);	
/**********************************************************************************
 *  npd_get_port_type
 *
 *	DESCRIPTION:
 * 		this routine get port type from module type and port number
 *
 *	INPUT:
 *		module_id - module type,such as CRSMU,6GTX,6GE SFP etc.
 *		eth_port_no - ethernet port number 
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
enum eth_port_type_e npd_get_port_type
(
	enum module_id_e module_id, 
	int eth_port_no
) ;

int npd_port_media_get
(
	unsigned int eth_g_index,
	enum eth_port_type_e *portMedia
);


/**********************************************************************************
 *  npd_create_eth_port
 *
 *	DESCRIPTION:
 * 		this routine create ethernet port with <slot,port> and 
 *		attach port info to global data structure.
 *
 *	INPUT:
 *		slot_index - slot number(index is the same as panel slot number)
 *		eth_local_index - port index(index may different from panel number)
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_create_eth_port
(
	unsigned int slot_index,
	unsigned int eth_local_index
) ;

/**********************************************************************************
 *  npd_init_eth_ports
 *
 *	DESCRIPTION:
 * 		this routine initialize all ethernet ports
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_init_eth_ports(void) ;

/**********************************************************************************
 *  npd_get_subcard_port_type
 *
 *	DESCRIPTION:
 * 		get 5612, 5612i, 4626 subcard type port index
 *
 *	INPUT:
 *		unsigned int - card_num
 *	
 *	OUTPUT:
 *		unsigned char - subcard port type
 *						01 ----- AX51-XFP
 *						10 ----- AX51-GTX
 *						11 ----- AX51-SFP
 *
 * 	RETURN:
 * 		unsigned char - subcard whether on or not
 *											
 *
 **********************************************************************************/
unsigned char npd_get_subcard_port_type
(
	unsigned int card_num,
	unsigned char* subcard_type
);

/**********************************************************************************
 *  npd_check_subcard_type
 *
 *	DESCRIPTION:
 * 		get 5612, 5612i, 4626 subcard type
 *
 *	INPUT:
 * 		unsigned int - panel port number (currently start from 1);
 *	
 *	OUTPUT:
 *  	NULL
 *
 * 	RETURN:
 * 		unsigned char - subcard port type
 *						01 ----- AX51-XFP
 *						10 ----- AX51-GTX
 *						11 ----- AX51-SFP					
 *		NPD_FALSE  -  subcard is not on its slot
 **********************************************************************************/
unsigned char npd_check_subcard_type
(
	unsigned int product_Id,
	unsigned int panel_port
);

/**********************************************************************************
 *  npd_get_port_by_index
 *
 *	DESCRIPTION:
 * 		get ethernet port info by global eth index
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		portInfo	- if port found
 *		
 **********************************************************************************/
struct eth_port_s *npd_get_port_by_index
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_set_pvid
 *
 *	DESCRIPTION:
 * 		set ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_set_pvid
(
	unsigned int   eth_g_index,
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_eth_port_get_pvid
 *
 *	DESCRIPTION:
 * 		get ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		vlanId - vlan id
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_get_pvid
(
	unsigned int   eth_g_index,
	unsigned short *vlanId
);

/**********************************************************************************
 *  npd_eth_port_clear_pvid
 *
 *	DESCRIPTION:
 * 		clear or delete ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_clear_pvid
(
	unsigned int   eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_dot1q_add
 *
 *	DESCRIPTION:
 * 		set ethernet port vid(802.1q-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_dot1q_add
(
	unsigned int   eth_g_index,
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_eth_port_dot1q_remove
 *
 *	DESCRIPTION:
 * 		remove ethernet port vid(802.1q-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_dot1q_remove
(
	unsigned int   eth_g_index,
	unsigned short vlanId
);
unsigned int npd_eth_port_based_vlan_add
(
	unsigned int eth_g_index,
	unsigned short vlanId
);
unsigned int npd_eth_port_based_vlan_del
(
	unsigned int eth_g_index,
	unsigned short vlanId
);
unsigned int npd_eth_port_set_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short trunkId
);
/**********************************************************************************
 *  npd_eth_port_get_ptrunkid
 *
 *	DESCRIPTION:
 * 		get ethernet port trunkid(port-based trunk id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		trunkId - trunk id
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_get_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short *trunkId
);
/**********************************************************************************
 *  npd_eth_port_clear_ptrunkid
 *
 *	DESCRIPTION:
 * 		clear or delete ethernet port vid(port-based vlan id)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL	 - if error occurred
 *		
 **********************************************************************************/
unsigned int npd_eth_port_clear_ptrunkid
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 *  npd_get_eth_ports_status
 *
 *	DESCRIPTION:
 * 		get ethernet port status
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_eth_ports_status() ;

/**********************************************************************************
 *  npd_scan_eth_ports_link_status
 *
 *	DESCRIPTION:
 * 		link scan ethernet port status
 *
 *	INPUT:
 *		NULL
 *		
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_scan_eth_ports_link_status();

/**********************************************************************************
 *  npd_get_port_admin_status
 *
 *	DESCRIPTION:
 * 		get ethernet port admin status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - admin status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_admin_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
) ;
/**********************************************************************************
 *  npd_set_port_admin_status
 *
 *	DESCRIPTION:
 * 		set ethernet port admin status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -admin status enable or disable
 *	
 *	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_set_port_admin_status
(
	IN unsigned int eth_g_index,
	IN unsigned int status,
	IN unsigned int isBroad
);

/**********************************************************************************
 *  npd_get_port_link_status
 *
 *	DESCRIPTION:
 * 		get ethernet port link status:link up or link down
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - admin status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
unsigned int npd_get_port_link_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
) ;
/**********************************************************************************
 *  npd_set_port_link_status
 *
 *	DESCRIPTION:
 * 		set ethernet port link status:link up or link down
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -linkstatus enable or disable
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 **********************************************************************************/
int npd_set_port_link_status
(
	IN unsigned int eth_g_index,
	IN unsigned int status,
	IN unsigned int isBoard
);

/**********************************************************************************
 *  npd_get_port_autoNego_status
 *
 *	DESCRIPTION:
 * 		get ethernet port auto-negotiation status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status - auto-negotiation status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_autoNego_status
(
	unsigned int eth_g_index,
	unsigned int *status
) ;

/**********************************************************************************
 *  npd_set_port_autoNego_status
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation status:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego status enable or disable
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_status
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_set_port_autoNego_duplex
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation duplex:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego duplex enable or disable
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_duplex
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_set_port_autoNego_speed
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation speed:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego speed enable or disable
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_speed
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_set_port_autoNego_flowctrl
 *
 *	DESCRIPTION:
 * 		set ethernet port auto-negotiation flowcontrol:enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status -autoNego speed enable or disable
 *	
 * RETURN:
 *		NPD_FALI
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_autoNego_flowctrl
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *	npd_get_port_attrmap_by_gindex
 *
 *	DESCRIPTION:
 *		get ethernet port attribute bit map
 *
 *	INPUT:
 *		eth_g_index - ethernet global index 	
 *	
 *	OUTPUT:
 *		local_port_attrmap - port attribute bit map
 *
 *	RETURN:
 *		ret
 *		
 *
 **********************************************************************************/
unsigned int 
npd_get_port_attrmap_by_gindex
(
	unsigned int eth_g_index,
	unsigned int *isWAN,
	unsigned int *local_port_attrmap
);

/**********************************************************************************
 *  npd_get_port_duplex_mode
 *
 *	DESCRIPTION:
 * 		get ethernet port duplex status:half or full
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		mode - duplex half or duplex full
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_duplex_mode
(
	unsigned int eth_g_index,
	unsigned int *mode
) ;

/**********************************************************************************
 *  npd_set_port_duplex_mode
 *
 *	DESCRIPTION:
 * 		set ethernet port duplex status:half or full
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		mode - full or half
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_duplex_mode
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_get_port_speed
 *
 *	DESCRIPTION:
 * 		get ethernet port speed :10M ,100M or 1000M etc.
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		speed - port speed 10M or 100M or 1000M
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_speed
(
	unsigned int eth_g_index,
	unsigned int *speed
) ;

/**********************************************************************************
 *  npd_set_port_speed
 *
 *	DESCRIPTION:
 * 		set ethernet port speed :10M ,100M or 1000M etc.
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		speed 10M,100M,1000M
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_speed
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_get_port_flowCtrl_state
 *
 *	DESCRIPTION:
 * 		get ethernet port flow control status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status  - flow control is enable or disable
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_flowCtrl_state
(
	unsigned int eth_g_index,
	unsigned int *status
) ;

/**********************************************************************************
 *  npd_set_port_flowCtrl_state
 *
 *	DESCRIPTION:
 * 		set ethernet port flow control status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status - enable or disable
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_flowCtrl_state
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_get_port_backPressure_state
 *
 *	DESCRIPTION:
 * 		get ethernet port back pressure status: enable or disable
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		status  - back pressure is enable or disable
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_backPressure_state
(
	unsigned int eth_g_index,
	unsigned int *status
) ;

/**********************************************************************************
 *  npd_set_port_backPressure_state
 *
 *	DESCRIPTION:
 * 		set ethernet port back pressure status: enable or disable
 *			
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		status - enable or disable
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_backPressure_state
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_get_port_mru
 *
 *	DESCRIPTION:
 * 		get ethernet port MRU
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *	
 *	OUTPUT:
 *		mru  - Maximum Receive Unit value
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_get_port_mru
(
	unsigned int eth_g_index,
	unsigned int *mru
) ;

/**********************************************************************************
 *	npd_eth_port_config_stp
 *		set ethernet port Spanning-Tree state
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		state - enable or disable Spanning-Tree protocol on port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   memory allocation failed
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
int npd_eth_port_config_stp
(
	unsigned short	eth_g_index,
	unsigned int 	 mode,
	unsigned int	state
);

/**********************************************************************************
 *  npd_set_port_mru
 *
 *	DESCRIPTION:
 * 		set ethernet port MRU
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *		mru <64-8912>
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_mru
(
	unsigned int eth_g_index,
	unsigned int status,
	unsigned int isBoard
);

/**********************************************************************************
 *  npd_set_port_attr_default
 *
 *	DESCRIPTION:
 * 		set ethernet port default value
 *
 *	INPUT:
 *		eth_g_index - ethernet global index		
 *
 * RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *
 **********************************************************************************/
int npd_set_port_attr_default
(
	unsigned int eth_g_index,
	unsigned int isBoard,
	enum module_id_e moduleId
);

/**********************************************************************************
 *	npd_eth_port_l3intf_create
 *
 *    create Layer 3 ethernet port interface(actually add L3 interface index to eth port structure)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 				   memory allocation failed
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_create
(
	unsigned short	eth_g_index,
	unsigned int	ifIndex
);

/**********************************************************************************
 *	npd_eth_port_l3intf_destroy
 *
 *    destroy Layer 3 ethernet port interface(actually delete L3 interface index from eth port structure)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		ifIndex - L3 interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_destroy
(
	unsigned short	eth_g_index,
	unsigned int	ifIndex
);

/**********************************************************************************
 *  npd_eth_port_interface_check
 *
 *	DESCRIPTION:
 * 		check out if specified ethernet port is a L3 interface
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		ifIndex - L3 interface index
 *
 * 	RETURN:
 *		NPD_TRUE  - if eth port is a L3 interface
 *		NPD_FALSE - if eth port is not a L3 interface
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_interface_check
(
	unsigned int 	eth_g_index,
	unsigned int   *ifIndex
);

/**********************************************************************************
 *	npd_eth_port_l3intf_mac_get
 *
 *    Get Layer 3 ethernet port interface mac address
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		macAddr - mac address
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_mac_get
(
	unsigned short	eth_g_index,
	unsigned char *macAddr
);

/**********************************************************************************
 *	npd_eth_port_l3intf_mac_set
 *
 *    Set Layer 3 ethernet port interface mac address
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		macAddr - mac address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_mac_set
(
	unsigned short	eth_g_index,
	unsigned char *macAddr
);

/**********************************************************************************
 *	npd_eth_port_l3intf_ustatus_get
 *
 *    Get Layer 3 ethernet port interface status in user space(NPD scope)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		state - l3 interface status
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_ustatus_get
(
	unsigned short	eth_g_index,
	unsigned char *state
);

/**********************************************************************************
 *	npd_eth_port_l3intf_ustatus_set
 *
 *    Set Layer 3 ethernet port interface status in user space(NPD scope)
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		state - l3 interface status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FAIL - if ethernet port is not exists or
 *				   input parameter illegal or
 *				   port is not route mode interface
 *		NPD_SUCCESS - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_l3intf_ustatus_set
(
	unsigned short	eth_g_index,
	unsigned char state
);

/**********************************************************************************
 *  npd_eth_port_check_promi_subif
 *
 *	DESCRIPTION:
 * 		check promi subif vlan id
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid 				- vlan id
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_TRUE
 *		NPD_FALSE
 *		
 *
 **********************************************************************************/
int npd_eth_port_check_promi_subif
(
	unsigned int eth_g_index,
	unsigned short  vid
);

/**********************************************************************************
 *  npd_eth_port_create_promi_subif
 *
 *	DESCRIPTION:
 * 		create promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid - vlan id
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_DBUS_SUCCESS 
 *		
 *
 **********************************************************************************/
int npd_eth_port_create_promi_subif
(
	unsigned int eth_g_index,
	unsigned short vid
);


/**********************************************************************************
 *  npd_eth_port_del_promi_subif
 *
 *	DESCRIPTION:
 * 		del promi subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid				- vlan id
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
unsigned int npd_eth_port_del_promi_subif
(
	unsigned int eth_g_index,
	unsigned short vid
);

/**********************************************************************************
 *  npd_eth_port_del_promi_all_subif
 *
 *	DESCRIPTION:
 * 		del promi all subif
 *
 *	INPUT:
 *		eth_g_index - port index
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
unsigned int npd_eth_port_destroy_all_promi_subif
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_set_ve_flag
 *
 *	DESCRIPTION:
 * 		set port ve flag
 *
 *	INPUT:
 *		eth_g_index - port index
 *		flag -  port ve flag
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_eth_port_save_promi_cfg
(
	char** strShow,
	unsigned int* avalidLen
);
/**********************************************************************************
 *  npd_get_promis_subif_count
 *
 *	DESCRIPTION:
 * 		set port ve flag
 *
 *	INPUT:
 *            NONE
 *	OUTPUT:
 *		
 * 	RETURN:
 *		Sub if count
 *		
 *
 **********************************************************************************/
unsigned int npd_get_promis_subif_count
(
    void 
);

/**********************************************************************************
 *  npd_check_eth_port_status
 *
 *	DESCRIPTION:
 * 		check ethernet port link stauts
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FAIL - if some errors occur
 *		ETH_ATTR_LINKUP
 *		ETH_ATTR_LINKDOWN
 *
 *
 **********************************************************************************/
int npd_check_eth_port_status
(
	unsigned int eth_g_index
) ;

/**********************************************************************************
 *  npd_route_port_link_change
 *
 *	DESCRIPTION:
 * 		ethernet port link event process
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/
 int npd_route_port_link_change
(
	unsigned int	eth_g_index,
	struct eth_port_intf_s* intfPtr,
	enum ETH_PORT_NOTIFIER_ENT	event
);

/**********************************************************************************
 *  npd_promi_port_link_change
 *
 *	DESCRIPTION:
 * 		ethernet port link event process
 *
 *	INPUT:
 *		eth_g_index - ethernet port global index
 *		event - port link up/down event
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occur
 *		NPD_FAIL - if some errors occur
 *		
 *
 **********************************************************************************/
 int npd_promi_port_link_change
(
	unsigned int	eth_g_index,
	struct eth_port_promi_s* promiPtr,
	enum ETH_PORT_NOTIFIER_ENT	event
);

/**********************************************************************************
 *  npd_eth_port_register_notifier_hook
 *
 *	DESCRIPTION:
 * 		Register ethernet port event notifier callback function
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *
 **********************************************************************************/
int npd_eth_port_register_notifier_hook
(
	void
);

/**********************************************************************************
 *  npd_eth_port_counter_statistics
 *
 *	DESCRIPTION:
 * 		statistics packets info
 *
 *	INPUT:
 *		eth_g_index - port_index
 *		portPtr - pakets info
 *	
 *
 * RETURN:
 *		NPD_FAIL  
 *		NPD_SUCESS 
 *		
 *
 **********************************************************************************/
int npd_eth_port_counter_statistics
(
	unsigned int eth_g_index,
	struct npd_port_counter *portPtr,
	struct port_oct_s		*portOctPtr
);

/**********************************************************************************
 *  npd_check_port_promi_mode
 *
 *	DESCRIPTION:
 * 		check port promiscuous mode
 *
 *	INPUT:
 *		eth_g_index - port index 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_TRUE
 *			NPD_FALSE
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_check_port_promi_mode
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_port_type_deal
 *
 *	DESCRIPTION:
 * 		deal port mode
 *
 *	INPUT:
 *		eth-g_index - port index
 *		mode  		    - port  mode
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *				NPD_DBUS_SUCCESS		
 *				NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_port_type_deal
(
	unsigned int eth_g_index,
	unsigned int mode,
	int ifnameType
);

/**********************************************************************************
 *  npd_eth_port_get_vlan_all
 *
 *	DESCRIPTION:
 * 		This method gets all bridge vlans and dot1q vlans this port belongs to.
 *	bridge vlans stands for vlan the eth-port belongs to untagged, and 
 *  dot1q vlans for vlan the eth-port belongs to tagged.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		brgVid - bridge vlans' vid list
 *		dot1qVid - dot1q vlans' vid list
 *		brgVlanCnt - maximum supported # of bridge vlans
 *		dot1qVlanCnt - maximum supported # of dot1q vlans
 *	
 *	OUTPUT:
 *		brgVlanCnt - # of bridge vlans
 *		dot1qVlanCnt - # of dot1q vlans
 *
 * 	RETURN:
 *			NPD_OK - if no failure occurs
 *			NPD_FAIL - if get vlan list failed
 *
 **********************************************************************************/
int npd_eth_port_get_vlan_all
(
	unsigned int eth_g_index,
	unsigned short brgVid[],
	unsigned short dot1qVid[],
	unsigned int *brgVlanCnt,
	unsigned int *dot1qVlanCnt
);

/**********************************************************************************
 *  npd_eth_port_poll_add_member
 *
 *	DESCRIPTION:
 * 		Add port to participate link status polling.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port
 *		NPD_FAIL - no proper position found for the port
 *
 **********************************************************************************/
int npd_eth_port_poll_add_member
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_poll_add_member
 *
 *	DESCRIPTION:
 * 		delete port from poll ports array to stop link status polling.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		NPD_FAIL - port not found in the polling array.
 *
 **********************************************************************************/
int npd_eth_port_poll_del_member
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_status_polling_thread
 *
 *	DESCRIPTION:
 * 		eth-port status polling thread
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
void npd_eth_port_status_polling_thread
(
	void
);


/**********************************************************************************
 *  npd_eth_port_link_reset_thread
 *
 *	DESCRIPTION:
 * 		eth-port link reset thread(each time reset eth-port phy device)
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
void npd_eth_port_link_reset_thread
(
	void
);

/**********************************************************************************
 * npd_arp_snooping_get_num_byport
 *
 *  	show ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_num_byport
(
	unsigned int   eth_g_index
);


/**********************************************************************************
 * npd_arp_snooping_find_item_byport
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

/**********************************************************************************
 * npd_arp_snooping_get_valid_num_byport
 *
 *  	show valid ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		valid ARP items number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_byport
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 * npd_arp_snooping_get_valid_num_byport_for_nexthop
 *
 *  	show valid ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		valid ARP items number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_byport_for_nexthop
(
	unsigned int   eth_g_index
);

/**********************************************************************************
 * npd_arp_snooping_find_valid_item_byport
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_valid_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

/**********************************************************************************
 * npd_arp_snooping_get_valid_nexthop_item
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		arrayLen - ARP snooping item array size
 *	
 *	OUTPUT:
 *		item - ARP snooping item array
 *		refCntArray - ARP snooping item reference count array
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_get_valid_nexthop_item
(
	unsigned int eth_g_index,
	struct arp_snooping_item_s *item,
	unsigned int arrayLen,
	unsigned int *refCntArray
);

/*
* npd_static_arp_validate_by_port
*/
unsigned int npd_static_arp_validate_by_port
(
     unsigned int eth_g_index
);

/*
* npd_static_arp_invalidate_by_port
*/
unsigned int npd_static_arp_invalidate_by_port
(
     unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_eth_port_mask_link_interrupt_set
 *
 *	DESCRIPTION:
 * 		This method enable/disable(unmask/mask) eth-port link status change interrupt event.
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - unmask/mask eth-port link status interrupt successfully.
 *		other - set up failed.
 *
 **********************************************************************************/
unsigned int npd_eth_port_mask_link_interrupt_set
(
	unsigned int eth_g_index,
	unsigned int enable
) ;

/*******************************************************************************
 * npd_eth_port_rgmii_type_check
 *		connect to CPU RGMII interface 
 * INPUT:
 *		slot_no - the slot no. of the port we want to check (eg. 0~4 in PRODUCT_ID_AX7K)
 *		port_no - the port no. of the port we want to check (eg. 1~52 in PRODUCT_ID_AU3K)
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE - the port is connect to CPU RGMII interface
 *		FALSE - the port is NOT connect to CPU RGMII interface
 * NOTE:
 *
 *******************************************************************************/
unsigned int npd_eth_port_rgmii_type_check
(
	unsigned int slot_no,
	unsigned int port_no
);

/**********************************************************************************
 *  npd_set_cscd_port_promi_mode
 *
 *	DESCRIPTION:
 * 		set promiscuous port running mode
 *
 *	INPUT:
 *		port - witch port want to set 
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			INTERFACE_RETURN_CODE_SUCCESS
 *                INTERFCE_RETURN_CODE_ADVANCED_VLAN_NOT_EXISTS
 *			INTERFACE_RETURN_CODE_ERROR
 *
 **********************************************************************************/
int npd_set_cscd_port_promi_mode
(
	unsigned char port
);

/**********************************************************************************
 *  npd_set_cscd_port_ipaddr
 *
 *	DESCRIPTION:
 * 		set cscd port ip address.
 *
 *	INPUT:
 *	ipaddr - ip address want to add	
 *	port - witch port want to add
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
int npd_set_cscd_port_ipaddr
(
	unsigned int ipaddr,
	unsigned char port
);

extern unsigned int npd_arp_clear_by_port
(
	unsigned int   eth_g_index
);

extern unsigned int npd_fdb_number_port_set_check
(
    unsigned char slotNum,
    unsigned char portNum,
    unsigned int * number
);
	
extern int nam_set_ethport_type
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_TYPE_E portType
);
extern int nam_set_ethport_inband_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
extern int nam_set_ethport_inband_an_mode
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_INBAND_AN_MODE anMode
);
extern int nam_set_ethport_enable
(
	unsigned char	devNum,
	unsigned char	portNum,
	unsigned long	portAttr
);
extern int nam_set_ethport_force_linkup
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);
extern int nam_set_ethport_force_auto
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

extern int nam_set_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

extern int nam_get_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
extern int nam_set_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
);

extern int nam_get_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
extern int nam_set_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

extern int nam_get_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
);
extern int nam_set_ethport_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_DUPLEX_ENT dMode
);
extern int nam_set_ethport_speed
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_SPEED_ENT speed
);
extern int nam_set_ethport_flowCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
);
extern int nam_set_ethport_PeriodFCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long enable
);
extern int nam_set_ethport_backPres
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
);
extern int nam_set_ethport_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int mruSize
);
extern unsigned int nam_get_port_en_dis
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
) ;

extern unsigned int nam_get_port_link_state
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
);

extern unsigned int nam_get_port_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int *portAttr
);

extern unsigned int nam_get_port_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);

extern unsigned int nam_get_port_autoneg_state
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned long* portAttr
);

extern unsigned int nam_get_port_flowCtrl_state
(
	unsigned char devNum,
	unsigned char portNum, 
	unsigned long* portAttr
);

extern unsigned int nam_get_port_backPres_state
(
	unsigned char devNum,
	unsigned char portNum, 
	unsigned long* portAttr
);

extern unsigned int nam_get_port_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int  *portMru
);

extern int nam_set_ethport_force_linkdown
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

extern int nam_pvlan_cscd_port_config_spi
(
	unsigned char devNum1,
	unsigned char portNum1,
	unsigned char dev,
	unsigned char port
);


extern int nam_pvlan_port_config_spi
(
	unsigned int eth_g_index1,
	unsigned char dev,
	unsigned char port
);

extern int nam_port_phy_port_media_type
(	
	enum module_id_e	 moduleType,
	unsigned int slotno,
	unsigned int portno,
	enum eth_port_type_e 	*portMedia	
);

extern unsigned int nam_set_ethport_led_intf
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long enable
);

extern unsigned int nam_fdb_table_delete_entry_with_trunk
(
	unsigned short trunk_no
);

extern unsigned int nam_get_port_config_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);
extern unsigned int nam_get_port_config_flowcontrol
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
);

/*
*Set Phy Autoneg
*/
extern int nam_set_phy_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

unsigned int npd_eth_port_acl_bind_check
(
	unsigned int eth_g_index,
	unsigned int dir_info
);

unsigned int eth_port_acl_enable
(	
	unsigned int eth_g_index,
	unsigned int acl_enable,
	unsigned int dir_info
	
);

extern unsigned int nam_asic_port_pkt_statistic
(
	unsigned char devNum, 
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s			*portOctCount
);

extern unsigned int nam_asic_clear_port_pkt_stat
(
	unsigned char dev,
	unsigned char port
);
/* add from aw1.3 */
extern unsigned int nam_asic_clear_xg_port_pkt_stat
(
	unsigned char dev,
	unsigned char port
);

extern unsigned int nam_set_port_global_buffer_mode
(
	unsigned char enable
);

/*******************************************************************************
* nam_eth_port_phy_reinit
*
* DESCRIPTION:
*      Reinit phy smi interface 
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      other - on hardware error or parameters error 
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned int nam_eth_port_phy_reinit
(
	unsigned char dev,
	unsigned char port
);

unsigned int npd_port_bind_group_check
(
	unsigned int eth_g_index,
	unsigned int *groupNo,
	unsigned int dir_info
);

unsigned int npd_qos_port_bind_check
(
	unsigned int eth_g_index,
	unsigned int *ID
);

int npd_qos_traffic_shape_check
(
	unsigned int 				g_eth_index,
	QOS_TRAFFIC_SHAPE_PORT_STC  *shapeData
);

/**********************************************************************************
 *  cavim_do_intf
 *
 *	DESCRIPTION:
 * 		this routine create or delete cavim dev
 *
 *	INPUT:
 *		cmd -- create or delete
 *		param -- associated param
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
int cavim_do_intf
(
	unsigned int cmd,
	void* param
);

/*******************************************************************************
* nam_set_eth_port_trans_media
*
* DESCRIPTION:
*      Set media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern int nam_set_eth_port_trans_media
(
	unsigned char dev,
	unsigned char port,
	unsigned int  media
);

/*******************************************************************************
* nam_get_eth_port_trans_media
*
* DESCRIPTION:
*      Get media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern int nam_get_eth_port_trans_media
(
	unsigned char dev,
	unsigned char port,
	unsigned int  *media
);

extern int nam_read_eth_port_info
(
	unsigned int module_type, 
	unsigned char slotno, 
	unsigned char portno, 
	struct eth_port_s *ethport
) ;

/************************************************************
 *
 *   for XG-Port 
 *
 *************************************************************/
extern int nam_set_ethport_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char  value
);
extern int nam_set_port_buff_mode
(
	unsigned char devNum,
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
);
extern int nam_set_xg_port_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT ipgMode,
	CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT ipgBase
);
extern int nam_set_xsmi_ac_timing
(
	unsigned char devNum
);

extern int nam_set_xgport_led_on_external_phy
(
	unsigned char devNum,
	unsigned char portNum
);

/**********************************************************************************
 * nam_ethport_mask_link_interrupt_set
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		devNum - DXCH device number
 *		portNum - port number on the DXCH device
 *		enable - enable/disable flag(0 for disable interrupt and 1 for enable interrupt)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		status - return value from cpss driver layer.
 *		
 **********************************************************************************/
extern unsigned long nam_ethport_mask_link_interrupt_set
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long enable
);

/* port auto negotiation for 3052*/
extern unsigned int nam_set_bcm_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
);

/*****************************nbm apis********************************/
extern int nbm_set_eth_port_trans_media
(
	unsigned int port,
	unsigned int media
);
extern int nbm_get_eth_port_trans_media
(
	unsigned int port,
	unsigned int* media
);
extern int nbm_read_eth_port_info
(
	unsigned char 		portNum,
	struct eth_port_s 	*portInfo
);
extern unsigned int nbm_board_port_pkt_statistic
(
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount
);
extern unsigned int nbm_board_clear_port_pkt_stat
(
	unsigned char portNum
);
extern int nbm_set_ethport_enable
(
	unsigned char	portNum,
	unsigned long	portAttr
);
extern int nbm_set_ethport_enable
(
	unsigned char	portNum,
	unsigned long	portAttr
);
extern int nbm_set_ethport_mru
(
	unsigned char portNum, 
	unsigned int mruSize
);
extern int nbm_set_ethport_flowCtrl
(
	unsigned char portNum, 
	unsigned long state
);
extern int nbm_set_ethport_backPres
(
	unsigned char portNum, 
	unsigned long state
);
extern int nbm_set_ethport_force_linkup
(
	unsigned char portNum,
	unsigned long state
);
extern int nbm_set_ethport_force_linkdown
(
	unsigned char portNum,
	unsigned long state
);
extern int nbm_set_ethport_duplex_mode
( 
	unsigned char portNum, 
	unsigned int 	dMode
);
extern int nbm_set_ethport_speed
(
	unsigned char portNum, 
	PORT_SPEED_ENT speed
);
extern int nbm_set_autoneg
(
	unsigned char portNum, 
	unsigned long state
);
extern int nbm_set_ethport_duplex_autoneg
(
	unsigned char portNum,
	unsigned long state
);
extern int nbm_set_ethport_fc_autoneg
(
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
);
extern int nbm_set_ethport_speed_autoneg
(
	unsigned char portNum,
	unsigned long state
);

extern unsigned int nam_fdb_enable_cscd_port_auto_learn_set
(
	unsigned char devNum,
	unsigned char devPort,
	unsigned int state
);

extern unsigned int nam_fdb_enable_port_auto_learn_set
(
	unsigned int index,
	unsigned int state
);

/* 
 * call cpss function cpssDxChBrgVlanLearningStateSet and cpssDxChBrgVlanNASecurEnable
 * if both return 0,then return 0; else return NPD_DBUS_ERROR
 *
 */
extern unsigned int nam_learning_state_set_na_secur_enable
(
    unsigned char dev,
    unsigned short vid,
    unsigned int flag
);


/**********************************************************************************
 *  nam_set_promi_attr
 *
 *	DESCRIPTION:
 * 		set promiscuous port attribute.
 *
 *	INPUT:
 *		eth_g_index - index of port's mode 
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NPD_DBUS_SUCCESS
 *			NPD_DBUS_ERROR
 *
 **********************************************************************************/
extern int nam_set_promi_attr_on_pvid
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned short vid,
	unsigned int flag
);

extern int nam_set_promi_attr_on_all_vlan
(
	unsigned int product_id,
	unsigned int eth_g_index,
	unsigned char dev,
	unsigned int flag
);

/* 
 * call cpss function cpssDxChBrgVlanEgressFilteringEnable and cpssDxChBrgPortEgrFltUregMcastEnable
 * if both return 0,then return 0; else return 1
 *
 */

extern int nam_egress_filter_enable
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned long enable
);

/**********************************************************************************
 *  npd_eth_port_reinit
 *
 *	DESCRIPTION:
 * 		eth-port reinit when hot plugin
 *
 *	INPUT:
 *		chassis_slot_index - eth-port slot index
 *		local_port_index - eth-port port index on slot
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - find position to add port.
 *		
 *
 **********************************************************************************/
int npd_eth_port_reinit
(
	unsigned int chassis_slot_index,
	unsigned int local_port_index
);

/************************************************************************************
 *		NPD dbus operation
 *
 ************************************************************************************/
DBusMessage * npd_dbus_ethports_interface_show_ethport_attr
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_ethports_interface_show_ethport_stat
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_ethports_interface_clear_ethport_stat
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
) ;

DBusMessage * npd_dbus_ethports_interface_config_ethport_one
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
) ;

DBusMessage* npd_dbus_ethports_interface_config_ports_attr
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_ethports_interface_show_ethport_list
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
) ;

DBusMessage * npd_dbus_config_port_mode
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage *npd_dbus_ethports_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);


DBusMessage* npd_dbus_eth_link_state_config
(
    DBusConnection *conn,
    DBusMessage *msg, 
    void *user_data
);
unsigned int npd_eth_port_get_slotno_portno_by_eth_g_index
(
    unsigned int eth_g_idex,
    unsigned char * slot_no,
    unsigned char * port_no
);

unsigned int npd_eth_port_count_of_bmp(unsigned int * portBmp);

/**********************************************************************************
 *  npd_eth_port_destroy_all_qinq_subif
 *
 *	DESCRIPTION:
 * 		del promi all subif
 *
 *	INPUT:
 *		eth_g_index - port index
 *		vid  - subif first tag
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
unsigned int npd_eth_port_destroy_all_qinq_subif
(
	unsigned int eth_g_index,
	unsigned short vid
);
unsigned int npd_eth_port_save_intf_qinq_type_cfg
(
	char** strShow,
	unsigned int* avalidLen
);

extern int npd_eth_port_interface_l3_flag_get
(
	unsigned int 	eth_g_index,
	unsigned int   	*enable
);

#endif
