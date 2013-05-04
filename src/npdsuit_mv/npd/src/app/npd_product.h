#ifndef __NPD_PRODUCT_H__
#define __NPD_PRODUCT_H__

#include <dbus/dbus.h>

extern struct product_s productinfo;

extern struct product_type_data_s*(*product_type_data)(unsigned int);
struct product_type_data_s *product_type_data_old(unsigned int product_id);
extern unsigned int arp_smac_check;

#define PRODUCT_ID (productinfo.product_id)
#if 0
#define PRODUCT_IS_BOX ((CHASSIS_SLOT_COUNT) == 1)
#define PRODUCT_IS_CHASSIS ((CHASSIS_SLOT_COUNT) > 1)
#else
#define PRODUCT_IS_BOX (0xB00000 == (0xF00000 & PRODUCT_ID))
#define PRODUCT_IS_CHASSIS (0xC00000 == (0xF00000 & PRODUCT_ID))
#endif
#define PRODUCT_MAC_ADDRESS (productinfo.sysinfo.basemac)
#define PRODUCT_SYSTEM_NAME (productinfo.sysinfo.name)

#define CHASSIS_SLOT_COUNT ((product_type_data(productinfo.product_id))->chassis_slot_count)
#define CHASSIS_SUBCARD_COUNT ((product_type_data(productinfo.product_id))->chassis_subcard_count)
#define CHASSIS_SLOT_START_NO ((product_type_data(productinfo.product_id))->chassis_slot_start_no)
#define CHASSIS_VLAN_RANGE_MIN		1		/*added by wujh*/
#define CHASSIS_VLAN_RANGE_MAX		4096
#define CHASSIS_VIDX_RANGE_MAX		4095
#define CHASSIS_TRUNK_RANGE_MAX		127
#define FDB_AGING_MIN		60
#define FDB_AGING_MAX		600
#define NPD_ERR_SYSTEM_MAC  3
typedef struct
{
    unsigned int gtHostInPkts;
    unsigned int gtHostOutPkts;
    unsigned int gtHostOutBroadcastPkts;
    unsigned int gtHostOutMulticastPkts;
} BRIDGE_HOST_CNTR_STC;
typedef struct {
	unsigned char arEther[6];
}NETHERADDR;

/*
	NOTE: slot_no is the lable printed in the front panel (might start with 1),
			slot_index is the index of the array chassis_slots, always start with 0
*/
#define CHASSIS_SLOT_INDEX2NO(slot_index) ((CHASSIS_SLOT_START_NO) + (slot_index))
#define CHASSIS_SLOT_NO2INDEX(slot_no) ((slot_no) - (CHASSIS_SLOT_START_NO))

#define CHASSIS_SLOTNO_ISLEGAL(slot_no) (((slot_no) >= (CHASSIS_SLOT_START_NO)) && ((slot_no) <= ((CHASSIS_SLOT_START_NO) + (CHASSIS_SLOT_COUNT) - 1)))
/*#define CHASSIS_SLOTNO_ISLEGAL(slot_no) (((slot_no) >= (CHASSIS_SLOT_START_NO)) && ((slot_no) <= (CHASSIS_SLOT_COUNT)))*/
/*added by wujh*/
#define CHASSIS_VLANID_ISLEGAL(vlan_id) (((vlan_id) >= (CHASSIS_VLAN_RANGE_MIN)) && ((vlan_id) <= (CHASSIS_VLAN_RANGE_MAX)))
#define CHASSIS_AGINGTIME_ISLEGAL(time) (((time) >= (FDB_AGING_MIN)) && ((time) <= (FDB_AGING_MAX)))

#define LOCAL_MODULE_ID (productinfo.local_module_id)
#define LOCAL_CHASSIS_SLOT_NO (productinfo.local_chassis_slot_no)
#define LOCAL_CHASSIS_SLOT_INDEX (CHASSIS_SLOT_NO2INDEX(LOCAL_CHASSIS_SLOT_NO))
#define LOCAL_MODULE_HW_VERSION	(productinfo.local_module_hw_version)
#define NPD_SYS_GLOBAL_RUNNING_CFG_MEM (5*1024)

typedef enum{
	FUNCTION_ACL = 0x0,
	FUNCTION_QOS,
	FUNCTION_TUNNEL,
	FUNCTION_PVLAN,
	FUNCTION_IGMP,
	FUNCTION_MLD,
	FUNCTION_DLDP,
	FUNCTION_STP,
	FUNCTION_VLAN,
	FUNCTION_INTERFACE,
	FUNCTION_MIRROR,
	FUNCTION_FDB,
	FUNCTION_TRUNK,
	FUNCTION_ARP,
	FUNCTION_ESLOT,
	FUNCTION_ASIC,
	FUNCTION_OAM,
	FUNCTION_ROUTE
	
}FUNCTION_TYPE;


typedef struct _PRODUCT_FUNCTION_{
	unsigned char	function_acl;		
	unsigned char	function_qos;
	unsigned char	function_tunnel;
	unsigned char	function_pvlan;
	unsigned char	function_igmp;
	unsigned char	function_mld;
	unsigned char	function_dldp;
	unsigned char	function_stp;
	unsigned char	function_vlan;
	unsigned char	function_interface;
	unsigned char	function_mirror;
	unsigned char	function_fdb;
	unsigned char	function_trunk;
	unsigned char	function_arp;
	unsigned char	function_eslot;
	unsigned char	function_asic;
	unsigned char	function_oam;
	unsigned char	function_route;
		
}NPD_PRODUCT_FUNCTION;

typedef struct
 {
	unsigned int outUcFrames;
	unsigned int outMcFrames;
	unsigned int outBcFrames;
	unsigned int brgEgrFilterDisc;
	unsigned int txqFilterDisc;
	unsigned int outCtrlFrames;
 }NPD_PORT_EGRESS_CNTR_STC;
int (*npd_init_productinfo)(void);
int npd_init_productinfo_old(void);

enum product_id_e npd_query_product_id(void);
struct product_sysinfo_s *npd_query_sysinfo(void);

/**********************************************************************************
 * npd_product_support_function
 * 
 * whether the product support the function
 *
 *	INPUT:
 *		MULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *	NOTATION:
 *		
 *		
 **********************************************************************************/

int npd_product_support_function
(
	struct product_s *product_info
);

/**********************************************************************************
 * get_product_function_runbmp
 * 
 * get product's running bmp
 *
 *	INPUT:
 *		MULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *	NOTATION:
 *		
 *		
 **********************************************************************************/

NPD_PRODUCT_FUNCTION *npd_get_product_function_runbmp
(
	unsigned int product_id	
);

/**********************************************************************************
 * get_product_function_funbmp
 * 
 * get product's running bmp
 *
 *	INPUT:
 *		MULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *	NOTATION:
 *		
 *		
 **********************************************************************************/

NPD_PRODUCT_FUNCTION *npd_get_product_function_funbmp
(
	unsigned int product_id	
);


/**********************************************************************************
 * npd_system_verify_basemac
 * 
 * Verify if the given mac address is the same as system base mac address
 *
 *	INPUT:
 *		macAddr - mac address will compare to system base mac address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		-1 - if parameters illegall.
 *		-2 - input parameter to long.
 *		0 - if the given mac address is not the same as system base mac address.
 *		1 - if the given mac address is the same as system base mac address.
 *
 *	NOTATION:
 *		input parameter macAddr is a ASCII code formatted MAC address, such as 
 *		"001122334455" stands for mac address 00:11:22:33:44:55 or 00-11-22-33-44-55
 *		
 **********************************************************************************/
int npd_system_verify_basemac
(
	char *macAddr
);

/**********************************************************************************
 * npd_system_get_basemac
 * 
 * 	Get system mac address in Hex format
 *
 *	INPUT:
 *		size - macAddr buffer size
 *	
 *	OUTPUT:
 *		macAddr - mac address will be returned back
 *
 * 	RETURN:
 *		-1 - if mac address buffer size too small.
 *		-2 - illegal character found.
 *		0 - if no error occur
 *
 *	NOTATION:
 *		system mac address is a ASCII code formatted MAC address, such as 
 *		"001122334455" stands for mac address 00:11:22:33:44:55 or 00-11-22-33-44-55
 *		
 **********************************************************************************/
int npd_system_get_basemac
(
    unsigned char *macAddr,
    unsigned int  size
);

/**********************************************************************************
 * npd_init_system_stateinfo
 * 
 * Initialize system accessories(such as fan,power,temperature etc.) state info
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		
 **********************************************************************************/
int npd_init_system_stateinfo
(
	void
);

/**********************************************************************************
 * npd_init_oam
 * 
 *  Initialize OAM settings such as TELNET/SSH/HTTP/HTTPS/SNMP traffic special treatment
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 *	NOTATION:
 *		
 **********************************************************************************/
void npd_init_oam
(
	void
);

/**********************************************************************************
 * npd_oam_traffic_setup
 *
 * 		Initialize system OAM traffic such as TELNET/SSH/HTTP/HTTPS/SNMP classfication to
 * 	provide high quality of service with high priority, low drop rate.
 *
 *	INPUT:
 *		type - OAM control traffic type
 *		oamInfo - detailed OAM control info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		-1 - if error occurred.
 *		
 **********************************************************************************/
int npd_oam_traffic_setup
(
	enum OAM_TRAFFIC_TYPE_ENT type,
	struct oam_ctrl_s *oamInfo
);

/*******************************************************************************
* npd_check_system_startup_state
*
* DESCRIPTION:
*      Check if system has started up correctly.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_check_system_startup_state
(
	void
);

/*******************************************************************************
* npd_init_packet_socket
*
* DESCRIPTION:
*      This function initialize all packet sockets used for sending packets from/to other module.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_packet_socket
( 
	void
);

/*******************************************************************************
* npd_init_tell_whoami
*
* DESCRIPTION:
*      This function is used by each thread to tell its name and pid to NPD_ALL_THREAD_PID_PATH
*
* INPUTS:
*	  tName - thread name.
*	  lastTeller - is this the last teller or not, pid file should be closed if true.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);
/*******************************************************************************
* npd_init_tell_stage_end
*
* DESCRIPTION:
*      This function is used by NPD to tell rc script loader about initialization process done.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
void npd_init_tell_stage_end
(
	void
);

/*******************************************************************************
* npd_init_tell_system_state_end
*
* DESCRIPTION:
*      This function is used by NPD to tell system is running OK.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	    0 ---OK.  
*      -1 ---fialed.
*
* COMMENTS:
*       
*
*******************************************************************************/
int npd_init_tell_system_state_end
(
	void
);


/****************************************************
 *	Description:
 *		check port's target is SPI or not
 *		SPI path with uplink   (1,26) on AX7 and
 *							(0,24) on AX5612i and
 *							(0,0) on AX5612
 *
 *	Input:
 *		unsigned char -device num of eth-port
 *		unsigned char -port num of eth-port
 *
 *	Output:
 *		NULL
 *
 *	ReturnCode:
 *		NPD_SUCCESS	- success
 *		NPD_FAIL		- fail
 *		
 ****************************************************/
unsigned int npd_product_check_spi_port
(
	unsigned char devNum,
	unsigned char portNum
);

/**********************************************************************************
 * npd_product_hardware_watchdog_function_check
 *
 * 	Check whether the product support hardware watch dog or not.
 *	Currently, all chassis and box product with CPLD supported has hardware watchdog funcationality.
 *  That means asic chip with XCAT series has no hardware watchdog functionality.
 *
 *	INPUT:
 *		productId - product id
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		0 - no hardware watchdog on this product
 *		1 - hardware watchdog is deployed on this product
 *
 **********************************************************************************/
int npd_product_hardware_watchdog_function_check
(
	void
);

extern int nbm_init_system_state_info
(
	struct system_state_s *stateInfo
);

/*******************************************************************************
* appDemoBoardSigHandler
*
* DESCRIPTION:
*       Perform board signal process to catch up external signals
*
* INPUTS:
*	  signo - signal number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       
*
*******************************************************************************/
extern unsigned long appDemoBoardSigHandler
(
	int signo
);
/*******************************************************************************
* appDemoBoardAfterSysUpIntEnable
*
* DESCRIPTION:
*       Enable DxCh devices' interrupt hierarchy to report interrupts 
*	   via enabling root mask register bits of the interrupt scanning-tree.
*
* INPUTS:
*	  None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after system started up.
*
*******************************************************************************/
extern unsigned long appDemoBoardAfterSysUpIntEnable
(
	void
);

/**********************************************************************************
 * nam_asic_set_cpufc_port_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
int nam_oam_traffic_setup
(
	enum OAM_TRAFFIC_TYPE_ENT oamType,
	struct oam_ctrl_s *oamInfo
);

extern int nam_asic_set_cfc
(
	CPU_FC_PACKET_TYPE packettype,
	unsigned int time,
	unsigned int rate
);

extern unsigned long nam_packet_rx_adapter_init
(
	void
);

extern unsigned long nam_packet_tx_adapter_init
(
	void
);

/**********************************************************************************
 * nam_set_system_arpsmaccheck_enable
 *
 *	set if enable to excute smac check or not for arp
 *
 *	INPUT:
 *		isenable - binary value to indicate enable or disable
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *
 **********************************************************************************/

extern unsigned int nam_arp_smac_check_enable
(
	unsigned int isenable
);

/************************************************************************************
 *		NPD dbus operation
 *
 ************************************************************************************/
/*

cmd used to test this method
dbus-send --system --dest=aw.npd --type=method_call --print-reply=literal /aw/npd aw.npd.show_ver

 arg lists for method NPD_DBUS_INTERFACE_METHOD_VER
  in arg list:
	NONE
  out arg list:  // in the order as they are appended in the dbus message.
	uint32 product_id	// unsigned int of product type. uniquely identify a product, defined in npd_sysdef.h ,
	uint32 sw_version	   // bitmap definition in npd_sysdef.h
	string product_name  // backplane info for chassis product, board info for box product
	string product_base_mac_addr  // 12 char of mac address  with no : or - spliter.
	string product_serial_number  // 32 bytes string
	string sw_name		// software description string
 

 */

DBusMessage * npd_dbus_interface_show_ver
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_interface_show_hwconf
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*****************************************************************************************
 * DESCRIPTION:
 *	arg lists for method  NPD_DBUS_SYSTEM_CORE_TEMPERATURE
 *	in the order as they are appended in the dbus message.
 *
 * INPUT:
 *	uint16 -high_value		  //input system core high temperature value
 *	uint16 -high_value		  //input system core low  temperature value
 * OUTPUT:
 *	uint32 - ret	   // judge from the ret value whethere operation successful.
 *	
 *
 *****************************************************************************************/

DBusMessage * npd_dbus_config_system_temperature
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_config_system_alarm_state
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);


DBusMessage * show_system_environment_state
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_system_shut_down_enable
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_system_arp_smac_check_enable
(
    DBusConnection *conn,
    DBusMessage *msg,
    void *user_data
);

DBusMessage * npd_dbus_sys_global_cfg_save
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_set_arp_aging_time
(	
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
); 

DBusMessage * npd_system_arp_smac_check_enable
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * nbm_dbus_interface_syslog_no_debug
( 	
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * nam_dbus_interface_syslog_no_debug
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage* npd_dbus_asic_syslog_debug
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage* npd_dbus_asic_syslog_no_debug
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_asic_bcast_rate_limiter
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpufc_config
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_cpufc_set_queue_quota
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpufc_get_queue_quota
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpufc_set_queue_shaper
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpufc_set_port_shaper
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpu_show_sdma_info
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpu_show_sdma_mib
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_cpu_clear_sdma_mib
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_vlan_trunk_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_vlan_trunk_member_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_vlanlist_trunk_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_vlanlist_trunk_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_igmp_snoop_info_get
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_igmp_snoop_info_get_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage *npd_dbus_vlan_show_vlan_routeport_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage *npd_dbus_vlan_show_vlan_routeport_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage *npd_dbus_vlan_exchange_ifindex_igmp_to_npd
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_get_slotport_by_ethportindex
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_config_vlan_mtu
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_config_vlan_egress_filter
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_config_vlan_filter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_vlan_igmp_snp_show_running_config
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_trunk_load_banlc_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_allow_refuse_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_show_vlanlist
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
); 

DBusMessage * npd_dbus_trunk_show_running_config
(	
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_show_running_config
(	
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_static_fdb_with_vlan
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_static_fdb_with_port
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_fdb_with_trunk
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_config_static_fdb_trunk_item
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_config_static_fdb_trunk_with_name
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
); 

DBusMessage * npd_dbus_fdb_show_dynamic_fdb
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_blacklist_fdb
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage *npd_dbus_fdb_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage *npd_dbus_fdb_config_vlan_port_number
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_config
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_create_dest_port
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_del_dest_port
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_create_source_acl
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_delete_source_acl
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_create_source_port
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_del_source_port
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_create_source_vlan
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_check_vlan_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_pvlan_show_running_cfg
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_show_running_cfg
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_set
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_cancel
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_mirror_show
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_delete
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_show_running_cfg
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_mirror_del_source_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_show_qos_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_config_qos_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_show_remap_table
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_delete_qos_profile
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_delete_policy_map
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_delete_policer
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_qos_counter_show_running_config
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_qos_acl_apend
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_show_qos_acl_apend
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_time_range_acl
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * show_rtdrv_all
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * show_rtdrv_entry
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * config_ucrpf_enable
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * show_ucrpf_enable
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * show_route_status
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_config_buffer_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_config_buffer_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_ethports_ipg
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_ethports_interface_config_ports_media
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_ethport_clear_arp
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_ethport_show_arp
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_show_arp
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_ethport_show_nexthop
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_show_nexthop
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_get_broad_id
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_get_port_speed
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_get_port_duplex_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_stp_set_stpid_for_vlan
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_show_buffer_mode
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_get_slot_port_by_portindex
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_arp_aging_destmac_broadcast
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
); 
DBusMessage * npd_dbus_promis_port_add2_vlan_intf
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_sys_host_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_sys_host_smac_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);
DBusMessage * npd_dbus_sys_host_dmac_ingress_counter
(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);

DBusMessage *npd_dbus_sys_show_ingress_counter


(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);
DBusMessage *npd_dbus_sys_show_egress_counter


(
	DBusConnection *conn,
	DBusMessage *msg,
	void *user_data
);
DBusMessage *  npd_dbus_vlan_egress_drop_statistic
(
	  
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_sys_egress_drop_statistic
(
 
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data

);
DBusMessage * npd_dbus_sys_global_counter_drop_statistic
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_sys_global_config_counter_drop_statistic
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
unsigned int npd_product_adv_routing_cmd_support(void);

#endif
