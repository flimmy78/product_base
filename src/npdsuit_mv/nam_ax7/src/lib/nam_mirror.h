#ifndef __NAM_MIRROR_H__
#define __NAM_MIRROR_H__

typedef struct {
    unsigned char arEther[6];
}ETHERADDR;
/*
typedef unsigned short GT_TRUNK_ID;
#define NAM_TRUE 0x1
#define NAM_FALSE 0x0

typedef enum
{
    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E = 0,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E
} CPSS_MAC_ENTRY_EXT_TYPE_ENT;


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


typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        unsigned char   devNum;
        unsigned char   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    unsigned short       vidx;
    unsigned short       vlanId;
#ifdef DRV_LIB_CPSS_3_4
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



typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       unsigned int 				isStatic;
       CPSS_MAC_TABLE_CMD_ENT       daCommand;
       CPSS_MAC_TABLE_CMD_ENT       saCommand;
       unsigned int					daRoute;
       unsigned int					mirrorToRxAnalyzerPortEn;
       unsigned int                 sourceID;
       unsigned int                 userDefined;
       unsigned int                 daQosIndex;
       unsigned int                 saQosIndex;
       unsigned int                 daSecurityLevel;
       unsigned int                 saSecurityLevel;
       unsigned int					appSpecificCpuCode;
#ifdef DRV_LIB_CPSS_3_4
	   unsigned int 				 age;
#endif
} CPSS_MAC_ENTRY_EXT_STC;

*/

void nam_mirror_init
(
	void
);

unsigned int nam_mirror_analyzer_port_set
(
	unsigned int profile,
	unsigned char analyzerDev,
	unsigned char analyzerPort,
	unsigned int    direct
	
);

unsigned int nam_mirror_fdb_entry_set
(
    unsigned short vlanid,
    unsigned int global_index,
    ETHERADDR *macAddr,
    unsigned int flag
);

unsigned int nam_mirror_port_set
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned int  direct,
	unsigned int profile
);
unsigned int nam_acl_mirror_action_update
(
	unsigned int ruleIndex,
	unsigned int enable,
	int unit
);

unsigned int nam_mirror_vlan_set
(
	unsigned short vid,
	unsigned int enable
);

unsigned int profile_check(unsigned int profile);

unsigned int nam_mirror_analyzer_port_set_for_bcm
(
    unsigned char srcDev,
    unsigned char srcPort,
    unsigned char analyzerDev,
	unsigned char analyzerPort,
	unsigned int  direct
	
);

unsigned int nam_mirror_vlan_set_for_bcm
(
	unsigned short vid,
	unsigned char destDev,
	unsigned char destPort,
	unsigned int enable
);


#ifdef DRV_LIB_CPSS
/*
extern unsigned long cpssDxChMirrorRxAnalyzerPortSet
(
   unsigned char    dev,
   unsigned char    analyzerPort,
    unsigned char   analyzerdev
);

extern unsigned long cpssDxChMirrorTxAnalyzerPortSet
(
   unsigned char    dev,
   unsigned char    analyzerPort,
   unsigned char   analyzerdev
);

extern unsigned long cpssDxChMirrorRxPortSet
(
    unsigned char    dev,
    unsigned char    mirrPort,
    unsigned int  enable
);

extern unsigned long cpssDxChMirrorTxPortSet
(
    unsigned char    dev,
    unsigned char    mirrPort,
    unsigned int  enable
);

extern unsigned long cpssDxChBrgVlanIngressMirrorEnable
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned int enable
);



extern unsigned long cpssDxChBrgFdbHashCalc
(   unsigned char devNum,
    CPSS_MAC_ENTRY_EXT_KEY_STC * macEntryKeyPtr,
    unsigned int  * hashPtr
);

extern unsigned long cpssDxChBrgFdbMacEntryRead
(   unsigned char devNum,
    unsigned int index,
    unsigned int * validPtr,
    unsigned int * skipPtr,
    unsigned int  * agedPtr,
    unsigned char * associatedDevNumPtr,
    CPSS_MAC_ENTRY_EXT_STC * entryPtr
);
extern unsigned long cpssDxChBrgFdbMacEntryWrite
(   
    unsigned char devNum,
    unsigned int index,
    unsigned long  skip,
    CPSS_MAC_ENTRY_EXT_STC * macEntryPtr
);



extern unsigned long cpssDxChMirrorTxAnalyzerPortGet
(
    unsigned char    dev,
    unsigned char  * analyzerPortPtr,
    unsigned char  * analyzerDevPtr
);


extern unsigned long cpssDxChMirrorRxAnalyzerPortGet
(
    unsigned char dev,
    unsigned char * analyzerPortPtr,
    unsigned char * analyzerDevPtr
);
*/
#endif
#if 1
extern unsigned int (*npd_get_global_index_by_devport)
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int * eth_g_index
);
extern unsigned int (*npd_get_devport_by_global_index)
(
    unsigned int eth_g_index,
    unsigned char * devNum,
    unsigned char * portNum
);
#else
extern unsigned int npd_get_global_index_by_devport
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int * eth_g_index
);
extern unsigned int npd_get_devport_by_global_index
(
    unsigned int eth_g_index,
    unsigned char * devNum,
    unsigned char * portNum
);
#endif
#endif
