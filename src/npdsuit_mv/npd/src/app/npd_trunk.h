#ifndef __NPD_TRUNK_H__
#define __NPD_TRUNK_H__

#include <dbus/dbus.h>
#include <pthread.h>
#include "npd_vlan.h"
#include <cvm/autelan_product.h>   /* for product_info */


#define CHECK_DEV_NO_ISLEGAL(dev_no) 			(0 == dev_no || 1 == dev_no)
#define CHECK_VIRPORT_NO_ISLEGAL(virport_no)	(virport_no <=29) 
#define TRUNK_MEMBER_NUM_MAX		0x8
#define TRUNK_CONFIG_SUCCESS	0x0
#define	TRUNK_CONFIG_FAIL		0xff

/*************/
#define TRUNK_PORT_EXISTS_GTDB	0xf		/*port already exists in trunkDB*/
#define TRUNK_PORT_MBRS_FULL	(TRUNK_PORT_EXISTS_GTDB+1)
#define NPD_TRUNK_ERR_NONE			NPD_SUCCESS
#define NPD_TRUNK_ERR_GENERAL		(NPD_TRUNK_ERR_NONE + 20)		/*general failure*/
#define NPD_TRUNK_BADPARAM			(NPD_TRUNK_ERR_NONE + 22)  		/*bad parameters*/
#define NPD_TRUNK_EXISTS			(NPD_TRUNK_ERR_NONE + 23)		/*vlan have been created already*/
#define NPD_TRUNK_NOTEXISTS			(NPD_TRUNK_ERR_NONE + 24)		/*vlan does not exists*/
#define NPD_TRUNK_ERR_HW			(NPD_TRUNK_ERR_NONE + 26) 		/*vlan error when operation on HW*/
#define NPD_TRUNK_PORT_EXISTS		(NPD_TRUNK_ERR_NONE + 27)		/*port already exists in vlan*/
#define NPD_TRUNK_PORT_NOTEXISTS	(NPD_TRUNK_ERR_NONE + 28)		/* port is not a member of vlan*/
#define NPD_TRUNK_PORT_MEMBERSHIP_CONFLICT	(NPD_TRUNK_ERR_NONE + 29)		/*port can NOT be Untag member of different vlans.*/ 
#define NPD_TRUNK_NAME_CONFLICT		(NPD_TRUNK_ERR_NONE + 30)	
#define NPD_TRUNK_MEMBERSHIP_CONFICT	(NPD_TRUNK_ERR_NONE + 31)	
#define NPD_TRUNK_ALLOW_ERR				(NPD_TRUNK_ERR_NONE +32)
#define NPD_TRUNK_REFUSE_ERR				(NPD_TRUNK_ERR_NONE +33)
#define NPD_TRUNK_MEMBER_ADD_ERR	(NPD_TRUNK_ERR_NONE + 34)
#define NPD_TRUNK_MEMBER_DEL_ERR	(NPD_TRUNK_ERR_NONE + 35)
#define NPD_TRUNK_GET_ALLOWVLAN_ERR	(NPD_TRUNK_ERR_NONE + 36)
#define NPD_TRUNK_NO_MEMBER			(NPD_TRUNK_ERR_NONE + 37)
#define NPD_TRUNK_SET_TRUNKID_ERR	(NPD_TRUNK_ERR_NONE + 38)
#define NPD_TRUNK_DEL_MASTER_PORT	(NPD_TRUNK_ERR_NONE + 39)		/*master port NOT allowd to delete.*/
#define NPD_TRUNK_PORT_ENABLE		(NPD_TRUNK_ERR_NONE + 40)		/*port enalbe in trunk*/
#define NPD_TRUNK_PORT_NOTENABLE	(NPD_TRUNK_ERR_NONE + 41)		/* port disable in trunk*/
#define NPD_TRUNK_ALLOW_VLAN		(NPD_TRUNK_ERR_NONE + 42)
#define NPD_TRUNK_NOTALLOW_VLAN		(NPD_TRUNK_ERR_NONE + 43)
#define NPD_TRUNK_LOAD_BANLC_CONFLIT (NPD_TRUNK_ERR_NONE + 44)		/*trunk load balance mode same to original*/
#define NPD_TRUNK_VLAN_TAGMODE_ERR (NPD_TRUNK_ERR_NONE + 45)
#define NPD_TRUNK_PORT_LINK_DOWN		    (NPD_TRUNK_ERR_NONE + 46)
#define NPD_TRUNK_UNSUPPORT         (NPD_TRUNK_ERR_NONE + 47)
#define NPD_TRUNK_PORT_CONFIG_DIFFER (NPD_TRUNK_ERR_NONE + 48) /*port has a differ configuration with master port*/
#define NPD_TRUNK_PORT_L3_INTF		(NPD_TRUNK_ERR_NONE + 61)



#define	DEFAULT_TRUNK_ID			0x0								/*NOT member of any trunk*/

#define MAX_STATIC_TRUNK_ID		118		/*<1-118> is for static trunk id,119 is internal use, and <120-127> is for dynamic */
#define NPD_MAX_TRUNK_ID		127
#define NPD_TRUNK_RUNNING_CFG_MEM	1024*1024

#if 1
typedef struct {
	unsigned short 	trunkId;
	char*			trunkName;
	unsigned char 	masterFlag;
	unsigned int 	mSlotNo;
	unsigned int	mPortno;
	PORT_MEMBER_BMP	mbrBmp_sp;
	PORT_MEMBER_BMP	disMbrBmp_sp;
	unsigned int	loadBalanc;
	unsigned int    tLinkStatus;
}actTrunkList;
#endif

typedef struct {
	unsigned char portNum;
	unsigned char devNum;
}trunk_member_stc;
typedef struct{
    unsigned int   ports[2];
}port_bmp_stc;


typedef enum
{
    CSCD_TRUNK_LINK_TYPE_TRUNK_E,
    CSCD_TRUNK_LINK_TYPE_PORT_E
} cscd_trunk_link_type_mode;


typedef struct
{
    unsigned long   linkNum;
    cscd_trunk_link_type_mode  linkType;
} cscd_trunk_link_type;

typedef enum
{
    CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
    CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E,
    CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E
} cscd_trunk_link_hash_mode;



typedef struct {
	unsigned short vlanId;
	char*		vlanName;
	unsigned char tagMode;
}trunkAllowVlan;

enum trunk_status_e {
	TRUNK_STATE_DOWN_E = 0,
	TRUNK_STATE_UP_E,
	TRUNK_STATE_MAX
};

typedef enum {
	LOAD_BANLC_SRC_DEST_MAC = 0 ,
	LOAD_BANLC_SOURCE_DEV_PORT ,
	LOAD_BANLC_SRC_DEST_IP,
	LOAD_BANLC_TCP_UDP_RC_DEST_PORT ,
	LOAD_BANLC_MAC_IP,
	LOAD_BANLC_MAC_L4,
	LOAD_BANLC_IP_L4 ,
	LOAD_BANLC_MAC_IP_L4,
	LOAD_BANLC_MAX
}trkLoadBanlcMode;
extern pthread_mutex_t arpHashMutex;
extern unsigned int nam_asic_trunk_entry_active
(
	unsigned short trunkId
);

/*extern unsigned int nam_asic_trunk_ports_add
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum
);*/

extern unsigned int nam_asic_trunk_ports_add
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum,
	unsigned char enDis
);


extern unsigned int nam_asic_trunk_ports_del
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum
);

extern unsigned int nam_asic_trunk_delete
(
	unsigned short trunkId
);

extern unsigned int nam_asic_trunk_load_balanc_set
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned int  lbMode
);
extern unsigned int nam_asic_trunk_get_port_member
(
	unsigned short	trunkId,
	unsigned int* 	memberCont,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
extern unsigned int nam_asic_trunk_get_port_member_bmp
(
	unsigned short	trunkId,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
#if 1
extern unsigned int (*npd_get_devport_by_global_index)
(
		unsigned int eth_g_index,
		unsigned char *devNum,
		unsigned char *portNum
);
#else
extern unsigned int npd_get_devport_by_global_index
(
		unsigned int eth_g_index,
		unsigned char *devNum,
		unsigned char *portNum
);
#endif
extern unsigned int npd_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
);
extern int npd_port_vlan_free
(
	unsigned int eth_g_index
);

extern unsigned int nam_asic_trunk_port_endis
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short trunkId,
	unsigned char enDis
);

extern struct trunk_s  ** g_trunks;

void npd_init_trunks
(
	void
);
unsigned int npd_check_trunk_exist
(
	unsigned short trunkId
);

struct trunk_s *npd_find_trunk
(
	unsigned short trunkId
);
struct trunk_s* npd_find_trunk_by_name
(
	char *name
);
int npd_check_trunk_status
(
	unsigned short trunkId
);
struct trunk_s* npd_create_trunk
(
	unsigned short trunkId
);
struct trunk_s* npd_create_trunk_by_name
(
	char *name
);
unsigned int npd_delete_trunk
(
	unsigned short trunkId
);
unsigned int npd_trunk_fillin_global
(
	unsigned short trunkId,
	struct trunk_s	*trunkNode
);

unsigned int npd_trunk_flushout_global
(
	unsigned short trunkId
);

unsigned int npd_trunk_check_port_membership
(
	unsigned short trunkId,
	unsigned int   eth_index
);
unsigned int npd_trunk_check_vlan_allowship
(
	unsigned short trunkId,
	unsigned short vid
);

unsigned int npd_trunk_member_port_index_get_all
(
	unsigned short	trunkId,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
);
unsigned int npd_trunk_endis_member_port_index_get
(
	unsigned short	trunkId,
	unsigned int	eth_g_index[],
	unsigned char 	enDis,
	unsigned int	*mbr_count
);
unsigned int npd_eth_port_configuration_check
(
	unsigned int eth_g_idx,
	unsigned short trunkId
);

unsigned int npd_trunk_allow_vlan_id_get
(
	unsigned short	trunkId,
	unsigned short	vid[],
	unsigned int	*v_count
);
unsigned int npd_trunk_allow_vlan_tagmode_get
(
	unsigned short	trunkId,
	unsigned char	tag[],
	unsigned int	*v_count
);

unsigned int npd_trunk_master_port_config
(
	unsigned short trunkId,
	unsigned int   eth_index
);

unsigned int npd_trunk_port_set_endis
(
	unsigned short trunkId,
	unsigned int   eth_index,
	unsigned char  en_dis
);
unsigned int npd_trunk_load_balanc_set
(
	unsigned short trunkId,
	unsigned int lbMode
);
unsigned int npd_trunk_check_port_endis
(
	unsigned short trunkId,
	unsigned int   eth_index,
	unsigned char  *en_dis
);
unsigned int npd_trunk_master_port_get
(
	unsigned short trunkId,
	unsigned int   *eth_index
);
unsigned int npd_trunk_port_list_addto
(
	unsigned short trunkId,
	unsigned int   eth_g_index
);

unsigned int npd_trunk_port_list_delfrom
(
	unsigned short trunkId,
	unsigned int   eth_g_index
);
unsigned int npd_trunk_vlan_list_addto
(
	unsigned short trunkId,
	unsigned short   vid,
	unsigned char  tagMode
);
unsigned int npd_trunk_vlan_list_delfrom
(
	unsigned short trunkId,
	unsigned short vid,
	unsigned char tagMode
);
int npd_modify_trunk_status_by_portdown
(
	unsigned short trunkId
);
int npd_trunk_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_trunk_s *portTrunk,
	enum ETH_PORT_NOTIFIER_ENT event
);
unsigned int npd_trunk_port_member_add
(
	unsigned short trunkId,
	unsigned int   eth_index
);

unsigned int npd_trunk_port_member_del
(
	unsigned short trunkId,
	unsigned int   eth_index
);
unsigned int npd_trunk_hold_member_bmp
(
	unsigned short trunkId,
	unsigned int lastMbrBmp0,
	unsigned int lastMbrBmp1
);
unsigned int npd_trunk_activate
(
	unsigned short trunkId,
	unsigned char* name
);

unsigned int npd_trunk_port_add
(
	unsigned short trunkId, 
	unsigned int eth_index
);

unsigned int npd_trunk_port_del
(
	unsigned short	trunkId,
	unsigned int	eth_index
);

unsigned int npd_trunk_master_port_set
(
	unsigned short trunkId, 
	unsigned int eth_index
);

int npd_trunk_port_endis
(
	unsigned short trunkId,
	unsigned int eth_g_idx,
	unsigned char enDis
);

int npd_trunk_port_en_dis
(
	unsigned short trunkId,
	unsigned int eth_g_idx,
	unsigned char enDis
);

unsigned int npd_trunk_destroy_node
(
	unsigned short trunkId
);

int npd_trunk_allow_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
);

int npd_trunk_refuse_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
);

int npd_trunk_get_mbr_bmp_via_sw
(
	unsigned short trunkId,
	unsigned int *mbrBmp_sp,
	unsigned int *disMbrBmp_sp
);

int npd_trunk_get_mbr_bmp_via_sw_v1
(
	unsigned short trunkId,
	PORT_MEMBER_BMP *mbrBmp_sp,
	PORT_MEMBER_BMP *disMbrBmp_sp
);

unsigned int npd_get_port_link_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
);

unsigned int npd_trunk_status_get
(
	IN unsigned int trunkId,
	OUT unsigned int *status
);

int npd_check_eth_port_status
(
	unsigned int eth_g_index
);

unsigned int npd_eth_port_set_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short trunkId
);

unsigned int npd_eth_port_clear_ptrunkid
(
	unsigned int   eth_g_index
);

int npd_eth_port_get_vlan_all
(
	unsigned int eth_g_index,
	unsigned short brgVid[],
	unsigned short dot1qVid[],
	unsigned int *brgVlanCnt,
	unsigned int *dot1qVlanCnt
);

unsigned int npd_vlan_check_trunk_membership
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  isTagged
);

unsigned int npd_arp_snooping_get_valid_num_bytrunk
(
	unsigned short trunkId
);

int npd_arp_snooping_find_valid_item_bytrunk
(
	unsigned int   trunkId,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

int	npd_trunk_list_delete(unsigned char trunkId);

/*****************************************************************************************
 *		NPD dbus operation
 *
 *****************************************************************************************/
/*create trunk entity with trunkId,on Both Hw &Sw.*/
DBusMessage * npd_dbus_trunk_create_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*enter trunk configure node*/
DBusMessage * npd_dbus_trunk_config_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*enter trunk configure node*/
DBusMessage * npd_dbus_trunk_config_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_add_delete_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_delete_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_master_port_set
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_trunk_port_endis
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_trunk_delete_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_one_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_by_name_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);


/*original*/
DBusMessage *npd_dbus_trunk_show_trunklist_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*original*/
DBusMessage *npd_dbus_trunk_show_trunklist_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

#endif
