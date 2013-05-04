#ifndef __NPD_ACL_DRV_H__
#define __NPD_ACL_DRV_H__

#define NPD_ACL_RULE_SHOWRUN_CFG_SIZE (100*1024)

#define MAX_IP_STRLEN	16
#define MAX_MAC_STRLEN  18
#define MAX_PORT_NUMBER 24
#define MAX_GROUP_NUM 1024

#define MAX_ACL_RULE_NUMBER	1000
#define MAX_ACL_EXT_RULE_NUMBER	500
#define MAX_CFG_LEN 100
#define MAX_VID_IDNUM 4096   /*vid 0-4095*/
#define MAX_TRUNK_IDNUM 256  /*trunkid (0-255)*/
#define MAX_PORT_IDNUM 24    /*port 0-23*/
#define MAX_VLANID_IDNUM 4096 /*vlanId*/

#define ACCESS_PORT_TYPE 0
#define ACCESS_VID_TYPE  1

#define STANDARD_ACL_RULE 0
#define EXTENDED_ACL_RULE 1

#define ACL_DIRECTION_INGRESS 0
#define ACL_DIRECTION_EGRESS  1
#define ACL_DIRECTION_TWOWAY  2

#define ACL_TIME_NAME_EXIST    1
#define ACL_TIME_NAME_NOTEXIST 2
#define ACL_TIME_PERIOD_NOT_EXISTED 3
#define ACL_TIME_PERIOD_EXISTED 4

#define QOS_PROFILE_NOT_EXISTED 4
#define NPD_TIME_RANGE_SIZE		32
#define ACL_ANY_PORT NPD_ACL_RULE_SHOWRUN_CFG_SIZE
#define ACL_ANY_PORT_CHAR 255
#define MAX_POLICER_NUM				256
#define ACL_DEFAULT_DEVID		ACL_ANY_PORT_CHAR
#define ACL_SAVE_FOR_UDP_GROUP 	999

/* Port PBMP operators */
#define NPD_PBMP_PORT_SET(pbm, port)    ((pbm) = (1 << (port)))
#define NPD_PBMP_PORT_ADD(pbm, port)    ((pbm) |= (1 << (port)))
#define NPD_PBMP_PORT_REMOVE(pbm, port) ((pbm) &= ~(1 << (port)))
#define NPD_PBMP_PORT_FLIP(pbm, port)	 ((pbm) ^= (1 << (port)))

typedef enum
{
	ACL_FALSE = 0,
	ACL_TRUE  = 1
}BOOL;

#define ACL_GROUP_ERROR_NONE			0
#define ACL_GROUP_SUCCESS				(ACL_GROUP_ERROR_NONE)
#define ACL_GROUP_EXISTED				(ACL_GROUP_ERROR_NONE + 1)
#define ACL_GROUP_NOT_EXISTED 			(ACL_GROUP_ERROR_NONE + 16)
#define ACL_GROUP_PORT_NOTFOUND			(ACL_GROUP_ERROR_NONE + 3)
#define ACL_GROUP_RULE_NOTEXISTED		(ACL_GROUP_ERROR_NONE + 4)
#define ACL_GROUP_RULE_EXISTED			(ACL_GROUP_ERROR_NONE + 5)
#define NPD_DBUS_ACL_ERR_GENERAL		(ACL_GROUP_ERROR_NONE + 6)
#define ACL_GROUP_PORT_BINDED    		(ACL_GROUP_ERROR_NONE + 7)
#define ACL_GROUP_NOT_SHARE         	(ACL_GROUP_ERROR_NONE + 8)
#define ACL_GLOBAL_NOT_EXISTED          (ACL_GROUP_ERROR_NONE + 9)
#define ACL_GLOBAL_EXISTED              (ACL_GROUP_ERROR_NONE + 10)
#define ACL_SAME_FIELD				    (ACL_GROUP_ERROR_NONE + 11)
#define ACL_EXT_NO_SPACE				(ACL_GROUP_ERROR_NONE + 12)
#define ACL_GROUP_NOT_BINDED			(ACL_GROUP_ERROR_NONE + 13)
#define ACL_GROUP_VLAN_BINDED           (ACL_GROUP_ERROR_NONE + 14)
#define ACL_GROUP_EGRESS_ERROR          (ACL_GROUP_ERROR_NONE + 17)
#define EGRESS_ACL_GROUP_RULE_EXISTED   (ACL_GROUP_ERROR_NONE + 18)
#define ACL_GROUP_EGRESS_NOT_SUPPORT    (ACL_GROUP_ERROR_NONE + 19)
#define ACL_GROUP_WRONG_INDEX		 	(ACL_GROUP_ERROR_NONE + 20)
#define ACL_ON_PORT_DISABLE         	(ACL_GROUP_ERROR_NONE + 21)
#define ACL_POLICER_ID_NOT_SET      	(ACL_GROUP_ERROR_NONE + 22)
#define ACL_GROUP_SAME_ID		    	(ACL_GROUP_ERROR_NONE + 23)
#define ACL_RULE_TIME_NOT_SUPPORT         (ACL_GROUP_ERROR_NONE + 24)
#define ACL_RULE_INDEX_ERROR             (ACL_GROUP_ERROR_NONE + 25)
#define ACL_GROUP_INDEX_ERROR             (ACL_GROUP_ERROR_NONE + 26)
#define ACL_NORMAL_MALLOC_ERROR_NONE	 (ACL_GROUP_ERROR_NONE + 100)
#define ACL_MIRROR_USE					(ACL_GROUP_ERROR_NONE + 27)
#define ACL_RULE_EXT_ONLY					(ACL_GROUP_ERROR_NONE + 28)
#define ACL_UNBIND_FRIST					(ACL_GROUP_ERROR_NONE + 29)
#define ACL_ADD_EQUAL_RULE					(ACL_GROUP_ERROR_NONE + 30)
#define ACL_RANGE_NOT_EXIST					(ACL_GROUP_ERROR_NONE + 31)
#define ACL_UDP_VLAN_RULE_ENABLE				(ACL_GROUP_ERROR_NONE + 32)
#define ACL_PORT_NOT_SUPPORT_BINDED				(ACL_GROUP_ERROR_NONE + 33)

struct ipv6addr {
	unsigned char ipbuf [16];
};

typedef struct{
  unsigned long ruleIndex;
  unsigned int  startIndex;
  unsigned int  endIndex;
  unsigned int  ruleType;
  unsigned long dip;
  unsigned long sip;
  unsigned long  maskdip;
  unsigned long  masksip;
  struct ipv6addr dipv6;
  struct ipv6addr sipv6;
  unsigned int nextheader;
  unsigned long srcport;
  unsigned long dstport;
  unsigned char icmp_code;
  unsigned char code_mask;
  unsigned char icmp_type;
  unsigned char type_mask;
  unsigned long packetType;
  unsigned long actionType;      
  unsigned char dmac[6];
  unsigned char smac[6];
  unsigned int  vlanid;
  unsigned char sourceslot;
  unsigned char sourceport;
  unsigned char mirrorslot;
  unsigned char mirrorport;
  unsigned char redirectslot;	
  unsigned char redirectport;
  unsigned int policer;
  unsigned int  policerId;  
  unsigned int  modifyUP;
  unsigned int  modifyDSCP;
  unsigned int  up;
  unsigned int dscp;
  unsigned int egrUP;
  unsigned int egrDSCP;
  unsigned int precedence;
  unsigned int qosprofileindex;
  unsigned char appendInfo;
  
}ACL_SHOW_STC;

typedef struct{
	unsigned int group_num;
	unsigned int count;
	unsigned int portnum;
	unsigned int vid;
}GROUP_SHOW_STC;

typedef struct{
	unsigned int rule_index;
}GROUP_RULE_INDEX_STC;

typedef struct{
	unsigned int slot;
	unsigned int port;
	unsigned int ethindex;
	unsigned int vidindex;
}GROUP_ETH_INDEX_STC;
typedef enum
{
	CPSS_DXCH_ACL_RULE_FORMAT_INGRESS = 0,
	CPSS_DXCH_ACL_RULE_FORMAT_EGRESS,
	CPSS_DXCH_ACL_RULE_FORMAT_MAX
}ACL_TCAM_RULE_DIRECTION_E;

typedef enum 
{
	ACL_RULE_STD_NOT_IP = 0,
	ACL_RULE_STD_IP_L2_QoS,
	ACL_RULE_STD_IPv4_L4,
	ACL_RULE_STD_IPv6_DIP,
	ACL_RULE_EXT_IPv4_L4,
	ACL_RULE_EXT_NOT_IPv6,
	ACL_RULE_EXT_IPv6_L2,
	ACL_RULE_EXT_IPv6_L4,
	ACL_RULE_QOS,
	ACL_RULE_MAX
	
}ACL_TCAM_RULE_FORMAT_E;

typedef enum 
{
	ACL_ACTION_TYPE_PERMIT = 0,
	ACL_ACTION_TYPE_DENY,
	ACL_ACTION_TYPE_TRAP_TO_CPU,
	ACL_ACTION_TYPE_MIRROR_TO_ANALYZER,
	ACL_ACTION_TYPE_REDIRECT,
	ACL_ACTION_TYPE_INGRESS_QOS,
	ACL_ACTION_TYPE_EGRESS_QOS,
	ACL_ACTION_TYPE_MAX
}ACL_ACTION_TYPE_E;

typedef enum
{
	ACL_ACTION_REDIRECT_PORT = 0,
	ACL_ACTION_REDIRECT_VID,
	ACL_ACTION_REDIRECT_VIDX,
	ACL_ACTION_REDIRECT_TRUNK,
	ACL_ACTION_REDIRECT_MAX
}ACL_REDIRECT_DST_TYPE_E;

typedef enum
{
	ACL_TCAM_RULE_PACKETTYPE_IP = 0,
	ACL_TCAM_RULE_PACKETTYPE_UDP,
	ACL_TCAM_RULE_PACKETTYPE_TCP,
	ACL_TCAM_RULE_PACKETTYPE_ICMP,
	ACL_TCAM_RULE_PACKETTYPE_ETHERNET,
	ACL_TCAM_RULE_PACKETTYPE_ARP,
	ACL_TCAM_RULE_PACKETTYPE_QOS,
	ACL_TCAM_RULE_PACKETTYPE_IPV6,
	ACL_TCAM_RULE_PACKETTYPE_TCPV6,
	ACL_TCAM_RULE_PACKETTYPE_MAX
}ACL_TCAM_RULE_PACKETTYPE_E;

/*  ACL_ACTION_TRAP_TO_CPU */
 struct ACL_ACTION_TRAP_TO_CPU
{
   unsigned long cpucode;
} ;

/*	ACL_ACTION_REDIRECT_STC*/
struct ACL_ACTION_REDIRECT_STC {
	unsigned char portNum;
	unsigned char modid;
};


/*ACL_ACTION_MIRROR_TO_ANALYZER */
 struct ACL_ACTION_MIRROR_TO_ANALYZER 
{
	unsigned char portNum;
	unsigned char modid;
};

/*---STDNOTIP-------------*/
typedef struct _ACL_STDNOTIP_PATTERN_S_
{
	unsigned int ACLId;
	unsigned char dMac[6];
	unsigned char sMac[6];
	unsigned int  vlanId;
	unsigned char  portno;
	unsigned int modid;
	unsigned int ethertype;
}STDNOTIP_PATTERN_T;

typedef struct _ACL_STDNOTIP_MASK_S_
{
	unsigned int  maskACLId;
	unsigned char maskDMac[6];
	unsigned char maskSMac[6];
	unsigned int  maskVlanId;
	unsigned char  maskPortno;
	unsigned int  maskethertype;
	
}STDNOTIP_MASK_T;

typedef struct _ACL_RULE_STDNOTIP_INFO_
{
	STDNOTIP_PATTERN_T 	rule;
	STDNOTIP_MASK_T		mask;
}ACL_RULE_STDNOTIP_T;

/*---STDIPL2QOS--------*/
typedef struct _ACL_STDIPL2QOS_PATTERN_S_
{
	int ACLId;
	unsigned char dMac[6];
	unsigned char sMac[6];
	
}STDIPL2QOS_PATTERN_T;

typedef struct _ACL_STDIPL2QOS_MASK_S_
{
	BOOL maskACLId;
	unsigned char maskDMac[6];
	unsigned char maskSMac[6];
	
}STDIPL2QOS_MASK_T;

typedef struct _ACL_RULE_STDIPL2QOS_INFO
{
	STDIPL2QOS_PATTERN_T 	rule;
	STDIPL2QOS_MASK_T		mask;
}ACL_RULE_STDIPL2QOS;

/*---STDIPV4L4----------*/
typedef struct _ACL_STDIPV4L4_PATTERN_S_
{
	unsigned int   ACLId;
	unsigned long  Dip;
	unsigned long  Sip;
	unsigned char  Type;
	unsigned char  Code;
	unsigned long  SrcPort;
	unsigned long  DstPort;
	unsigned int   UP;
	unsigned int   DSCP;
}STDIPV4L4_PATTERN_T;

typedef struct _ACL_STDIPV4L4_MASK_S_
{
	unsigned int   maskACLId;
	unsigned long  maskDip;
	unsigned long  maskSip;
	unsigned long  maskLenDip;
	unsigned long  maskLenSip;	
	unsigned char  maskType;
	unsigned char  maskCode;
	unsigned long  maskSrcPort;
	unsigned long  maskDstPort;
	unsigned int   maskUP;
	unsigned int   maskDSCP;
}STDIPV4L4_MASK_T;

typedef struct _ACL_RULE_STDIPV4L4_INFO
{
	STDIPV4L4_PATTERN_T 	rule;
	STDIPV4L4_MASK_T		mask;
}ACL_RULE_STDIPV4L4_T;

/*------STDIPV6DIP----------*/
typedef struct _ACL_STDIPV6DIP_PATTERN_S_
{
	int ACLId;
	unsigned long Dip;
	unsigned long Sip;
	unsigned int type;
	unsigned int Code;
	unsigned int SrcPort;
	unsigned int DstPort;

}STDIPV6DIP_PATTERN_T;

typedef struct _ACL_STDIPV6DIP_MASK_S_
{
	BOOL maskACLId;
	unsigned long maskDip;
	unsigned long maskSip;
	unsigned int  maskLenDip;
	unsigned int  maskLenSip;
	BOOL maskType;
	BOOL maskCode;
	BOOL maskSrcPort;
	BOOL maskDstPort;
	
}STDIPV6DIP_MASK_T;

typedef struct _ACL_RULE_STDIPV6DIP_INFO
{
	STDIPV6DIP_PATTERN_T 	rule;
	STDIPV6DIP_MASK_T		mask;
}ACL_RULE_STDIPV6DIP_T;

/*------EXTNOTIPV6----------*/
typedef struct _ACL_EXTNOTIPV6_PATTERN_S_
{
    unsigned int ACLId;
	unsigned long Dip;
	unsigned long Sip;
	unsigned long SrcPort;
	unsigned long DstPort;
	unsigned char dMac[6];
	unsigned char sMac[6];
	unsigned int  vlanId;
	unsigned char  portNo;
	unsigned int modid;
}EXTNOTIPV6_PATTERN_T;

typedef struct _ACL_EXTNOTIPV6_MASK_S_
{
    unsigned int maskACLId;
	unsigned long maskDip;
	unsigned long maskSip;
	unsigned long  maskLenDip;
	unsigned long  maskLenSip;	
	unsigned long maskSrcPort;
	unsigned long maskDstPort;
	unsigned char maskDmac[6];
	unsigned char maskSmac[6];
	unsigned int  maskVlanId;
	unsigned char  maskPortNo;
	
}EXTNOTIPV6_MASK_T;


typedef struct _ACL_RULE_EXTNOTIPV6_INFO
{
	EXTNOTIPV6_PATTERN_T 	rule;
	EXTNOTIPV6_MASK_T		mask;
}ACL_RULE_EXTNOTIPV6_T;

/*------EXTIPV6L2----------*/
typedef struct _ACL_EXTIPV6L2_PATTERN_S_
{
	int ACLId;
	unsigned long Dip;
	unsigned long Sip;
	unsigned int type;
	unsigned int Code;
	unsigned int SrcPort;
	unsigned int DstPort;
	unsigned char Dmac[6];
	unsigned char Smac[6];

}EXTIPV6L2_PATTERN_T;

typedef struct _ACL_EXTIPV6L2_MASK_S_
{
	BOOL maskACLId;
	unsigned long maskDip;
	unsigned long maskSip;
	unsigned int  maskLenDip;
	unsigned int  maskLenSip;
	BOOL maskType;
	BOOL maskCode;
	BOOL maskSrcPort;
	BOOL maskDstPort;
	unsigned char maskDMac[6];
	unsigned char maskSMac[6];
}EXTIPV6L2_MASK_T;

typedef struct _ACL_RULE_EXTIPV6L2_INFO
{
	EXTIPV6L2_PATTERN_T 	rule;
	EXTIPV6L2_MASK_T		mask;
}ACL_RULE_EXTIPV6L2_T;

/*-----EXTIPV6L4-------------*/
typedef struct _ACL_EXTIPV6L4_PATTERN_S_
{
	int ACLId;
	/*BOOL isIpv6;*/
	struct ipv6addr Dip;
	struct ipv6addr Sip;
	unsigned int type;
	unsigned int Code;
	unsigned int SrcPort;
	unsigned int DstPort;

}EXTIPV6L4_PATTERN_T;

typedef struct _ACL_EXTIPV6L4_MASK_S_
{
	int maskACLId;
	/*BOOL maskIsIpv6;*/
	struct ipv6addr maskDip;
	struct ipv6addr maskSip;
	unsigned int  maskLenDip;
	unsigned int  maskLenSip;
	unsigned int maskType;
	unsigned int maskCode;
	unsigned int maskSrcPort;
	unsigned int maskDstPort;
}EXTIPV6L4_MASK_T;

typedef struct _ACL_RULE_EXTIPV6L4_INFO
{
	EXTIPV6L4_PATTERN_T 	rule;
	EXTIPV6L4_MASK_T		mask;
}ACL_RULE_EXTIPV6L4_T;

/*---TCAM-------------*/
 struct ACL_TCAM_STC
{	
	ACL_TCAM_RULE_DIRECTION_E direction;
	ACL_TCAM_RULE_FORMAT_E    ruleFormat;	
	ACL_TCAM_RULE_PACKETTYPE_E packetType;
	unsigned int nextheader;
	union
	{
		ACL_RULE_STDNOTIP_T	stdNotIp;
		ACL_RULE_STDIPL2QOS stdL2Qos;
		ACL_RULE_STDIPV4L4_T stdIpv4L4;
		ACL_RULE_STDIPV6DIP_T stdIpv6;
		ACL_RULE_EXTNOTIPV6_T extNotIpv6;
		ACL_RULE_EXTIPV6L2_T extIpv6L2;
		ACL_RULE_EXTIPV6L4_T extIpv6L4;
	}ruleInfo;	
   
};

struct ACL_ACTION_QOS_STC
{
   QOS_NORMAL_ENABLE_E modifyDscp;
   QOS_NORMAL_ENABLE_E  modifyUp;
   
    union
     {
		struct
		{
			unsigned int 		     profileIndex;
			BOOL	     		     profileAssignIndex;
			QOS_ATTRIBUTE_ASSIGN_PRECEDENCE_E  profilePrecedence;
		}ingress;
		struct
		{
			unsigned int	egrDscp;
			unsigned int    egrUp;
		}egress;
     }qos;
};
typedef struct
{
	unsigned int policerEnable;
	unsigned int policerId;
}POLICER_STC;
/*----append--*/
typedef struct
{
	unsigned char appendType; 
	
}ACL_APPEND_INFO_STC;


/*-------action------------*/
struct  ACL_ACTION_STC
{
	ACL_ACTION_TYPE_E actionType;	
	struct ACTION
	{
		struct ACL_ACTION_REDIRECT_STC			redirect;
    	struct ACL_ACTION_TRAP_TO_CPU			trap;
		struct ACL_ACTION_MIRROR_TO_ANALYZER    mirror;
		struct ACL_ACTION_QOS_STC				qos;	
	}actionInfo;	
	 POLICER_STC		policerPtr;
};

 /*------AccessList-----------*/ 

typedef struct ACCESS_LIST_STC
{
	unsigned long 		ruleIndex;
	unsigned int		 startIndex;
	unsigned int		 endIndex;
	unsigned int 		ruleType;
	unsigned int 		rangerule;
	unsigned  char		timeName[NPD_TIME_RANGE_SIZE];   /*timeRange*/
	ACL_APPEND_INFO_STC			 appendInfo; /*append infos*/
	struct	ACL_ACTION_STC		 ActionPtr;
	struct  ACL_TCAM_STC		 TcamPtr;	
}AclRuleList;


extern unsigned int npd_acl_service(BOOL enable);

void npd_acl_len2mask (int len,unsigned long *mask);

AclRuleList *npd_acl_find_addr_by_ruleIndex
(
	unsigned long index
);

unsigned int npd_acl_group_add_equal_rule
(
	unsigned long index,
	unsigned int groupnum,
	unsigned int groupty
);

unsigned int npd_acl_add
(
	struct ACCESS_LIST_STC *
);
unsigned int npd_acl_add_notIp
(
	struct ACCESS_LIST_STC * 
);

extern unsigned int nam_drv_acl_stdIpv4L4_ListDeal				
(
	struct ACCESS_LIST_STC *,
	unsigned int aclId,
	int unit,
	unsigned int dir_info
);
extern unsigned int nam_drv_acl_extIpv4L4_ListDeal				
(
	struct ACCESS_LIST_STC *,
	unsigned int aclId,
	int unit,
	unsigned int dir_info
);
extern unsigned int nam_drv_acl_stdNotIp_ListDeal
(
	struct ACCESS_LIST_STC *,
	unsigned int aclId,
	int unit,
	unsigned int dir_info
);
extern unsigned int nam_drv_acl_extNotIpv6_ListDeal
(
	struct ACCESS_LIST_STC *,
	unsigned int aclId,
	int unit,
	unsigned int dir_info
);
extern unsigned int nam_drv_ingress_acl_extIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
);

extern unsigned int nam_drv_egress_acl_extIpv6_ListDeal
(
	struct ACCESS_LIST_STC * node,
	unsigned int aclId, 
	int unit
);

unsigned int npd_ip_long2str
(
	unsigned long ipAddress,
	unsigned char **buff
);

unsigned int npd_acl_nodesearch
(
	void
);

unsigned int npd_acl_notIp_display
(
	struct ACCESS_LIST_STC  *
);

unsigned int npd_acl_isIp_display
(
	int ,
	struct ACCESS_LIST_STC  *
);
unsigned int npd_acl_extNotIpv6_display
(
	struct ACCESS_LIST_STC  *
);

unsigned int npd_acl_add_extNotIpv6
(
	struct ACCESS_LIST_STC  *
);

unsigned int npd_acl_delete
(
	unsigned long index,
	unsigned int *num
);


unsigned int npd_drv_bind_acl_group
(
	unsigned char devnum,
	unsigned char virPortNo,
	unsigned int  group_num,
	unsigned int  eth_g_index,
	unsigned int  access_type,
	unsigned short vlanId,
	unsigned int   dir_info,
	unsigned int stdrule
);

unsigned int npd_drv_unbind_acl_group
(
	unsigned char devnum,
	unsigned char virPortNo,
	unsigned int  group_num,
	unsigned int  eth_g_index,
	unsigned int  access_type,
	unsigned short vlanId,
	unsigned int   dir_info,
	unsigned int stdrule
);


unsigned int search_global_acl_rule
(
	unsigned long number
);


unsigned int search_acl_group_rule_index_exist
(	
	unsigned long index,
	unsigned int  *num,
	unsigned int *direction
);

unsigned int delete_acl_eth_rule
(
	unsigned long number
);

unsigned int insert_acl_eth_rule
(
	unsigned char  portId,
	unsigned long index,
	unsigned long aclId
);

unsigned int npd_show_eth_rule
(
	unsigned int eth_g_index
);

unsigned int npd_creat_acl_group
(
	unsigned long    groupnum
);
unsigned int npd_drv_delete_group_acl
(
	unsigned int	 groupnum,
	unsigned int 	 dir_info
);
unsigned int npd_drv_ethport_delete_acl_group
(
	unsigned char devnum,
	unsigned char virPortNo,
	unsigned int  eth_g_index,
	unsigned int  access_type
);
unsigned int npd_show_acl_group(unsigned int dir_info);

struct acl_group_list_s *npd_acl_group_find_addr_by_Index
(
	unsigned long index
);

extern unsigned int nam_acl_drv_tcam_set
(
	unsigned int group_num,
	unsigned int index,
	ACL_TCAM_RULE_FORMAT_E   ruleFormat,
	unsigned int	dir_info
);

extern unsigned int nam_bcm_acl_drv_tcam_set
(
	unsigned int group_num,
	int unit,
	unsigned int *pbmp
);

extern unsigned int nam_acl_drv_tcam_clear
(
	unsigned int index,
	int enable
);

extern unsigned int nam_bcm_acl_drv_tcam_clear
(
	unsigned int group_num,
	int unit
);

extern unsigned int nam_acl_service_init
(
	void
);

extern unsigned int nam_acl_drv_cfg_table_set
(
	unsigned char devNum,
	unsigned char portId,
	unsigned int  aclId,
	unsigned int  flag,
	unsigned int access_type,
	unsigned short vlanId,
	unsigned int  dir_info,
	unsigned int stdrule
);

extern unsigned int nam_acl_port_acl_enable
(
	unsigned char devNum,
	unsigned char  portId,
	unsigned char acl_enable,
	unsigned int  dir_info,
	int serviceenable
);

extern unsigned int nam_bcm_acl_port_acl_enable
(
	unsigned char devNum,
	unsigned char  portId,
	unsigned char acl_enable,
	unsigned int  dir_info,
	int serviceenable
);


extern unsigned int nam_acl_service
(
	unsigned char enable
);

int npd_acl_group_search
(
	unsigned int *group_count
);

extern unsigned int nam_asic_vlan_get_drv_port_bmp
(
 unsigned short vlanId,
 unsigned int *untagBmp,
 unsigned int *tagBmp
);
#if 0
extern unsigned int nam_vlan_convert_port_bitmap_global_index
(
	struct DRV_VLAN_PORT_BMP_S *portbmp,
	unsigned int *eth_g_index,
	unsigned int *port_count
);
#endif 
unsigned int npd_drv_port_acl_enable
(
	unsigned int eth_g_index,
	unsigned int acl_enable,
	unsigned int dir_info
);

 unsigned int npd_acl_service_enable
 (
 	int enable,
 	int init
 );

 unsigned int npd_drv_vlan_acl_enable
 (
	 unsigned int eth_g_index,
	 unsigned int acl_enable,
	 unsigned short vlanId,
	 unsigned int dir_info
 );
 unsigned int npd_vlan_acl_enable
 (
 	unsigned short vlanId,
 	unsigned int acl_enable,
 	unsigned int dir_info
 );
 unsigned int npd_vlan_acl_bind_check
(
	unsigned short vlanId,
	unsigned int    *groupno,
	unsigned int   dir_info

 );
 unsigned int npd_acl_drv_rule_action_check
 (
 	unsigned int ruleIndex
 );

 unsigned int npd_vlan_bind_group
 (
 	unsigned short vlanId,
 	unsigned int group_num,
 	unsigned int dir_info
 );
 unsigned int npd_port_bind_group
 (
 	unsigned int eth_g_index,
 	unsigned int group_num,
 	unsigned int dir_info
 );
 unsigned int npd_acl_rule_index_check
 (
 	unsigned long index,
 	unsigned int type
 );
 unsigned int npd_port_bind_group_check
 (
 	unsigned int eth_g_index,
 	unsigned int *groupNo,
 	unsigned int dir_info
 );
 unsigned int npd_acl_group_bind_check
 (
 	struct acl_group_list_s *nodeGroupPtr
 );
 extern unsigned int eth_port_acl_enable
 (
 	unsigned int eth_g_index,
 	unsigned int acl_enable,
 	unsigned int dir_info
 	
 );
 
 unsigned int npd_vlan_unbind_group
 (	unsigned short vlanId,
 	unsigned int group_num,
 	unsigned int dir_info
 );
 
 extern unsigned int nam_drv_acl_arp_listDeal
 (
 	struct ACCESS_LIST_STC *node,
 	unsigned int aclId,
 	int unit,
 	int port
 );
 
 unsigned int npd_group_bind_check
 (
 	unsigned int dir_info,
 	struct acl_group_list_s *nodeGroupPtr
 );
 
 unsigned int npd_egress_acl_group_bind_check
 (
 	struct acl_group_list_s *nodeGroupPtr
 );
 unsigned int npd_acl_drv_add_rule2group
(
	struct acl_group_list_s *groupInfo,
	unsigned int			add_index,
	unsigned int 			group_num,
	unsigned int 			dir_info
);
 extern unsigned int nam_get_devport_by_slotport
 (
	 IN 	 unsigned int slotNo,
	 IN 	 unsigned int localPortNo,
	 IN 	 unsigned int moduleType,
	 OUT	 unsigned int* devNum,
	 OUT	 unsigned int* virtPortNo
 );
 unsigned int npd_acl_qos_add
 (
 	struct ACCESS_LIST_STC *node
 );
 unsigned int npd_acl_qos_egrAdd
  (
	 struct ACCESS_LIST_STC *node
  );
 unsigned int npd_acl_drv_rule_in_action_check
 (
 	unsigned int ruleIndex
 );
  unsigned int npd_acl_drv_ingress_rule_check
 (
 	unsigned int ruleIndex,
 	unsigned int groupnum
 );
  unsigned int npd_acl_drv_egress_rule_check
 (
 	unsigned int ruleIndex,
 	unsigned int groupnum
 );
 extern unsigned int npd_qos_profile_index_check
 (
 	unsigned int profileIndex
 );
 extern unsigned int npd_qos_profile_in_acl_check
 (
 	unsigned int profileIndex
 );
 unsigned int npd_vlan_acl_enable_check
 (
	 unsigned short vlanId,
	 unsigned int dir_info
 );
 unsigned int npd_acl_policer_id_check(unsigned int policerId);
 unsigned int npd_acl_policer_use_check
 (
 	unsigned int policerId
 );
 #if 1
 extern unsigned int (*npd_get_devport_by_global_index)
 (
	 IN  unsigned int eth_g_index,
	 OUT unsigned char *devNum,
	 OUT unsigned char *portNum
 ); 
 #else
 extern unsigned int npd_get_devport_by_global_index
 (
	 IN  unsigned int eth_g_index,
	 OUT unsigned char *devNum,
	 OUT unsigned char *portNum
 );
 #endif
 extern unsigned int nam_get_slotport_by_devport
 (
	 IN 	 unsigned int devNum,
	 IN 	 unsigned int virtPortNo,
	 IN 	 unsigned int productId,
	 OUT	 unsigned int* slotNo,
	 OUT	 unsigned int* localPortNo
 );
extern unsigned int nam_qos_queue_init();
int npd_acl_time_name_check
(
	char *Name,
	unsigned int *timeId
);
int npd_acl_time_activate
(
	 char *timeName,
     unsigned int *timeId
);

int npd_acl_time_period_check
(
	unsigned int timeId,
	unsigned int period
);

extern unsigned int nam_bcm_acl_support_check
(
	void
);


extern unsigned int nam_acl_qos_action_update(unsigned int ruleIndex);
extern unsigned int nam_acl_qos_action_clear(unsigned int ruleIndex);

extern unsigned int npd_acl_drv_time_rule_in_action_check(unsigned int ruleIndex);
extern unsigned int nam_acl_service_get(unsigned int *enable);
extern int npd_mirror_src_acl_check
(
	unsigned int profile,
	unsigned int  ruleIndex
);
extern unsigned int  nam_acl_time_action_update(unsigned int ruleIndex);

#endif
