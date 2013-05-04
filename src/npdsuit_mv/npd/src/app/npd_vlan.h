#ifndef __NPD_VLAN_H__
#define __NPD_VLAN_H__

#include <dbus/dbus.h>
#include "npd/nam/npd_amapi.h"
#define NPD_VLAN_NODE_EXITST	0x0
#define NPD_VLAN_NODE_NOT_EXIST	0xFF
#define NPD_VLAN_NODE_ERROR		0xFF
#define NPD_VLAN_NODE_UPDATE_SUCCESS	0x0
#define NPD_VLAN_NODE_UPDATE_FAIL		0xFF

#define CHECK_DEV_NO_ISLEGAL(dev_no) 			(0 == dev_no || 1 == dev_no)
#define CHECK_VIRPORT_NO_ISLEGAL(virport_no)	(virport_no <=29)
#define ETH_PORT_NUM_MAX		0x6
#define EXTENDED_SLOT_NUM_MAX	0x4
#define TRUNK_MEMBER_NUM_MAX	0x8

#define	ALIAS_NAME_SIZE			0x15
extern struct vlan_s	**g_vlans;
/*show running cfg mem*/
#define NPD_VLAN_RUNNING_CFG_MEM (1024*1024*2)
#define NPD_VLAN_IGMP_SNP_RUNNING_CFG_MEM (1024*1024)

#define NPD_VLAN_ERR_NONE			NPD_SUCCESS
#define NPD_VLAN_ERR_GENERAL		(NPD_VLAN_ERR_NONE + 10)		/*general failure*/
#define NPD_VLAN_BADPARAM			(NPD_VLAN_ERR_NONE + 12)  	/*bad parameters*/
#define NPD_VLAN_EXISTS			(NPD_VLAN_ERR_NONE + 13)		/*vlan have been created already*/
#define NPD_VLAN_NAME_CONFLICT	(NPD_VLAN_ERR_NONE + 14)
#define NPD_VLAN_NOTEXISTS		(NPD_VLAN_ERR_NONE + 15)		/*vlan does not exists*/
#define NPD_VLAN_ERR_HW			(NPD_VLAN_ERR_NONE + 16) 	/*vlan error when operation on HW*/
#define NPD_VLAN_PORT_EXISTS		(NPD_VLAN_ERR_NONE + 17)		/*port already exists in vlan*/
#define NPD_VLAN_PORT_NOTEXISTS	(NPD_VLAN_ERR_NONE + 18)		/* port is not a member of vlan*/
#define NPD_VLAN_PORT_MEMBERSHIP_CONFLICT	(NPD_VLAN_ERR_NONE + 19)		/*port can NOT be Untag member of different vlans. */
#define NPD_VLAN_L3_INTF			(NPD_VLAN_ERR_NONE + 20)		/*vlan is L3 interface*/
#define NPD_VLAN_PORT_TAG_CONFLICT	(NPD_VLAN_ERR_NONE +21)
#define NPD_TRUNK_MEMBER_NONE		(NPD_VLAN_ERR_NONE + 22)    /*trunk has no member when add to vlan*/
#define NPD_VLAN_PORT_PROMISCUOUS_MODE_ADD2_L3INTF  (NPD_VLAN_ERR_NONE + 23) /*the promiscuous mode port add to l3 interface*/
#define NPD_VLAN_PORT_DEL_PROMISCUOUS_PORT_DELETE (NPD_VLAN_ERR_NONE + 24) /*del promiscuous port but default is l3 intf*/
#define NPD_VLAN_SUBINTF_EXISTS     (NPD_VLAN_ERR_NONE + 26)    /* sub intf exists*/
#define NPD_VLAN_CONTAINS_ROUTE_PORT (NPD_VLAN_ERR_NONE + 27)   /* vlan contains route mod port */
#define NPD_VLAN_PORT_PROMIS_PORT_CANNOT_ADD2_VLAN (NPD_VLAN_ERR_NONE + 28)  /*promis try to add to other vlan(not vlan 1)*/
#define NPD_VLAN_PORT_SUBINTF_EXISTS     (NPD_VLAN_ERR_NONE + 29)  /* port sub interface exists*/
#define NPD_VLAN_PORT_L3_INTF		(NPD_VLAN_ERR_NONE + 61)
#define NPD_VLAN_TRUNK_EXISTS		(NPD_VLAN_ERR_NONE + 63)		
#define NPD_VLAN_TRUNK_NOTEXISTS	(NPD_VLAN_ERR_NONE + 64)
#define NPD_VLAN_TRUNK_CONFLICT		(NPD_VLAN_ERR_NONE + 65)
#define NPD_VLAN_PORT_TRUNK_MBR		(NPD_VLAN_ERR_NONE + 66)  /* port belong to trunk ,it can NOT add to vlan as port*/
#define NPD_VLAN_TRUNK_MBRSHIP_CONFLICT	(NPD_VLAN_ERR_NONE + 67)
#define NPD_VLAN_NOT_SUPPORT_IGMP_SNP (NPD_VLAN_ERR_NONE + 70)
#define NPD_VLAN_IGMP_ROUTE_PORTEXIST (NPD_VLAN_ERR_NONE + 71)
#define NPD_VLAN_IGMP_ROUTE_PORTNOTEXIST (NPD_VLAN_ERR_NONE + 72)
#define NPD_VLAN_ARP_STATIC_CONFLICT (NPD_VLAN_ERR_NONE + 73)
#define NPD_VLAN_ERR_MAX			(NPD_VLAN_ERR_NONE + 255)

#define DEFAULT_VLAN_ID			0x1
#define NPD_PORT_L3INTF_VLAN_ID	0xfff 
#define NPD_ADV_ROUTING_DEFAULT_VID (NPD_PORT_L3INTF_VLAN_ID - 1)
#if 0
#define NPD_VLAN_OBC0_VLAN_ID  (NPD_PORT_L3INTF_VLAN_ID - 2)
#else
/* for bug of vlan-4093 on AX7605i(bug:AXSSZFI-281), change it to 0xBADBAD, need not it. zhangdi 2011-07-25 */
#define NPD_VLAN_OBC0_VLAN_ID  0xBADBAD
#endif
#define NPD_MAX_VLAN_ID 4095

#define ACL_ON_PORT_DISABLE  21  /*sl*/
#define ACL_GROUP_VLAN_BINDED  14  /*sl*/
#define ACL_DIRECTION_INGRESS  0	/*sl*/
#define ACL_DIRECTION_EGRESS   1	/*sl*/


extern unsigned int promis_port_add2_intf;

typedef struct {
	unsigned int trunkMbr[4];
}TRUNK_MEMBER_BMP;

typedef struct {
	unsigned int portMbr[2];
}PORT_MEMBER_BMP;
	
typedef struct {
	unsigned short	vlanId;
	char*			vlanName;
    unsigned int    ifIndex;
	PORT_MEMBER_BMP untagMbrBmp;
	PORT_MEMBER_BMP	tagMbrBmp;
	TRUNK_MEMBER_BMP untagTrkBmp;
	TRUNK_MEMBER_BMP tagTrkBmp;
	unsigned int vlanStat;
}vlanList;


typedef struct {
	unsigned short vidx;
	unsigned int	groupIp;
	unsigned short	groupIpv6[SIZE_OF_IPV6_ADDR];
	PORT_MEMBER_BMP   groupMbrBmp;
}groupList;

typedef struct {
	unsigned int slotno;
	unsigned int localportNo;
	unsigned short pvid;
}portPvid;

struct vlan_ports{
	unsigned int count;
	unsigned int ports[128];
};

enum vlan_status_e {
	VLAN_STATE_DOWN_E = 0,
	VLAN_STATE_UP_E,
	VLAN_STATE_MAX
};

enum vlan_port_op_e {
	VLAN_PORT_ADD_E = 0,
	VLAN_PORT_DEL_E,
	VLAN_PORT_OP_NONE
};
struct DRV_VLAN_PORT_BMP_S {
    unsigned int   ports[2];
};

/*Test code*/
/* Initialize a VLAN data information structure. */
typedef struct vlan_data_s {
    int vlan_tag; 
    int port_bitmap[2]; 
    int ut_port_bitmap[2]; 
} vlan_data_t;

extern unsigned int nam_asic_vlan_entry_active
(	
	unsigned int product_id,
	unsigned short vlanId
);

extern unsigned int nam_asic_vlan_entry_ports_add
(
	unsigned char 	devNum,
	unsigned short 	vlanId,
	unsigned char 	portNum,
	unsigned char 	isTagged
); 
extern unsigned int nam_asic_port_l3intf_vlan_entry_set
(
	unsigned int product_id,
	unsigned short vlanId
);

extern unsigned int nam_asic_set_port_vlan_ingresflt
(	
	unsigned char devNum,
	unsigned char portNum,
	unsigned char enDis
);
extern unsigned int nam_asic_set_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short pvid
);
extern unsigned int nam_asic_get_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short* pvid
);
extern unsigned int nam_asic_vlan_entry_ports_del
(
	unsigned short vlanId,
	unsigned char  devNum,
	unsigned char  portNum,
	unsigned char  needJoin
);
extern unsigned int nam_asic_vlan_entry_trunk_mbr_add
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int	trunkMbrBmp0,
	unsigned int	trunkMbrBmp1,
	unsigned char isTagged
);
extern unsigned int nam_asic_vlan_entry_trunk_mbr_del
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int	trunkMbrBmp0,
	unsigned int	trunkMbrBmp1,
	unsigned char isTagged
);
extern unsigned int nam_asic_vlan_entry_delete
(
	unsigned short vlanId
);
extern unsigned int nam_vlan_update_vid
(
	unsigned int product_id,
	unsigned short vid_old,
	unsigned short vid_new
);
extern unsigned int nam_vlan_convert_port_bitmap_global_index
(
	struct DRV_VLAN_PORT_BMP_S *portbmp,
	unsigned int *eth_g_index,
	unsigned int *port_count
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

extern int nam_asic_bcast_rate_limiter_config(	unsigned int enDis );

extern unsigned int nam_asic_vlan_port_route_en
(
	unsigned char devNum,
	unsigned char portNum
);
extern unsigned int nam_asic_vlan_get_port_members_bmp
(
	unsigned short	vlanId,
	unsigned int	*untagBmp,
	unsigned int	*tagBmp
);
extern unsigned int nam_asic_trunk_get_port_member_bmp
(
	unsigned short	trunkId,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
extern unsigned int npd_eth_port_interface_check
(
	unsigned int 	eth_g_index,
	unsigned int   *ifIndex
);
extern int nam_fdb_table_delete_entry_with_vlan(unsigned int vlanid);

extern int nam_fdb_table_delete_entry_with_port
(
	unsigned int index
);
extern unsigned int npd_eth_port_dot1q_add
(
	unsigned int   eth_g_index,
	unsigned short vlanId
);
extern unsigned int npd_eth_port_dot1q_remove
(
	unsigned int   eth_g_index,
	unsigned short vlanId
);
extern unsigned int npd_eth_port_based_vlan_add
(
	unsigned int eth_g_index,
	unsigned short vlanId
);
extern unsigned int npd_eth_port_based_vlan_del
(
	unsigned int eth_g_index,
	unsigned short vlanId
);
extern unsigned int npd_fdb_static_blacklist_entry_del_with_vlan
(
     unsigned short vlanId
);
extern int npd_check_igmp_snp_port_status
(
	unsigned int eth_g_index,
	unsigned char* status
);
extern int npd_check_igmp_snp_vlan_status
(
	unsigned short vlanId,
	unsigned char *status
);

extern unsigned int nam_asic_group_mbr_bmp
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int *groupMbrBmp
);
extern unsigned int nam_asic_group_mbr_bmp_v1
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   groupAddr,
	unsigned short vlanId,
	PORT_MEMBER_BMP* groupMbrBmp
);


extern int nam_config_vlan_mtu
(
	unsigned char devNum,
	unsigned int mtuIndex,
	unsigned int mtuValue
);

extern int nam_config_vlan_egress_filter
(
	unsigned char devNum,
	unsigned int isable
);

extern int nam_config_vlan_filter
(
	unsigned short vlanId,
	unsigned int filterType,
	unsigned int enDis
);

extern int nam_config_route_mtu
(
	unsigned char devNum,
	unsigned int mtuIndex,
	unsigned int mtuValue
);

extern unsigned int nam_asic_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn
);
extern int npd_check_port_promi_mode
(
	unsigned int eth_g_index
);

extern int npd_check_port_promi_mode
(
	unsigned int eth_g_index
);
extern int npd_prot_vlan_vid_check_for_all_port
(
    unsigned short vlanId
);

extern int npd_prot_vlan_vid_check_by_slot_port
(
    unsigned char slotno,
    unsigned char local_port_no,
    unsigned short vlanId
);

extern int npd_prot_vlan_vid_check_by_global_index
(
    unsigned int eth_g_index,
    unsigned short vlanId
);

unsigned int npd_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn

 );


/**********************************************************************************
 *
 * init global vlan structure:NPD_VLAN_NUMBER_MAX pointers point to detailde vlan structure.
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
 *
 **********************************************************************************/
void npd_init_vlan(void) ;
int create_default_vlan(void);
int create_port_l3intf_vlan(void);
/**********************************************************************************
 *  npd_check_vlan_exist
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_BADPARAM  - if parameters given are wrong
 *		NPD_VLAN_EXISTS	- if vlan has been created before 
 *		NPD_VLAN_NOTEXISTS	- if vlan doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
unsigned int npd_check_vlan_exist
(
	unsigned short vlanId
);
/**********************************************************************************
 *  npd_check_vlan_status
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan is UP or DOWN
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		VLAN_STATE_UP_E  - if vlan is up
 *		VLAN_STATE_DOWN_E	- if vlan is down
 *		-NPD_VLAN_NOTEXISTS	- if vlan doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
int npd_check_vlan_status
(
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_find_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		vlanNode	- if vlan has been created before 
 *		
 **********************************************************************************/
struct vlan_s *npd_find_vlan_by_vid
(
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_find_vlan_by_name
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		name is used to compare with each vlan exists.
 *
 *	INPUT:
 *		name - vlan name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to vlan found.
 *
 **********************************************************************************/
struct vlan_s* npd_find_vlan_by_name
(
	char *name
);

/**********************************************************************************
 *  npd_vlan_fill_global
 *
 *	DESCRIPTION:
 * 		fill in vlan info to global vlan array for specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		vlanNode - vlan structure pointer
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred.
 *		NPD_VLAN_BADPARAM - if vlan id is out of range.
 *
 **********************************************************************************/
unsigned int npd_vlan_fillin_global
(
	unsigned short vlanId,
	struct vlan_s *vlanNode
);
/**********************************************************************************
 *  npd_vlan_flushout_global
 *
 *	DESCRIPTION:
 * 		flush out vlan info from global vlan array for specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred.
 *		NPD_VLAN_BADPARAM - if vlan id is out of range.
 *
 **********************************************************************************/

unsigned int npd_vlan_flushout_global
(
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_create_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Create vlan node by VLAN ID
 * 		Vlan node should be have not been created before.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to vlan being created.
 *
 **********************************************************************************/
struct vlan_s* npd_create_vlan_by_vid
(
	unsigned short vlanId
);

/**********************************************************************************
 *  npd_delete_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Delete vlan node by VLAN ID
 * 		Vlan node should be have been created before.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		NPD_VLAN_NOTEXISTS - if vlan does not exists
 *		
 *
 **********************************************************************************/
int npd_delete_vlan_by_vid
(
	unsigned short vlanId
);


/**********************************************************************************
 *  npd_vlan_check_port_membership
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_port_membership
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged
);

/**********************************************************************************
 *  npd_vlan_check_port_igmp_routeport
 *
 *	DESCRIPTION:
 * 		check out if a port is an igmp route port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		flag - route port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a igmp route port of vlan
 *		NPD_FALSE - if port is not a igmp route port of vlan
 *		
 *
 **********************************************************************************/

unsigned int npd_vlan_check_port_igmp_routeport
(
	unsigned short vlanId,
	unsigned char  isTagged,
	unsigned int   eth_g_index
);

/**********************************************************************************
 *  npd_vlan_port_igmp_mcrouter_flag_set
 *
 *	DESCRIPTION:
 * 		check out if a port is an igmp route port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		flag - route port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a igmp route port of vlan
 *		NPD_FALSE - if port is not a igmp route port of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_igmp_mcrouter_flag_set
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char  isTagged,
	unsigned char  en_dis
);
unsigned int npd_vlan_port_igmp_endis_flag_check
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char tagMode,
	unsigned char  *en_dis
);
unsigned int npd_vlan_port_igmp_endis_flag_set
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char tagMode,
	unsigned char  en_dis
);
unsigned int npd_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
);
unsigned int npd_igmpsnp_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
);
unsigned int npd_vlan_get_igmpsnp_mbr_bmp_via_sw
(
	unsigned short vlanId,
	unsigned int *untagMbrBmp_sp,/**/
	unsigned int *tagMbrBmp_sp
);
unsigned int npd_vlan_get_igmpsnp_mbr_bmp_via_sw_v1
(
	unsigned short vlanId,
	PORT_MEMBER_BMP * untagMbrBmp_sp,/**/
	PORT_MEMBER_BMP * tagMbrBmp_sp
);

unsigned int npd_vlan_get_trunk_mbr_bmp_via_sw
(
	unsigned short vlanId,
	TRUNK_MEMBER_BMP *untagMbrBmp_sp,/**/
	TRUNK_MEMBER_BMP *tagMbrBmp_sp
);

/******************************************************
 *  show vlan port membership by read Software record.
 *
 *****************************************************/
unsigned int npd_vlan_get_mbr_bmp_via_sw
(
	unsigned short vlanId,
	unsigned int *untagMbrBmp_sp,/**/
	unsigned int *tagMbrBmp_sp
);

unsigned int npd_vlan_get_mbr_bmp_via_sw_v1
(
	unsigned short vlanId,
	PORT_MEMBER_BMP *untagMbrBmp_sp,/**/
	PORT_MEMBER_BMP *tagMbrBmp_sp
);

unsigned int npd_vlan_get_mbr_bmp_wan
(
	unsigned short vlanId,
	PORT_MEMBER_BMP *tagMbrBmp_sp
);

unsigned int npd_trunk_status_get
(
	unsigned int trunkId,
	unsigned int *status
);
unsigned int npd_vlan_get_mbr_trunk_id
(
	unsigned short vid,
	unsigned char  tagMode,
	unsigned short **trunkId,
	unsigned int   *mbr_count
);
/**********************************************************************************
 *  npd_vlan_check_contain_port
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		isTagged - ethernet port is untagged or tagged port member of vlan
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_contain_port
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  *isTagged
);

unsigned int npd_vlan_check_contain_igmp_routeport
(
	unsigned short vlanId,
	unsigned int   eth_g_index
);

int npd_vlan_portmbr_igmp_flags_clear(void);

int npd_igmpsnp_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_igmpsnp_s *portIgmpSnp,
	enum ETH_PORT_NOTIFIER_ENT event
);
int npd_port_vlan_free
(
 unsigned int eth_g_index
);

/*
* make untag port free from the vlan.
*/
int npd_untag_port_del_from_vlan
(
	unsigned int eth_g_index
);

/**********************************************************************************
 *  npd_vlan_get_all_ports
 *
 *	DESCRIPTION:
 * 		get all ports in the vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		untagPorts - untag ports in the vlan
 *     tagPorts - tag ports in the vlan
 *
 * 	RETURN:
 *			
 *			NPD_SUCCESS
 *			NPD_FAIL
 *
 **********************************************************************************/
int npd_vlan_get_all_ports
(
	unsigned short vlanId,
	struct vlan_ports* untagPorts,
	struct vlan_ports* tagPorts
);

/**********************************************************************************
 *  npd_vlan_get_all_active_ports
 *
 *	DESCRIPTION:
 * 		get all ports in the vlan with valid STP/MSTP state( in Forwarding state)
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		untagPorts - untag ports in the vlan
 *     	tagPorts - tag ports in the vlan
 *
 * 	RETURN:
 *			
 *			NPD_SUCCESS
 *			NPD_FAIL
 *
 **********************************************************************************/
int npd_vlan_get_all_active_ports
(
	unsigned short vlanId,
	struct vlan_ports* untagPorts,
	struct vlan_ports* tagPorts
);

/**********************************************************************************
 *  npd_vlan_port_list_addto
 *
 *	DESCRIPTION:
 * 		add port to vlan as untagged or tagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE  - if no error occurred
 *		NPD_VLAN_PORT_EXISTS - if port is already a member of vlan
 *		...
 *
 **********************************************************************************/
unsigned int npd_vlan_port_list_addto
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged	
);

/**********************************************************************************
 *  npd_vlan_port_list_delfrom
 *
 *	DESCRIPTION:
 * 		delete a port from vlan port list
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port list
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_list_delfrom
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged
);

/**********************************************************************************
 *  npd_vlan_port_member_add
 *
 *	DESCRIPTION:
 * 		add port to vlan as untagged or tagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_NOTEXISTS - if vlan is not exists
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_member_add
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged	
);

/**********************************************************************************
 *  npd_vlan_port_member_del
 *
 *	DESCRIPTION:
 * 		delete port from vlan as untagged or tagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_NOTEXISTS - if vlan is not exists
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_member_del
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged	
);
unsigned int npd_vlan_set_port_vlan_ingresfilter
(
	unsigned int eth_g_index,
	unsigned char enDis
);
unsigned int npd_vlan_set_port_pvid
(
	unsigned int eth_g_index,
	unsigned short	pvid
);
unsigned int npd_vlan_port_pvid_get
(
	unsigned int	eth_g_index,
	unsigned short	*pvid
);
unsigned int npd_vlan_l3intf_create
(
	unsigned short	vlanId,
	unsigned int	ifIndex
);
unsigned int npd_vlan_l3intf_destroy
(
	unsigned short	vlanId,
	unsigned int	ifIndex
);

/**********************************************************************************
 *  npd_vlan_interface_check
 *
 *	DESCRIPTION:
 * 		check out if specified vlan is a L3 interface
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if vlan is a L3 interface
 *		NPD_FALSE - if vlan is not a L3 interface
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_interface_check
(
	unsigned short vlanId,
	unsigned int   *ifIndex
);

/**********************************************************************************
 *  npd_vlan_l3intf_get_mac
 *
 *	DESCRIPTION:
 * 		This route get vlan interface mac address via KAP
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		macAddr - mac address got
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_get_mac
(
	unsigned short vlanId,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_vlan_l3intf_set_mac
 *
 *	DESCRIPTION:
 * 		This route set vlan interface mac address via KAP
 *
 *	INPUT:
 *		vlanId - vlan id
 *		macAddr - mac address got
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_set_mac
(
	unsigned short vlanId,
	unsigned char *macAddr
);

/**********************************************************************************
 *  npd_vlan_l3intf_get_status
 *
 *	DESCRIPTION:
 * 		This route get vlan interface status in user space(NPD scope)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		state - l3 interface sw state
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_get_ustatus
(
	unsigned short vlanId,
	unsigned char *state
);

/**********************************************************************************
 *  npd_vlan_l3intf_set_status
 *
 *	DESCRIPTION:
 * 		This route set vlan interface status in user space(NPD scope)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		state - l3 interface sw state
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_set_ustatus
(
	unsigned short vlanId,
	unsigned char state
);

/**********************************************************************************
 *  npd_vlan_get_interface_ve_flag
 *
 *	DESCRIPTION:
 * 		get vlan l3intf flag
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		flag - L3 interface flag
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_get_interface_ve_flag
(
	unsigned short vlanId,
	enum VLAN_PORT_SUBIF_FLAG  *flag
);

/**********************************************************************************
 *  npd_vlan_set_interface_ve_flag
 *
 *	DESCRIPTION:
 * 		get vlan l3intf flag 
 *
 *	INPUT:
 *		vlanId - vlan id
 *		flag - L3 interface flag
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_set_interface_ve_flag
(
	unsigned int vid,
	enum VLAN_PORT_SUBIF_FLAG flag
);

/**********************************************************************************
 *
 * create vlan by VLAN ID on both SW and HW side
 *
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_GENERAL - if error occurred when create vlan in SW side 
 *		NPD_VLAN_ERR_HW - if error occurred when create vlan in HW side
 *		
 *
 **********************************************************************************/
/*NPD module export FUNc used in NPD_DBUS message Op*/
unsigned int npd_vlan_activate_with_vid
(
	unsigned short vlanId
);
/**********************************************************************************
 *
 * create vlan by VLAN ID on both SW and HW side
 *
 *
 *	INPUT:
 *		vlanId - vlan id
 *		name - vlan alias name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_GENERAL - if error occurred when create vlan in SW side 
 *		NPD_VLAN_ERR_HW - if error occurred when create vlan in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_activate
(
	unsigned short vlanId,
	char *name
);

/**********************************************************************************
 *
 * change vlan untag port memebers and fdb entry by port link status
 *
 *
 *	INPUT:
 *		vid - vlan id
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/
int npd_vlan_untag_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_vlan_s *portVlan,
	enum ETH_PORT_NOTIFIER_ENT event
);

/**********************************************************************************
 *
 * change vlan tag port memebers and fdb entry by port link status
 *
 *
 *	INPUT:
 *		dot1vlanlst - all vlans of port added by tag 
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/
int npd_vlan_tag_port_link_change
(
	unsigned int eth_g_index,
	dot1q_vlan_list_s* dot1Vlanlst,
	enum ETH_PORT_NOTIFIER_ENT event
);

/**********************************************************************************
 *  npd_vlan_add_or_del_port
 *
 *	DESCRIPTION:
 * 		vlan add or delete port as tagged or untagged mode
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *      isAdd - add or delete port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		VLAN_RETURN_CODE_ERR_NONE - add or delete port success
 *
 **********************************************************************************/
 
unsigned int npd_vlan_add_or_del_port
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged,
	unsigned char   isAdd
);


/**********************************************************************************
 *  npd_vlan_get_port_wan
 *
 *	DESCRIPTION:
 * 		get whether the port is wan by global ethernet port index
 *
 *	INPUT:
 *		eth_g_index - global ethernet port index
 *		iswan - wan interface flag
 *				1: is a wan interface
 *				0:is not a wan interface
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ret
 *
 **********************************************************************************/
 
unsigned int npd_vlan_get_port_wan
(
	unsigned int	eth_g_index,
	unsigned int 	*iswan
);

unsigned int npd_vlan_interface_cscd_add
(
	unsigned short	vlanId,
	unsigned char	port,
	unsigned char	isTagged
);

unsigned int npd_vlan_interface_port_add
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
);
/*
*/
unsigned int npd_vlan_tag_port_add_for_subif
(
	unsigned short	vlanId,
	unsigned int	eth_g_index
);

unsigned int npd_vlan_interface_port_del
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
);
unsigned int npd_vlan_tag_port_del_for_subif
(
	unsigned short	vlanId,
	unsigned int	eth_g_index
);

int npd_vlan_trunk_entity_add
(
	unsigned short	vlanId,
	unsigned short	trunkId,
	unsigned char 	trunkTag
);
int npd_vlan_trunk_entity_del
(
	unsigned short	vlanId,
	unsigned short	trunkId,
	unsigned char 	trunkTag
);
unsigned int nam_vlan_trunk_member_get
(
	unsigned short trunkId,
	unsigned int *memberCount,
	unsigned char memberArray[TRUNK_MEMBER_NUM_MAX]
);
unsigned int npd_vlan_trunk_member_add
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  trunkTag
);
unsigned int npd_vlan_trunk_member_del
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char 	tagMode
);
/*
unsigned int npd_vlan_interface_trunk_add_del
(
	unsigned short	trunkId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
);
*/
unsigned int npd_vlan_interface_destroy_node
(
	unsigned short vlanId
);

unsigned int npd_vlan_interface_show_port_member
(
	unsigned short	vlanId,
	unsigned int	ethPortIndx,
	unsigned char	*isTaggPtr,
	unsigned short	*pvid
);

void npd_save_vlan_cfg
(
	char* buf,
	int bufLen
);

unsigned int npd_show_vidx_member
(
	unsigned short vlanId,
	unsigned short vlanIdx
);
unsigned int npd_vlan_check_contain_trunk
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  *isTagged
);
int npd_igmp_port_link_change
(
	unsigned int eth_g_index,	
	enum ETH_PORT_NOTIFIER_ENT event
);
/*
	npd_vlan_acl_enable_check
*/
unsigned int npd_vlan_acl_enable_check
(
	unsigned short vlanId,
	unsigned int dir_info
);
/*npd_vlan_acl_bind_check



ACL_GROUP_VLAN_BINDED	//bind,ignore acl enable or not
NPD_DBUS_SUCCESS		//acl----enable ,no group binded
NPD_DBUS_ERROR			//acl----disable,no group binded
NPD_VLAN_NOTEXISTS		//vlan error

*/
unsigned int npd_vlan_acl_bind_check
(
	unsigned short vlanId,
	unsigned int   *groupNo,
	unsigned int   dir_info
);
/**********************************************************************************
 * npd_count_static_arp_in_port
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		eth_g_index - eth_port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if there are no static arp items in the eth_port.
 *		1 - if there are static arp items in the eth_port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_in_port
(
    unsigned int eth_g_index
);
/*npd_vlan_unbind_group

NPD_VLAN_NOTEXISTS
ACL_GROUP_NOT_EXISTED //no group
NPD_DBUS_SUCCESS	//delete success
NPD_DBUS_ERROR    //wrong gourp id

*/
unsigned int npd_vlan_unbind_group
(
	unsigned short vlanId,
	unsigned int group_num,
	unsigned int dir_info
);

/*get promiscuous mode port bitmap*/
unsigned int npd_vlan_get_promis_port_bmp(unsigned int * promisPortBmp);

/*****************************************************************************************************
 *      NPD dbus operation
 *
 *****************************************************************************************************/
DBusMessage * npd_dbus_vlan_create_vlan_entry_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/*enter vlan configure node*/
DBusMessage * npd_dbus_vlan_config_layer2
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/*enter vlan configure node*/
DBusMessage * npd_dbus_vlan_config_layer2_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_config_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_config_vlan_update_vid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*original*/
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;
/*original*/
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
); 
/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_vname_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
); 


/*add | delete trunk to(from) vlan*/
DBusMessage * npd_dbus_vlan_config_trunk_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;

DBusMessage * npd_dbus_vlan_config_trunk_untag_tag_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_delete_vlan_entry_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_delete_vlan_entry_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*npd_dbus_vlan_set_port_vlan_ingresflt*/
DBusMessage * npd_dbus_vlan_set_port_vlan_ingresflt
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*npd_dbus_vlan_set_one_port_pvid*/
DBusMessage * npd_dbus_vlan_set_one_port_pvid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*npd_dbus_vlan_show_one_port_pvid*/
DBusMessage * npd_dbus_vlan_show_one_port_pvid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) ;


/*npd_dbus_vlan_show_ports_pvid*/
DBusMessage * npd_dbus_vlan_show_ports_pvid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_running_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_check_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_igmp_snoop_count
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_portmbr_check_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage* npd_dbus_vlan_portmbr_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_mcrouterport_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_show_one_group_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
DBusMessage * npd_dbus_vlan_show_vlan_group_list_port_mbr
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

DBusMessage * npd_dbus_vlan_show_vlan_group_list_port_mbr_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
unsigned int npd_vlan_get_port_bmp_by_mode(unsigned int * portBmp,unsigned int mode);
/* check a vlan interface node has exists in the list or not*/
/* we use vlanId as a index for searching for the node*/
/**********************************************************************************
 *  npd_check_vlan_real_exist
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUE	- if vlan has been created before 
 *		FALSE	- if vlan doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
unsigned int npd_check_vlan_real_exist
(
	unsigned short vlanId
);

#ifndef AX_PLATFORM_DISTRIBUTED
#define VLAN_UP 0x1
#define VLAN_DOWN 0x0
#define VLAN_VALID 0x1
#define VLAN_INVALID 0x0
#define VLAN_BONDED 0x1
#define VLAN_NOBONDED 0x0

int npd_init_distributed_vlan(void);
unsigned int npd_vlan_cscd_add(unsigned short vlanId);
unsigned int npd_vlan_cscd_del(unsigned short vlanId);
unsigned int npd_vlanlist_valid(unsigned short vlanId,char *vlanName);
unsigned int npd_vlanlist_invalid(unsigned short vlanId);
unsigned int npd_vlan_to_cpu_port_add(unsigned short vlanId,unsigned short cpu_no,unsigned short cpu_port_no);
unsigned int npd_vlan_to_cpu_port_del(unsigned short vlanId,unsigned short cpu_no,unsigned short cpu_port_no);
int npd_get_default_vlan_smu(void);
void npd_vlan_check_state(void);
int npd_vlan_check_bond_info(int vlanId);
unsigned int npd_vlanlist_add_del_trunk(unsigned short vlanId,unsigned short trunkId,unsigned char isAdd,unsigned char isTag);

/* update g_vlan_list[4096] when add/delete port */
DBusMessage *npd_dbus_vlan_config_vlanlist_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/* read g_vlanlist[4096] to show vlan port-member list */
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member_distributed
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);
/* Bond vlan to cpu port on special slot of distributed system  */
DBusMessage *npd_dbus_vlan_to_cpu_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
);

/*******************************************************************************
* force port 26,27 of XCAT to 10G mode  in nam_hw_diag.c
*******************************************************************************/
extern int force_to_xg_mode(void);

/*******************************************************************************
* nam_thread_create  in nam_utilus.c
*******************************************************************************/
extern void nam_thread_create
(
	char	*name,
	unsigned (*entry_point)(void*),
	void	*arglist,
	unsigned char accessChip,
	unsigned char needJoin
);
#endif

#endif /* _NPD_VLAN_H*/
