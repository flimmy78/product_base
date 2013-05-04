#ifndef __NAM_TRUNK_H__
#define __NAM_TRUNK_H__

#define TRUNK_CONFIG_SUCCESS	0x0
#define	TRUNK_CONFIG_FAIL		0xff
#define TRUNK_PORT_EXISTS_GTDB	0xf		/*port already exists in trunkDB*/
#define TRUNK_PORT_MBRS_FULL	(TRUNK_PORT_EXISTS_GTDB+1)
#define TRUNK_MEMBER_NUM_MAX	0x8

#define NPD_TRUNK_ERR_NONE		NPD_SUCCESS
#define NPD_TRUNK_UNSUPPORT     (NPD_TRUNK_ERR_NONE + 47)
#define BCM_RTAG_SRCDSTMAC      BCM_TRUNK_PSC_SRCDSTMAC  
#define BCM_RTAG_SRCDSTIP       BCM_TRUNK_PSC_SRCDSTIP

#define BCM_TRUNK_PSC_SRCDSTMAC 3          /* Source+dest MAC address. */
#define BCM_TRUNK_PSC_SRCDSTIP  6          /* Source+dest IP address. */

/*extern cpss-APIs */
typedef struct {
	unsigned char portNum;
	unsigned char devNum;
}CPSS_TRUNK_MEMBER_STC;
typedef struct{
    unsigned int   ports[2];
}CPSS_PORTS_BMP_STC;

struct DRV_VLAN_PORT_BMP_S {
    unsigned int   ports[2];
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

typedef enum {
    CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E,
    CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E
}CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT;

typedef enum {
    CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E,
    CPSS_DXCH_TRUNK_L4_LBH_LONG_E,
    CPSS_DXCH_TRUNK_L4_LBH_SHORT_E
}CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT;

typedef enum
{
    CPSS_CSCD_LINK_TYPE_TRUNK_E,
    CPSS_CSCD_LINK_TYPE_PORT_E
} CPSS_CSCD_LINK_TYPE_ENT;


typedef struct
{
    unsigned long   linkNum;
    CPSS_CSCD_LINK_TYPE_ENT  linkType;
} CPSS_CSCD_LINK_TYPE_STC;

typedef enum
{
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E
} CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT;

#ifdef DRV_LIB_CPSS

extern unsigned long cpssDxChTrunkMembersSet
(
    unsigned char		devNum,
    unsigned short		trunkId,
    unsigned int		numOfEnabledMembers,
    CPSS_TRUNK_MEMBER_STC	enabledMembersArray[],
    unsigned int		numOfDisabledMembers,
    CPSS_TRUNK_MEMBER_STC	disabledMembersArray[]
);
extern unsigned long cpssDxChTrunkMemberAdd
(
    unsigned char		devNum,
    unsigned short		trunkId,
    CPSS_TRUNK_MEMBER_STC    *memberPtr
);

extern unsigned long cpssDxChTrunkMemberRemove
(
    unsigned char		devNum,
    unsigned short		trunkId,
    CPSS_TRUNK_MEMBER_STC    *memberPtr
);
extern unsigned long cpssDxChTrunkMemberDisable
(
    unsigned char		devNum,
    unsigned short		trunkId,
    CPSS_TRUNK_MEMBER_STC    *memberPtr
);
extern unsigned long cpssDxChTrunkMemberEnable
(
    unsigned char		devNum,
    unsigned short		trunkId,
    CPSS_TRUNK_MEMBER_STC    *memberPtr
);
extern unsigned long cpssDxChTrunkDbEnabledMembersGet
(
    unsigned char		devNum,
    unsigned short		trunkId,
    unsigned int		*numOfEnabledMembersPtr,
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
);
extern unsigned long cpssDxChTrunkDbDisabledMembersGet
(
    unsigned char		devNum,
    unsigned short		trunkId,
    unsigned int		*numOfEnabledMembersPtr,
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
);
extern unsigned long cpssDxChTrunkDbIsMemberOfTrunk
(
    unsigned char				devNum,
    CPSS_TRUNK_MEMBER_STC		*memberPtr,
    unsigned short				*trunkIdPtr
);

extern unsigned long trunkDbNonTrunkPortsCalc
(
    unsigned char                       devNum,
    unsigned short                      trunkId,
    CPSS_PORTS_BMP_STC          *nonTrunkPortsPtr
);

extern unsigned long cpssDxChTrunkHashGlobalModeSet
(
    unsigned char	devNum,
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
);
extern unsigned long cpssDxChTrunkHashIpAddMacModeSet
(
    unsigned char	devNum,
    unsigned int   enable
);
extern unsigned long cpssDxChTrunkHashIpModeSet
(
    unsigned char   devNum,
    unsigned int	enable
);

extern unsigned long cpssDxChTrunkHashL4ModeSet
(
    unsigned char	devNum,
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT hashMode
);

extern unsigned long cpssDxChCscdDevMapTableSet
(
    unsigned char devNum,
    unsigned char targetDevNum,
    unsigned char targetPortNum,
    CPSS_CSCD_LINK_TYPE_STC *cascadeLinkPtr,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
); 

extern unsigned long cpssDxChCscdDevMapTableGet
(
    unsigned char devNum,
    unsigned char targetDevNum,
    unsigned char targetPortNum,   
    CPSS_CSCD_LINK_TYPE_STC *cascadeLinkPtr,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
);

#endif
extern unsigned int nam_asic_convert_mbrbmp_slotportbmp
(
	struct DRV_VLAN_PORT_BMP_S mbrBmp,
	unsigned int *mbrSlotPortBmp
);

unsigned int nam_asic_trunk_entry_active
(
	unsigned short trunkId
);

/*unsigned int nam_asic_trunk_ports_add
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum
);*/

unsigned int nam_asic_trunk_ports_add
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum,
	unsigned char enDis
);


unsigned int nam_asic_trunk_ports_del
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned char portNum
);

unsigned int nam_asic_trunk_delete
(
	unsigned short trunkId
);
unsigned int nam_asic_trunk_load_balanc_set
(
	unsigned char devNum,
	unsigned short trunkId,
	unsigned int  lbMode
);
unsigned int nam_asic_trunk_get_port_member
(
	unsigned short	trunkId,
	unsigned int* 	memberCont,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
unsigned int nam_asic_trunk_get_port_member_bmp
(
	unsigned short	trunkId,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
#endif
