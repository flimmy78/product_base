#ifndef __NPD_FDB_H__
#define __NPD_FDB_H__

#include "npd_list.h"

#define  CPSS_INTERFACE_MAX 4
#define FDB_TABLE_LEN 16384
#define FDB_TABLE_LEN_32K 32768
#define CPU_PORT    0
#define FDB_CONFIG_FAIL		1
#define FDB_CONFIG_SUCCESS	0
#define FDB_HASH_INDEX_SIZE   (4096) 

#define AX_STATIC_FDB_ENTRY_SIZE (1024)

#define AX_FDB_PORT_NUMBER_MAX (27)
/*show running cfg mem*/
#define NPD_FDB_SHOWRUN_CFG_SIZE	(256*1024*2+1024*1024*2)  /* for maximum 1024 static FDB entry*/

#define NPD_FDB_HASH_TABLE_SIZE (2048) /* FDB hash table buckets 2048*/


 /* Flags for L2 learn limit. */
#define NPD_L2_LEARN_LIMIT_SYSTEM           0x00000001 /* Limit is system wide. */
#define NPD_L2_LEARN_LIMIT_VLAN             0x00000002 /* Limit is on per VLAN
														   basis. */
#define NPD_L2_LEARN_LIMIT_PORT             0x00000004 /* Limit is on per port
														   basis. */
#define NPD_L2_LEARN_LIMIT_TRUNK            0x00000008 /* Limit is on per trunk
														   basis. */
#define NPD_L2_LEARN_LIMIT_ACTION_DROP      0x00000010 /* Drop if over limit. */
#define NPD_L2_LEARN_LIMIT_ACTION_CPU       0x00000020 /* Send to CPU if over
														   limit. */
#define NPD_L2_NOT_SUPPORT                  -16
 enum npd_fdb_dcli{
	NPD_FDB_OP_CREATE_ITEM =0,   /*change for fdb table add or delete*/
	NPD_FDB_OP_CREATE_BLKLIST,  /*command match dorp*/
	NPD_FDB_OP_DELETE_BLKLIST,   /*command match is normal ---forwarding*/
	NPD_FDB_OP_CONFIG_SYS          /* check is dorp or forwarding current  fdb node*/
};
 typedef struct npd_learn_limit_s {
	 unsigned int  flags;		 /* BCM_L2_LEARN_LIMIT_xxx actions and qualifiers. */
	 unsigned short vlan;	 /* VLAN identifier. */
	 int port;	 /* Port number. */
	 int trunk;  /* Trunk identifier. */
	 int limit; 		 /* Maximum number of learned entries, -1 for unlimited. */
 } npd_learn_limit_t;


 enum{
	NPD_FDB_ERR_NONE =0,
	NPD_FDB_ERR_DBUS,
	NPD_FDB_ERR_GENERAL,
	NPD_FDB_ERR_BADPARA,
	NPD_FDB_ERR_OCCUR_HW,
	NPD_FDB_ERR_ITEM_ISMIRROR,
	NPD_FDB_ERR_NODE_EXIST,
	NPD_FDB_ERR_NODE_NOT_EXIST,
	NPD_FDB_ERR_PORT_NOTIN_VLAN, /* 3->4*/
	NPD_FDB_ERR_VLAN_NONEXIST, /*1.5->5*/
	NPD_FDB_ERR_SYSTEM_MAC,
	NPD_FDB_ERR_VLAN_NO_PORT,
	NPD_FDB_ERR_HW_NOT_SUPPORT,
	NPD_FDB_ERR_MAX
};

 enum{
 FDB_STATIC_NOMIRROR = 0,
 FDB_STATIC_ISMIRROR
 };
 
typedef struct {
	 unsigned short 	 vlanid;
	 unsigned char		 ether_mac[6];
	 unsigned int		 inter_type;
	 unsigned int		 value;
	 unsigned int        type_flag;
 }NPD_FDB;
typedef struct {
	 unsigned short 	 vlanid;
	 unsigned char		 ether_mac[6];
	 unsigned int		 inter_type;
	 unsigned int		 value;
	 unsigned int        type_flag;
	 unsigned int        dev;
	 unsigned int 		 asic_num;
	 unsigned int        slot_id;
}NPD_FDB_DBG;

typedef union {
	struct {
		unsigned char slot;
		unsigned char port;
	} s_port;
	struct {
		unsigned char flag;
	} b_list;
} key;

struct fdb_entry_node_s {
	unsigned short      vlanid;
	unsigned char       mac[6];
	unsigned int        hash_index;
	unsigned char       slot;
	unsigned char 		port;
	unsigned short      trunkid;
	unsigned int        isMirror;	
};

struct fdb_blacklist_node_s{
    unsigned short vlanid;
	unsigned char  mac[6];
	unsigned int   hash_index;
	unsigned char  dmac; /* 1 means drop,0 means forward*/
	unsigned char  smac;
};

struct fdb_number_limit_s{
    unsigned short vlanId;
	unsigned char slot_no;
	unsigned char local_port_no;
	unsigned int number;
};

enum FDB_BOOL{
	FDB_BOOL_FALSE = 0,
	FDB_BOOL_TRUE 
} ;

typedef struct
{
    unsigned char      vlan_type;
	unsigned char      port_type;
	unsigned char      vlan_port_type;
}FDB_LIMIT_TYPE;

typedef enum
{
    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E = 0,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E
} CPSS_MAC_ENTRY_EXT_TYPE_ENT;

typedef struct
{
    unsigned char       arEther[6];
}ETHERADDR;

typedef struct
{
    CPSS_MAC_ENTRY_EXT_TYPE_ENT         entryType;
    union {
       struct{
           ETHERADDR     	macAddr;
           unsigned short	vlanId;
       }macVlan;

       struct{
           unsigned char	sip[4];
           unsigned char	dip[4];
           unsigned short	vlanId;
       }ipMcast;

    } key;

}CPSS_MAC_ENTRY_EXT_KEY_STC;

typedef enum {
    CPSS_INTERFACE_PORT_E = 0,
    CPSS_INTERFACE_TRUNK_E,
    CPSS_INTERFACE_VIDX_E,
    CPSS_INTERFACE_VID_E
}CPSS_INTERFACE_TYPE_ENT;

typedef unsigned short  GT_TRUNK_ID;

typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        unsigned char   devNum;
        unsigned char   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    unsigned short       vidx;
    unsigned short       vlanId;
#ifdef CPU_ARM_XCAT
		unsigned char		 devNum;
		unsigned short		 fabricVidx;
		unsigned int		 index;
#endif
}CPSS_INTERFACE_INFO_STC;

typedef enum
{
    CPSS_MAC_TABLE_FRWRD_E = 0,
    CPSS_MAC_TABLE_DROP_E,
    CPSS_MAC_TABLE_INTERV_E,
    CPSS_MAC_TABLE_CNTL_E,
    CPSS_MAC_TABLE_MIRROR_TO_CPU_E,
    CPSS_MAC_TABLE_SOFT_DROP_E
} CPSS_MAC_TABLE_CMD_ENT;

#ifdef DRV_LIB_CPSS_3_4
typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       GT_BOOL                      isStatic;
       CPSS_MAC_TABLE_CMD_ENT       daCommand;
       CPSS_MAC_TABLE_CMD_ENT       saCommand;
       GT_BOOL                      daRoute;
       GT_BOOL                      mirrorToRxAnalyzerPortEn;
       GT_U32                       sourceID;
       GT_U32                       userDefined;
       GT_U32                       daQosIndex;
       GT_U32                       saQosIndex;
       GT_U32                       daSecurityLevel;
       GT_U32                       saSecurityLevel;
       GT_BOOL                      appSpecificCpuCode;
       GT_BOOL                      age;
       GT_BOOL                      spUnknown;
} CPSS_MAC_ENTRY_EXT_STC;
#else

typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       unsigned int					isStatic;
       CPSS_MAC_TABLE_CMD_ENT       daCommand;
       CPSS_MAC_TABLE_CMD_ENT       saCommand;
       unsigned int 				daRoute;
       unsigned int					mirrorToRxAnalyzerPortEn;
       unsigned int                 sourceID;
       unsigned int                 userDefined;
       unsigned int                 daQosIndex;
       unsigned int                 saQosIndex;
       unsigned int                 daSecurityLevel;
       unsigned int                 saSecurityLevel;
       unsigned int					appSpecificCpuCode;
#ifdef CPU_ARM_XCAT
	   unsigned int					age;
#endif
} CPSS_MAC_ENTRY_EXT_STC;
#endif

/*fdb item struct for hash index*/
struct fdb_item_s{
	unsigned char mac[MAC_ADDRESS_LEN];
	unsigned short vid;
	unsigned char dev;
	unsigned char port;
	unsigned short trunkId;
	boolean       isStatic;
};

typedef enum{
	MAC_VLAN_LOOKUP_E = 0,
    MAC_ONLY_LOOKUP_E
}NPD_FDB_LOOKUP;

enum NPD_FDB_HASH_ACTION {
	FDB_HASH_ADD_ITEM = 0,
	FDB_HASH_DEL_ITEM,
	FDB_HASH_UPDATE_ITEM,
	FDB_HASH_OCCUPY_ITEM,
	FDB_HASH_RESET_ITEM,
	FDB_HASH_ACION_MAX
};

/**********************************************************************************
 * npd_fdb_static_init
 *
 * static FDB database initialization, maximum static fdb entry count is 1K
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
 **********************************************************************************/
void npd_fdb_static_init();

void npd_fdb_number_port_init();

void npd_fdb_number_init();

/**********************************************************************************
 * npd_fdb_static_entry_add
 *
 * add static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		eth_index - global eth port index
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_entry_add
(
	unsigned char *mac,
	unsigned short vid,
	unsigned char slot,
	unsigned char port,
	unsigned int  isMirror
);

/**********************************************************************************
 * npd_fdb_static_entry_del
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		eth_index - global eth port index
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_entry_del
(
	unsigned char *mac,
	unsigned short vid
);

unsigned int npd_fdb_static_trunk_add
(
	unsigned char *mac,
	unsigned short vid,
    unsigned short trunkId
);

unsigned int npd_fdb_static_trunk_del
(
	char *mac,
	unsigned short vid
);

unsigned int npd_fdb_limit_port_count_add
(
    unsigned int eth_g_index
);

unsigned int npd_fdb_limit_port_count_get
(
    unsigned int eth_g_index
);

unsigned int npd_fdb_limit_vlan_count_add
(
    unsigned short vlanid
);

unsigned int npd_fdb_limit_vlan_count_get
(
    unsigned short vlanId
);

unsigned int npd_fdb_limit_vlan_port_count_add
(
    unsigned short vlanId,
    unsigned int eth_g_index
);

unsigned int npd_fdb_limit_vlan_port_count_get
(
    unsigned short vlanId,
    unsigned int eth_g_index
);
unsigned int npd_fdb_number_port_set
(
    unsigned char slotNum,
    unsigned char portNum,
    unsigned int number
);

unsigned int npd_fdb_number_port_set_check
(
     unsigned char slotNum,
     unsigned char portNum,
     unsigned int * number
);
unsigned int npd_fdb_number_vlan_set
(
    unsigned short vlanId,
    unsigned int number
);
unsigned int npd_fdb_number_vlan_set_check 
(
    unsigned short vlanId,
    unsigned int *number
);	
unsigned int npd_fdb_number_vlan_port_set
(
     unsigned short vlanid,
     unsigned char slotno,
     unsigned char portno,
     unsigned int fdblimit
);
unsigned int npd_fdb_number_vlan_port_set_check
(
     unsigned short vlanid,
     unsigned char slotno,
     unsigned char portno,
     unsigned int * fdblimit
);
unsigned int npd_fdb_reset_fdbCount_by_port
(
     unsigned int slot_no,
     unsigned int port_no
);
unsigned int npd_fdb_reset_fdbCount_by_vlan
(
     unsigned short vlanId
);
unsigned int npd_fdb_show_number_with_port
(
     unsigned char slotNum,
     unsigned char portNum,
     unsigned int * number
);	
	
unsigned int  npd_fdb_limit_aging
(
   CPSS_MAC_ENTRY_EXT_STC	   *MacEntry,
   unsigned int 			   *fdbCount
);

unsigned int  npd_na_msg_read
(
	CPSS_MAC_ENTRY_EXT_STC *macEntry,
	FDB_LIMIT_TYPE		   *fdb_limit_type
);


unsigned int npd_fdb_set_port_learn_status
(
    unsigned char slotNum,
	unsigned char portNum,
	unsigned int  status
) ;
unsigned int npd_fdb_na_msg_per_port_set
(
	unsigned char slotNum,
    unsigned char portNum,
	unsigned int  status

);
unsigned int npd_fdb_na_msg_per_vlan_set
(
     unsigned short vlanId,
     unsigned int  status,
     unsigned int  flag
);
unsigned int npd_get_port_index_from_eth_index
(
    unsigned int eth_index
);

unsigned int npd_get_slot_index_from_eth_index
(
    unsigned int eth_index
);
unsigned int npd_fdb_static_blacklist_entry_del_with_vlan
(
     unsigned short vlanId
);

unsigned int  npd_get_slot_no_from_index(unsigned int slot_index);
unsigned int  npd_get_port_no_from_index(unsigned int slot_index,unsigned int port_index);

void npd_fdb_number_limit_func();

/**********************************************************************************
 * npd_fdb_add_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		add a FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when add FDB item to table FDB
***********************************************************************************/
unsigned int npd_fdb_add_dldp_vlan_system_mac
(
	unsigned short vlanId
);

/**********************************************************************************
 * npd_fdb_del_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		del the  FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when del FDB item to table FDB
***********************************************************************************/
unsigned int npd_fdb_del_dldp_vlan_system_mac
(
	unsigned short vlanId
);

/* external variable reference */
extern unsigned int g_cpu_port_index;
extern unsigned int g_spi_port_index;

/* external function decleration*/
extern int nam_fdb_static_system_source_mac_add
(
	unsigned short,
	unsigned int
);
extern int nam_fdb_static_system_source_mac_del
(
	unsigned short,
	unsigned int
);

extern int nam_static_fdb_entry_mac_set_for_system
(
	unsigned short vlanId,
	unsigned int flag
) ;

extern int nam_fdb_table_agingtime_set(unsigned int timeout);

extern int nam_fdb_table_agingtime_get(unsigned int*	timeout);

extern int nam_static_fdb_entry_mac_vlan_port_set(unsigned char macAddr[],unsigned short vlanId,unsigned  int globle_index) ;

extern int nam_no_static_fdb_entry_mac_vlan_set(unsigned char macAddr[],unsigned short vlanId) ;

extern int nam_fdb_entry_mac_vlan_drop(unsigned char macAddr[],unsigned short vlanId,unsigned int flag,unsigned int* hash_index) ;

extern int nam_fdb_entry_mac_vlan_no_drop(unsigned char macAddr[],unsigned short vlanId,unsigned int flag) ;
extern int nam_fdb_table_delete_entry_with_port(unsigned int index);
extern int nam_fdb_table_delete_entry_with_vlan(unsigned int vlanid);
extern unsigned int npd_fdb_static_blacklist_entry_add(unsigned char * mac,unsigned short vid,unsigned char flag,unsigned int * blacklist_node_index);
extern unsigned int nam_show_fdb_one(NPD_FDB *fdb,unsigned char macAddr [ 6 ],unsigned short vlanId);

extern unsigned int nam_show_fdb_mac(NPD_FDB *fdb,unsigned char macAddr [ 6 ],unsigned int size);

extern unsigned int nam_show_fdb_port (NPD_FDB *fdb,unsigned int size,unsigned int port);
extern unsigned int nam_show_fdb_vlan (NPD_FDB *fdb,unsigned int size,unsigned short vlan);
extern unsigned int nam_show_fdb_count();

extern unsigned int nam_show_fdb (NPD_FDB *fdb,unsigned int size);
extern unsigned int nam_show_static_fdb (NPD_FDB *fdb,unsigned int size);
extern int nam_static_fdb_entry_mac_set_for_l3
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
) ;
extern unsigned int nam_fdb_na_msg_vlan_set
(
    unsigned short vlanId,
    unsigned int status
);
extern unsigned int nam_static_fdb_entry_mac_vlan_trunk_set
(
    unsigned char macAddr [ ],
    unsigned short vlanId,
    unsigned short trunkId
);

extern unsigned int  nam_show_dynamic_fdb
(
    NPD_FDB * fdb,
    unsigned int size
);
extern unsigned int nam_fdb_static_table_delete_entry_with_vlan
(
    unsigned short vlanId
);

extern unsigned int nam_fdb_static_table_delete_entry_with_port
(
    unsigned int eth_g_index
);

extern unsigned int nam_fdb_table_delete_entry_with_trunk
(
    unsigned short trunk_no
);

extern unsigned int nam_fdb_table_delete_entry_with_vlan_port
(
    unsigned int vlanid,
    unsigned int index
);

extern unsigned int nam_fdb_port_learn_status_set(unsigned char devNum,unsigned char port,unsigned int status);
extern unsigned int nam_fdb_na_msg_per_port_set(unsigned char devNum,unsigned char port,unsigned int status);

extern unsigned int npd_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn

 );

/***********************************************************************************
 *		NPD dbus operation
 *
 ***********************************************************************************/
 
DBusMessage * npd_dbus_fdb_config_agingtime
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_no_config_agingtime
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_blacklist
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_blacklist_with_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_create_blacklist
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_create_blacklist_with_vlanname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_agingtime
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_config_static_fdb_item
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_config_static_fdb_with_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage *  npd_dbus_fdb_delete_static_fdb
(
	DBusConnection *conn, 
	DBusMessage *msg,
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_static_fdb_with_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_mac
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/* for show debug fdb */
DBusMessage * npd_dbus_fdb_show_fdb_debug
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_count
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_fdb_with_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_delete_fdb_with_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_static_fdb
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_port
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_vlan
(
DBusConnection *conn, 
DBusMessage *msg, 
void *user_data
);

DBusMessage * npd_dbus_fdb_show_fdb_vlan_with_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_fdb_config_system_mac
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);



DBusMessage * npd_dbus_fdb_config_port_number
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
); 

DBusMessage * npd_dbus_fdb_config_vlan_number
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
); 

DBusMessage * npd_dbus_fdb_show_port_number
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
); 
DBusMessage *npd_dbus_fdb_show_limit_item
(
   DBusConnection *conn, 
   DBusMessage *msg,
   void *user_data
);


#endif
