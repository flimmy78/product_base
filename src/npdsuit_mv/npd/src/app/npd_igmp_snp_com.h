#ifndef __NPD_IGMP_SNP_COM_H__
#define __NPD_IGMP_SNP_COM_H__

/* include header file begin */
/* kernel or sys part */
#include <sys/socket.h>
#include <linux/netlink.h>
/* user or app part */
/* include header file end */

/* MACRO definition begin */
#define NPD_IGMP_SNP_SUCCESS 0
#define NPD_IGMP_SNP_ERRO_HW (NPD_IGMP_SNP_SUCCESS+1)
#define NPD_IGMP_SNP_ERRO_SW (NPD_IGMP_SNP_SUCCESS+2)
#define NPD_IGMP_SNP_VLAN_NOTEXIST (NPD_IGMP_SNP_SUCCESS+3)
#define NPD_IGMP_SNP_NOTENABLE	 (NPD_IGMP_SNP_SUCCESS+4)
#define NPD_IGMP_SNP_NOTENABLE_VLAN	(NPD_IGMP_SNP_SUCCESS+5)
#define NPD_IGMP_SNP_HASENABLE_VLAN	(NPD_IGMP_SNP_SUCCESS+6)
#define NPD_IGMP_SNP_NOTENABLE_PORT	(NPD_IGMP_SNP_SUCCESS+7)
#define NPD_IGMP_SNP_HASENABLE_PORT	(NPD_IGMP_SNP_SUCCESS+8)
#define NPD_IGMP_SNP_PORT_NOTEXIST	(NPD_IGMP_SNP_SUCCESS+9)
#define NPD_IGMP_SNP_NOTROUTE_PORT	(NPD_IGMP_SNP_SUCCESS+10)
#define IGMP_SNOOP_ERR_NOT_ENABLE_GLB	0xff

#define NPD_GROUP_SUCCESS 	0
#define NPD_GROUP_NOTEXIST (NPD_GROUP_SUCCESS+1)
#define NPD_ROUTE_PORT_NOTEXIST (NPD_GROUP_SUCCESS+2)


#define NPD_MC_FDB_ENTRY_SUCCESS 		0
#define NPD_MC_FDB_ENTRY_EXIST 			(NPD_MC_FDB_ENTRY_SUCCESS+2)
#define NPD_MC_FDB_ENTRY_ERROR_OUT_MAX	(NPD_MC_FDB_ENTRY_SUCCESS+3)
#define NPD_MC_FDB_ENTRY_ERROR_GENERAL	(NPD_MC_FDB_ENTRY_SUCCESS+4)
#define NPD_IGMP_SNP_GROUP_NUM_MAX 		4095/* notations */


#define NPD_IGMPSNP_FD 0
typedef struct{
    unsigned int   ports[2];
}CPSS_PORTS_BMP_STC;

/*external function declarations begin*/
typedef enum
{
    CPSS_IP_PROTOCOL_IPV4_E     = 0,
    CPSS_IP_PROTOCOL_IPV6_E     = 1,
    CPSS_IP_PROTOCOL_IPV4V6_E   = 2
}CPSS_IP_PROTOCOL_STACK_ENT;

typedef enum
{
    CPSS_BRG_IPM_SGV_E,
    CPSS_BRG_IPM_GV_E
}CPSS_BRG_IPM_MODE_ENT;

extern unsigned long cpssDxChBrgMcEntryRead
(
    unsigned char		devNum,
    unsigned short		vidx,
    CPSS_PORTS_BMP_STC	*portBitmapPtr
);

extern unsigned int nam_asic_igmp_trap_set_by_vid
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned long 	enable
);

extern unsigned int nam_asic_igmp_trap_set_by_devport
(
	unsigned short	vid,
    unsigned char   devNum,
    unsigned char  	portNum,
    unsigned long 	enable
);
extern unsigned int nam_asic_vlan_ipv4_mc_enalbe
(
	unsigned char devNum,	
	unsigned short vlanId,	
	unsigned long enable
);
extern unsigned int nam_asic_vlan_ipv4_mcmode_set
(
	unsigned char devNum,
	unsigned short vlanId
);
extern unsigned int nam_asic_vlan_igmp_enable_drop_unmc
(	
	unsigned char devNum,
	unsigned short vlanId
);
extern unsigned int nam_asic_vlan_igmp_enable_forword_unmc
( 
	unsigned char devNum,
	unsigned short vlanId
);

extern unsigned int nam_asic_l2mc_member_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId,
	unsigned char *hasMbr
);

extern unsigned int nam_asic_l2mc_member_add
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId
);

extern unsigned int nam_asic_l2mc_group_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   group_ip,
	unsigned short vlanId
);
extern int nam_static_fdb_entry_direct_write_for_igmp
(
	unsigned int dip,
	unsigned short vidx,
	unsigned short vlanId
) ;
extern int nam_static_fdb_entry_indirect_set_for_igmp
(
	unsigned int dip,
	unsigned short vidx,
	unsigned short vlanId
) ;
extern int nam_static_fdb_entry_indirect_delete_for_igmp
(
	unsigned int dip,
	/*unsigned short vidx,	*//*no need for Vidx*/
	unsigned short vid
); 

extern unsigned int nam_mc_entry_read
(
   unsigned char devNum,
   unsigned short vidx,
   CPSS_PORTS_BMP_STC * portBitmapPtr
);

extern unsigned int npd_mld_copy_ipv6addr
(
	unsigned short * ugroup,
	unsigned short ** destugroup
);

/*external function declarations end*/

/* MACRO definition end */

/* structure definition begin */
typedef struct _mc_fdb_list
{
	struct _mc_fdb_list *next;
	struct _mc_fdb_list *prev;
	unsigned short vlanId;
	unsigned int  groupIp;        
	unsigned short vidx; 
}mc_fdb_list;


typedef struct 
{
	struct _mc_fdb_list *stListHead;
	unsigned int ulListNodeCount;
}mc_fdb_list_head;

typedef struct{
	unsigned char vidBit;
	unsigned char mbrArray[NPD_IGMP_SNP_GROUP_NUM_MAX];
}groupBasedVlan;


#ifndef __NPD_CMD_IGMP__

#define IGMP_SNP_TYPE_DEVICE_EVENT	3	/*device message*/

struct npd_dev_event_cmd{
	unsigned long event_type;
	unsigned short vid;
	unsigned int port_index;/*ifindex*/
};

/**device event ****/
#define NPD_IGMPSNP_EVENT_DEV_UP			(0x0001)
#define NPD_IGMPSNP_EVENT_DEV_DOWN			(0x0002)
#define NPD_IGMPSNP_EVENT_DEV_REGISTER		(0x0003)
#define NPD_IGMPSNP_EVENT_DEV_UNREGISTER	(0x0004)
#define NPD_IGMPSNP_EVENT_DEV_VLANADDIF		(0x0005)
#define NPD_IGMPSNP_EVENT_DEV_VLANDELIF		(0x0006)
#define NPD_IGMPSNP_EVENT_DEV_MCROUTER_PORT_UP	 (0x000f)
#define NPD_IGMPSNP_EVENT_DEV_MCROUTER_PORT_DOWN (0x0010)
#define NPD_IGMPSNP_EVENT_DEV_SYS_MAC_NOTI		 (0x0011) 	
#define	IGMP_SNP_TYPE_NOTIFY_MSG	1	/*notify message*/
#define	IGMP_SNP_FLAG_ADDR_MOD		3	/*notify information for modify address*/


#define IGMP_SYS_SET_INIT	1
#define IGMP_SYS_SET_STOP	2

enum igmpmodaddtype
{
	IGMP_ADDR_ADD,
	IGMP_ADDR_DEL,
	IGMP_ADDR_RMV,
	IGMP_SYS_SET,
	IGMP_TRUNK_UPDATE
};
#define IGMP_VLAN_ADD_PORT	13
#define	IGMP_VLAN_DEL_PORT	14	/*VLANÉ¾³ý¶Ë¿Ú*/
#define	IGMP_PORT_DOWN		15	/*¶Ë¿Údown*/
#define	IGMP_VLAN_DEL_VLAN	16	/**/


/*
 *
 * device event notifies to igmp Snoop
 *  ***Device notify message***
 *
 */
#define MACADDRLENGTH 6

typedef struct 
{
	unsigned long	event;
	long	vlan_id;
	unsigned long	ifindex;
	unsigned char 	sys_mac[MACADDRLENGTH];
	unsigned long	reserve;
}dev_notify_msg;

struct npd_mng_igmp{
		struct nlmsghdr nlh;
		dev_notify_msg npd_dev_mng;
};


/*
 *
 *IGMP snoop module notifies to device.
 *	****IGMP notify message****
 *
 *
 */
struct igmp_notify_mod_npd{
	unsigned long	mod_type;
	long	vlan_id;
	unsigned long	ifindex;
	unsigned long	groupadd;
	unsigned long	reserve;
};

struct igmp_msg_npd{
		struct nlmsghdr nlh;
		struct igmp_notify_mod_npd igmp_noti_npd;
};


extern struct eth_port_s *npd_get_port_by_index
(
	unsigned int eth_g_index
);
/*local function declarations begin*/
unsigned int npd_mc_fdb_entry_insert
(
	unsigned short vid,
	unsigned long groupIp,
	unsigned short vidx,
	unsigned int *entryExist
);
unsigned int npd_mc_fdb_entry_delete
(
	unsigned short vid,
	unsigned long groupIp,
	unsigned short vidx,
	unsigned int *entryExist
);
unsigned int npd_mc_fdb_entry_delete_all
(
	unsigned short vid[],
	unsigned int groupIp[],
	unsigned short vidx[],
	unsigned int *fdbCnt
);
unsigned int npd_igmp_check_contain_fdb
(
	unsigned short vlanId,
	unsigned short vidx
	/*unsigned int	groupAddr*/
);
unsigned int npd_igmp_fdb_entry_addto
(
	unsigned short vid,
	unsigned int groupIp,
	unsigned short vidx
);
unsigned int npd_igmp_fdb_entry_delfrom
(
	unsigned short vid,
	unsigned short vidx
);
unsigned int npd_igmp_get_groupip_by_vlan_vidx
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int	*groupAddr,
	unsigned short **groupAddrv6,
	unsigned int	*ret
);
unsigned int npd_igmp_get_groupaddr_by_vlanid_vidx
(
	unsigned short vid,
	unsigned short vidx,
	unsigned int *groupAddr
);
int npd_l2mc_entry_mbr_add
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int   eth_g_index,
	unsigned int   group_ip
);
int npd_l2mc_entry_mbr_del
(
	unsigned short vlanId,
	unsigned short vidx, 
	unsigned int eth_g_index,
	unsigned int group_ip
);
int npd_l2mc_entry_delete
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int   group_ip
);

/*EVENT_DEV_UP,EVENT_DEV_DOWN,EVENT_DEV_UNREGISTER*/
/******************/
int npd_msg_sendto_igmpsnp
(
	struct npd_dev_event_cmd* mngCmd	
);
int npd_igmp_sysmac_notifer(void);

/******************/
unsigned int npd_igmp_snp_recvmsg_proc
(
	struct igmp_notify_mod_npd* igmp_notify
);

/******************/
int creatservsock_dgram(char *path);
int creatservsock_stream(char *path);
int creatclientsock_stream(char *path,int *sock);
void igmp_snp_init(void);
/*****************/
void *npd_igmp_snp_mng_thread(void);

void *npd_igmp_snp_mng_thread_dgram(void);

/****************/
int npd_vlan_igmp_snp_endis_config
(
	unsigned char	isAdd,
	unsigned short	vid
	/*here wanted VIDX!!*/
);

/***************/
int npd_vlan_port_igmp_snp_endis_config
(
	unsigned char enable,
	unsigned short vid,
	unsigned int eth_g_idx,
	unsigned char tagMode
);
int npd_mcrouter_port_igmp_snp_endis_config
(
	unsigned char enable,
	unsigned short vid,
	unsigned int eth_g_idx
);
int npd_check_igmp_snp_status(unsigned char * status);
int npd_check_igmp_snp_vlan_status(unsigned short vlanId,unsigned char *status);

int npd_check_igmp_snp_vlanMbr_status
(
	unsigned short vlanId,
	unsigned int eth_g_index,
	unsigned char *status
);

int npd_igmp_snp_l2mc_vlan_exist(unsigned short vlanId);
int npd_igmp_snp_l2mc_group_exist(unsigned short vlanId,unsigned short vidx);
int npd_l2mc_entry_mbr_exist(unsigned short vidx);

/*local function declarations end*/
#endif /* __FILENAME_H_ */
#endif
