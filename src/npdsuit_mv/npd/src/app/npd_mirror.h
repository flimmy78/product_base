#ifndef __NPD_MIRROR_H__
#define __NPD_MIRROR_H__

#define MAX_MIRROR_PROFILE		8
#define DEFAULT_MIRROR_PROFILE 	0

/* default ingress mirror destination port index*/
#define MIRROR_DEST_INPORT_DEFAULT	(~0UL)
/* default egress mirror destination port index*/
#define MIRROR_DEST_EGPORT_DEFAULT	MIRROR_DEST_INPORT_DEFAULT

/* invalid ingress/egress mirror source port index*/
#define MIRROR_SRC_PORT_INVALID	(~0UL)

#define MIRROR_SAVE_CFG_MEM  (8 * 1024)

extern AclRuleList  **g_acl_rule;

/**********************************************************************************
 *  npd_mirror_init
 *
 *	DESCRIPTION:
 * 		this routine initialize all mirror mode
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
void npd_mirror_init(void);

/**********************************************************************************
 *  npd_mirror_profile_create
 *
 *	DESCRIPTION:
 * 		This method create mirror profile with global mirror profile id
 *
 *	INPUT:
 *		profileId - global mirror profile id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_PROFILE_ID_OUTOFRANGE - mirror profile id is out of range
 *		MIRROR_RETURN_CODE_PROFILE_NODE_CREATED - found mirror profile has been created before
 *		MIRROR_RETURN_CODE_MALLOC_FAIL - memory allocation fail when create new mirror profile
 *		MIRROR_RETURN_CODE_SUCCESS - create mirror profile successfully
 *
 **********************************************************************************/
unsigned int npd_mirror_profile_create
(
	unsigned int profileId
);

/**********************************************************************************
 *  npd_mirror_destination_port_save
 *
 *	DESCRIPTION:
 * 		this routine save destination port node
 *
 *	INPUT:
 *		showStr -- buf
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST
 *		MIRROR_RETURN_CODE_ERROR
 *		MIRROR_RETURN_CODE_SUCCESS
 *		
 *
 **********************************************************************************/
void npd_mirror_profile_config_save
(
	char* showStr
);

/**********************************************************************************
 *  npd_mirror_destination_node_port_get
 *
 *	DESCRIPTION:
 * 		this routine check destination node members
 *
 *	INPUT:
 *		profile			-- mirror profile
 *		direct			--  ingress or egress	
 *
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		MIRROR_RETURN_CODE_DESTINATION_NODE_NOTEXIST
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_destination_node_port_get
(
	unsigned int profile,
	unsigned int direct,
	unsigned int *eth_g_index
);


/**********************************************************************************
 *  npd_mirror_src_all_port_destroy
 *
 *	DESCRIPTION:
 * 		this routine destroy all src port node
 *
 *	INPUT:
 *		dest_eth_g_index -- destination port index
 *		
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_all_port_destroy
(
	unsigned int dest_eth_g_index
);


/**********************************************************************************
 *  npd_mirror_src_port_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save all src port configuration
 *
 *	INPUT:
 *		 srcPortList -- Port list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
void npd_mirror_src_port_cfg_save
(
	mirror_src_port_list_s* srcPortList,
	char** currentPos,
	unsigned int *totalLen,
	unsigned char *entered
);

/**********************************************************************************
 *  npd_mirror_src_all_vlan_destroy
 *
 *	DESCRIPTION:
 * 		this routine destroy all src vlan node
 *
 *	INPUT:
 *		dest_eth_g_index -- destination port index
 *		
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_all_vlan_destroy
(
	unsigned int dest_eth_g_index
);

/**********************************************************************************
 *  npd_mirror_src_vlans_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save all src vlan configuration
 *
 *	INPUT:
 *		srcVlanList -- vlan list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
void npd_mirror_src_vlan_cfg_save
(
	mirror_src_vlan_list_s* srcVlanList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
);

/**********************************************************************************
 *  npd_mirror_src_all_fdb_destroy
 *
 *	DESCRIPTION:
 * 		this routine destroy all src fdb node
 *
 *	INPUT:
 *		dest_eth_g_index -- destination port index
 *	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *		
 *
 **********************************************************************************/
int npd_mirror_src_all_fdb_destroy
(
	unsigned int dest_eth_g_index
);

/**********************************************************************************
 *  npd_mirror_src_fdb_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save src fdb configuration node
 *
 *	INPUT:
 *		srcFdbList -- fdb list head
 *		currentPos -- first no used char addr
 *		totalLen      -- used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *
 **********************************************************************************/
void npd_mirror_src_fdb_cfg_save
(
	mirror_src_fdb_list_s* srcFdbList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
);

/**********************************************************************************
 *  npd_mirror_src_policy_cfg_save
 *
 *	DESCRIPTION:
 * 		this routine save src policy configuration node
 *
 *	INPUT:
 *		srcPolicyList - policy list head
 *		currentPos - first no used char addr
 *		totalLen      - used buf len
 *		entered  - need to enter mirror node or not( 1 - no need re-enter, 0 - nee enter)
 *
 *	OUTPUT:
 *		entered - does mirror node entered or not
 *
 * 	RETURN:
 *		NPD_FALSE
 *		NPD_TRUE
 *
 **********************************************************************************/
void npd_mirror_src_policy_cfg_save
(
	mirror_src_policy_list_s* srcPolicyList,
	char** currentPos,
	unsigned int* totalLen,
	unsigned char *entered
);

unsigned int npd_mirror_acl_action_check
(
	unsigned int ruleIndex
);

unsigned int npd_mirror_nam_dest_port_set_for_bcm
(
	unsigned int profile,
	unsigned char devNum,
	unsigned char virPortId,
	unsigned int direct
);

unsigned int npd_mirror_nam_analyer_port_del_for_bcm
(
    unsigned int profile,
    MIRROR_DIRECTION_TYPE direct,
    unsigned int dest_eth_g_index
);

unsigned int npd_mirror_nam_mirror_port_set_for_bcm
(
	unsigned int profile,
	MIRROR_DIRECTION_TYPE direct,
	unsigned int src_eth_g_index
	
);

unsigned int npd_mirror_nam_mirror_port_del_for_bcm
(
	unsigned int profile,
	MIRROR_DIRECTION_TYPE direct,
	unsigned int src_eth_g_index
	
);

extern unsigned int nam_acl_mirror_action_update
(
	unsigned int ruleIndex,
	unsigned int enable,
	int unit
);
extern unsigned int search_acl_group_rule_index_exist
(	
	unsigned long index,
	unsigned int  *num,
	unsigned int *direction
);

extern unsigned int nam_mirror_analyzer_port_set
(
	unsigned int profile,
	unsigned char analyzerDev,
	unsigned char analyzerPort,
	unsigned int    direct
	
);

extern unsigned int nam_mirror_fdb_entry_set
(
    unsigned short vlanid,
    unsigned int  eth_g_index,
    ETHERADDR * macAddr,
    unsigned int flag
);

extern unsigned int nam_mirror_port_set
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned int direct,
	unsigned int profile
);

extern unsigned int nam_mirror_analyzer_port_get
(
    unsigned int profile,
    unsigned char * analyzerrxDev,
    unsigned char * analyzerrxPort,
    unsigned char * analyzertxDev,
    unsigned char * analyzertxPort
);
extern unsigned int nam_mirror_vlan_set
(
	unsigned short vid,
	unsigned int enable
);
extern unsigned int npd_fdb_static_mirror_entry_del
(
    unsigned char * mac,
    unsigned short vid
);
extern unsigned int npd_fdb_check_static_entry_exist
(
    unsigned char * mac,
    unsigned short vid,
    unsigned char * slot_no,
    unsigned char * port_no
);
int npd_mirror_src_acl_check
(
	unsigned int profile,
	unsigned int  ruleIndex
);
int npd_mirror_src_acl_create
(
	unsigned int profile,
	unsigned int ruleIndex
);
int npd_mirror_src_acl_remove
(
	unsigned int profile,
	unsigned int ruleIndex
);

int npd_mirror_delete_source_port_ingress_egress
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int src_eth_g_index,
    unsigned int profile
);

int npd_mirror_delete_source_port_bidirection
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int src_eth_g_index
);

DBusMessage * npd_dbus_mirror_config(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_create_dest_port(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_del_dest_port(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_create_source_acl(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_delete_source_acl(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_create_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_del_source_port(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_create_source_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_del_source_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_set(DBusConnection *conn, DBusMessage *msg, void *user_data);
DBusMessage * npd_dbus_mirror_fdb_mac_vlanid_port_cancel(DBusConnection *conn, DBusMessage *msg, void *user_data);

int npd_mirror_src_port_get
(
	unsigned int profile,
	unsigned int direct,
	unsigned int* src_eth_g_index,
	unsigned int* src_count
);


#endif
