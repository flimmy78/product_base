#ifndef __NAM_FDB_H__
#define __NAM_FDB_H__

#define FDB_CONFIG_NOEXST   3
#define FDB_CONFIG_EXST      2
#define FDB_CONFIG_FAIL		1
#define FDB_CONFIG_SUCCESS	0
#define SYSTEM_STATIC_LEN   4
#define TRUE	1
#define FALSE	0
#define FDB_INIT 0
#define TEST_VALUE 2
#define FDB_MAX_HASH_LEN     32
#define L2_ADDR_NUM_MAX      16384

#ifdef DRV_LIB_CPSS
#define NAM_CONVERT_DEV_PORT_MAC(devNum, portNum) \
{                                             \
   gtPortsDevConvert(&devNum, &portNum);      \
}

#define FDB_MAX_HASH_LEN     32
#define NAM_CONVERT_BACK_DEV_PORT_MAC(devNum, portNum) \
{                                                  \
   gtPortsDevConvertBack(&devNum, &portNum);       \
}
#endif
#define BCM_L2_COS_SRC_PRI              0x00000001 /* Source COS has priority
                                                      over destination COS. */
#define BCM_L2_DISCARD_SRC              0x00000002 
#define BCM_L2_DISCARD_DST              0x00000004 
#define BCM_L2_COPY_TO_CPU              0x00000008 
#define BCM_L2_L3LOOKUP                 0x00000010 
#define BCM_L2_STATIC                   0x00000020 
#define BCM_L2_HIT                      0x00000040 
#define BCM_L2_TRUNK_MEMBER             0x00000080 
#define BCM_L2_MCAST                    0x00000100 
#define BCM_L2_REPLACE_DYNAMIC          0x00000200 
#define BCM_L2_SRC_HIT                  0x00000400 
#define BCM_L2_DES_HIT                  0x00000800 
#define BCM_L2_REMOTE_TRUNK             0x00001000 
#define BCM_L2_MIRROR                   0x00002000 
#define BCM_L2_SETPRI                   0x00004000 
#define BCM_L2_REMOTE_LOOKUP            0x00008000 
#define BCM_L2_NATIVE                   0x00010000 
#define BCM_L2_MOVE                     0x00020000 
#define BCM_L2_FROM_NATIVE              0x00040000 
#define BCM_L2_TO_NATIVE                0x00080000 
#define BCM_L2_MOVE_PORT                0x00100000 
#define BCM_L2_LOCAL_CPU                0x00200000 /* Entry is for the local CPU
                                                      on the device. */
#define BCM_L2_USE_FABRIC_DISTRIBUTION  0x00400000 /* Use specified fabric
                                                      distribution class. */
#define BCM_L2_PENDING                  0x00800000 

/* Flags for L2 learn limit. */
#define NAM_L2_LEARN_LIMIT_SYSTEM           0x00000001 /* Limit is system wide. */
#define NAM_L2_LEARN_LIMIT_VLAN             0x00000002 /* Limit is on per VLAN
                                                          basis. */
#define NAM_L2_LEARN_LIMIT_PORT             0x00000004 /* Limit is on per port
                                                          basis. */
#define NAM_L2_LEARN_LIMIT_TRUNK            0x00000008 /* Limit is on per trunk
                                                          basis. */
#define NAM_L2_LEARN_LIMIT_ACTION_DROP      0x00000010 /* Drop if over limit. */
#define NAM_L2_LEARN_LIMIT_ACTION_CPU       0x00000020 /* Send to CPU if over
                                                          limit. */
#define NAM_L2_LEARN_LIMIT_ACTION_PREFER    0x00000040 /* Use system drop/CPU if
                                                          over both system limit
                                                          and non-system limit,
                                                          used by system wide
                                                          setting only. */

#define BCM_L2_DELETE_STATIC    0x00000001 
#define BCM_L2_DELETE_PENDING   0x00000002 


#define NAM_RV_REPLACE_OK(tmp_rv, rv, ok_rv) \
    if ((tmp_rv) < 0 && (tmp_rv) != (ok_rv)) (rv) = (tmp_rv)

	
typedef enum nam_error_e {
	NAM_E_NONE = 0, 
	NAM_E_INTERNAL = -1, 
	NAM_E_MEMORY = -2,
	NAM_E_UNIT = -3, 
	NAM_E_PARAM = -4, 
	NAM_E_EMPTY = -5, 
	NAM_E_FULL = -6, 
	NAM_E_NOT_FOUND = -7, 
	NAM_E_EXISTS = -8, 
	NAM_E_TIMEOUT = -9, 
	NAM_E_BUSY = -10, 
	NAM_E_FAIL = -11, 
	NAM_E_DISABLED = -12, 
	NAM_E_BADID = -13, 
	NAM_E_RESOURCE = -14, 
	NAM_E_CONFIG = -15, 
	NAM_E_UNAVAIL = -16, 
	NAM_E_INIT = -17, 
	NAM_E_PORT = -18 
} nam_error_t;


typedef unsigned char eth_mac_t[6];

typedef struct nam_learn_limit_s {
 unsigned int  flags;		 /* BCM_L2_LEARN_LIMIT_xxx actions and qualifiers. */
 unsigned short vlan;	 /* VLAN identifier. */
 int port;	 /* Port number. */
 int trunk;  /* Trunk identifier. */
 int limit; 		 /* Maximum number of learned entries, -1 for unlimited. */
} nam_learn_limit_t;

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
	 unsigned int        asic_num;
	 unsigned int        slot_id;
}NPD_FDB_DBG;

typedef enum
{
   CPSS_MAC_HASH_FUNC_XOR_E,
   CPSS_MAC_HASH_FUNC_CRC_E
} CPSS_MAC_HASH_FUNC_MODE_ENT;

/* Values returned by bcm_mcast_join and bcm_mcast_leave routines. */
#define NAM_MCAST_JOIN_ADDED        1          
#define NAM_MCAST_JOIN_UPDATED      2          
#define NAM_MCAST_LEAVE_DELETED     3          
#define NAM_MCAST_LEAVE_UPDATED     4          
#define NAM_MCAST_LEAVE_NOTFOUND    5  

typedef enum
{
    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E = 0,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E
} CPSS_MAC_ENTRY_EXT_TYPE_ENT;

typedef enum
{
    CPSS_ACT_AUTO_E = 0,
    CPSS_ACT_TRIG_E

} CPSS_MAC_ACTION_MODE_ENT;

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

typedef enum
{
    NAM_FALSE = 0,
    NAM_TRUE  = 1
} NAM_BOOL;

typedef enum
{
    CPSS_NA_E = 0,
    CPSS_QA_E,
    CPSS_QR_E,
    CPSS_AA_E,
    CPSS_TA_E,
    CPSS_SA_E,
    CPSS_QI_E,
    CPSS_FU_E

} CPSS_UPD_MSG_TYPE_ENT;
#ifdef DRV_LIB_CPSS_3_4
typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       NAM_BOOL                      isStatic;
       CPSS_MAC_TABLE_CMD_ENT       daCommand;
       CPSS_MAC_TABLE_CMD_ENT       saCommand;
       NAM_BOOL                      daRoute;
       NAM_BOOL                      mirrorToRxAnalyzerPortEn;
       unsigned int                       sourceID;
       unsigned int                       userDefined;
       unsigned int                       daQosIndex;
       unsigned int                       saQosIndex;
       unsigned int                       daSecurityLevel;
       unsigned int                       saSecurityLevel;
       NAM_BOOL                      appSpecificCpuCode;
       NAM_BOOL                      age;
       NAM_BOOL                      spUnknown;
} CPSS_MAC_ENTRY_EXT_STC;
#else
typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       NAM_BOOL						isStatic;
       CPSS_MAC_TABLE_CMD_ENT       daCommand;
       CPSS_MAC_TABLE_CMD_ENT       saCommand;
       NAM_BOOL						daRoute;
       NAM_BOOL						mirrorToRxAnalyzerPortEn;
       unsigned int                 sourceID;
       unsigned int                 userDefined;
       unsigned int                 daQosIndex;
       unsigned int                 saQosIndex;
       unsigned int                 daSecurityLevel;
       unsigned int                 saSecurityLevel;
       NAM_BOOL						appSpecificCpuCode;
#ifdef DRV_LIB_CPSS_3_4
	   unsigned int					age;
       unsigned int                 spUnknown;     /* add for show fdb count error! zhagndi 2011-6-11 */
#endif
} CPSS_MAC_ENTRY_EXT_STC;
#endif



typedef struct
{
    CPSS_UPD_MSG_TYPE_ENT       updType;
    NAM_BOOL                    entryWasFound;
    unsigned int                macEntryIndex;
    CPSS_MAC_ENTRY_EXT_STC      macEntry;
    NAM_BOOL                    skip;
    NAM_BOOL                    aging;
    unsigned char               associatedDevNum;
    unsigned char               queryDevNum;
    NAM_BOOL                    naChainIsTooLong;
    unsigned int                entryOffset;
} CPSS_MAC_UPDATE_MSG_EXT_STC;


typedef enum{
    CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E,
    CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E,
    CPSS_FDB_ACTION_DELETING_E,
    CPSS_FDB_ACTION_TRANSPLANTING_E
} CPSS_FDB_ACTION_MODE_ENT;

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
} CPSS_BRG_IPM_MODE_ENT;

typedef enum
{
    CPSS_LOCK_FRWRD_E = 1,
    CPSS_LOCK_DROP_E,
    CPSS_LOCK_TRAP_E,
    CPSS_LOCK_MIRROR_E,
    CPSS_LOCK_SOFT_DROP_E
} CPSS_PORT_LOCK_CMD_ENT;


int nam_fdb_table_delete_entry_with_port(unsigned int index);

int nam_fdb_table_delete_entry_with_vlan(unsigned short vlanid);

int nam_fdb_table_agingtime_set(unsigned int timeout);

 int nam_fdb_table_agingtime_get(unsigned int*	timeout);
 
 int nam_static_fdb_entry_mac_set_for_system
 (
	 unsigned short vlanId,
	 unsigned int flag
 ) ;
 int nam_static_fdb_entry_mac_vlan_port_set
 (
	 unsigned char macAddr[],
	 unsigned short vlanId,
	 unsigned int globle_index
 ) ;

 int nam_no_static_fdb_entry_mac_vlan_set
(
	unsigned char * macAddr,
	unsigned short vlanId
) ;
unsigned int nam_static_fdb_entry_mac_vlan_trunk_set
(
	 unsigned char macAddr[],
	 unsigned short vlanId,
	 unsigned short trunkId
);

 int nam_fdb_entry_mac_vlan_drop
(
	unsigned char  macAddr[],
	unsigned short vlanId,
	unsigned int flag,
	unsigned int *hash_index
) ;


 int nam_fdb_entry_mac_vlan_no_drop
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int  flag 
) ;

unsigned int  nam_asic_fdb_aa_msg_enable(unsigned int dev,unsigned int status);
 unsigned int nam_show_fdb_port(NPD_FDB *fdb ,unsigned int size,unsigned int port );
 
 unsigned int nam_show_fdb_vlan(NPD_FDB *fdb ,unsigned int size,unsigned short vlan );

 unsigned int nam_show_fdb_mac(NPD_FDB * fdb,unsigned char macAddr [ 6 ],unsigned int size);

 unsigned int nam_show_fdb_one(NPD_FDB *fdb ,unsigned char macAddr [ 6 ],unsigned short vlanId);
 unsigned int nam_show_fdb (NPD_FDB *fdb,unsigned int size);
 unsigned int nam_show_fdb_count();
 unsigned int nam_show_static_fdb (NPD_FDB *fdb,unsigned int size);
 unsigned int nam_fdb_port_learn_status_set(unsigned char port, unsigned char devNum,unsigned int status);
 unsigned int nam_fdb_na_msg_vlan_set(unsigned short vlanId,unsigned int status);
 int nam_fdb_table_delete_entry_with_trunk(unsigned short trunk_no);
 unsigned int nam_fdb_static_table_delete_entry_with_vlan
 (
	 unsigned short vlanId
 );
 unsigned int nam_fdb_static_table_delete_entry_with_port
 (
	 unsigned int eth_g_index
 );


unsigned int nam_fdb_item_read_from_na
(
     CPSS_MAC_UPDATE_MSG_EXT_STC  *auMessagesPtr
);

unsigned int nam_fdb_item_write_from_na
(
     CPSS_MAC_UPDATE_MSG_EXT_STC  *auMessagesPtr
);

 int nam_static_fdb_entry_mac_set_for_l3
(
	/*unsigned char devNum,*/
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int globle_index
) ;
int nam_fdb_set_port_status
(
	unsigned char	devNum,
	unsigned char	portNum,
	unsigned long	portAttr
);
unsigned int nam_del_fdb_port
(
	unsigned int eth_g_index
);

unsigned int nam_del_fdb_vlan
(
	unsigned short vlanid
);

unsigned int nam_del_fdb_vlan_port
(
	unsigned short vlanid,
	unsigned int  eth_g_index
);

#ifdef DRV_LIB_CPSS
/* external variables reference*/
extern unsigned int g_cpu_port_index;
extern unsigned int g_spi_port_index;


/* external function declearation*/
extern unsigned int cpssDxChBrgFdbAuMsgBlockGet
(
    unsigned char                      devNum,
    unsigned int                       *numOfAuPtr,
    CPSS_MAC_UPDATE_MSG_EXT_STC        *auMessagesPtr
);
extern void gtPortsDevConvertBack
(
   unsigned char *devNum,
   unsigned char *portNum
);


extern unsigned long  cpssDxChBrgFdbInit
(
      unsigned char   devNum
);

extern unsigned long cpssDxChBrgFdbMacEntryWrite
(
    unsigned char	devNum,
    unsigned int	index,
    NAM_BOOL			skip,
    CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

extern unsigned long cpssDxChBrgStaticFdbMacEntrySet

(
    unsigned char devNum,
    unsigned char mac[],
    unsigned short int vlanId,
    unsigned char  dstDev,
    unsigned char  dstPort,
    unsigned char daRoute
);

extern	unsigned long cpssDxChBrgFdbMacEntrySet
(
    unsigned char  				 devNum,
    CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

extern unsigned long cpssDxChBrgFdbAgingTimeoutGet
(
    unsigned char  devNum,
    unsigned int *timeoutPtr
);

extern unsigned long cpssDxChBrgFdbAgingTimeoutSet
(
	unsigned char  devNum,
	unsigned int  timeout
);

 extern unsigned long cpssDxChBrgFdbMaxLookupLenGet
(
     unsigned char    devNum, 
     unsigned int   *lookupLenPtr
);

 extern unsigned long cpssDxChBrgFdbHashCalc
 (
	 unsigned char			 devNum,
	 CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr,
	 unsigned int 		 *hashPtr
 );

 
 extern unsigned long  cpssDxChBrgFdbMacEntryRead
(
    unsigned char              devNum,
    unsigned int               index,
    unsigned int              *validPtr,
    unsigned int              *skipPtr,
    NAM_BOOL                  *agedPtr,
    unsigned char             *associatedDevNumPtr,
    CPSS_MAC_ENTRY_EXT_STC    *entryPtr
);

extern void gtPortsDevConvert
(
   unsigned char *devNum,
   unsigned char *portNum
);

extern unsigned long cpssDxChBrgStaticFdbMacEntryWrite

(
    unsigned char devNum,
    unsigned char mac[],
    unsigned short vlanId,
    unsigned char  dstDev,
    unsigned char  dstPort,
    NAM_BOOL        skip,
    unsigned long hashPtr
);

#if 1
extern unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);

extern unsigned int (*npd_get_global_index_by_devport)
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int *eth_g_index
);
#else
extern unsigned int npd_get_devport_by_global_index
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);

extern unsigned int npd_get_global_index_by_devport
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int *eth_g_index
);
#endif

extern enum product_id_e npd_query_product_id
(
);
extern unsigned int cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    unsigned char    devNum,
     NAM_BOOL ageOutAllDevOnNonTrunkEnable
);

extern unsigned int cpssDxChBrgFdbActionsEnableSet
(
    unsigned char    devNum,
    NAM_BOOL enable
);
extern unsigned int cpssDxChBrgFdbMacTriggerModeSet
(
   unsigned char                       devNum,
   NAM_BOOL                   mode
);
/*Sets FDB action mode without setting Action Trigger*/
extern unsigned int cpssDxChBrgFdbActionModeSet
(
    unsigned char                     devNum,
    CPSS_FDB_ACTION_MODE_ENT     mode
);
extern unsigned int cpssDxChBrgFdbAAandTAToCpuSet
(
    unsigned char                       devNum,
    NAM_BOOL                            enable
);

extern unsigned int cpssDxChBrgFdbStaticDelEnable
(
   unsigned char    devNum,
   NAM_BOOL         enable
);
extern unsigned int cpssDxChBrgFdbActionActiveVlanSet
(
    unsigned char   devNum,
    unsigned short   vlanId,
    unsigned short   vlanMask
);
extern unsigned int cpssDxChBrgFdbActionActiveInterfaceSet
(
    unsigned char devNum,
    unsigned int  actIsTrunk,
    unsigned int  actIsTrunkMask,
    unsigned int  actTrunkPort,
    unsigned int  actTrunkPortMask
);
extern unsigned int cpssDxChBrgFdbActionActiveDevSet
(
    unsigned char  devNum,
    unsigned int   actDev,
    unsigned int   actDevMask
);
extern unsigned int cpssDxChBrgFdbTrigActionStart
(
   unsigned char                       devNum,
   CPSS_FDB_ACTION_MODE_ENT             mode
);
extern unsigned int cpssDxChBrgFdbMacEntryInvalidate
(
    unsigned char       devNum,
    unsigned int        index
);
extern unsigned int cpssDxChBrgFdbTrigActionStatusGet
(
    unsigned char   devNum,
    NAM_BOOL        *actFinishedPtr
);
extern unsigned int cpssDxChBrgVlanIpmBridgingEnable
(
    unsigned char                       devNum,
    unsigned short                      vlanId,
    CPSS_IP_PROTOCOL_STACK_ENT          ipVer,
    NAM_BOOL                            enable
);

extern unsigned int cpssDxChBrgVlanIpmBridgingModeSet
(
    unsigned char                       devNum,
    unsigned short                      vlanId,
    CPSS_IP_PROTOCOL_STACK_ENT          ipVer,
    CPSS_BRG_IPM_MODE_ENT               ipmMode
);
extern unsigned int cpssDxChBrgFdbActionsEnableGet
(
    unsigned char    devNum,
    NAM_BOOL *enablePtr
);
extern unsigned int cpssDxChBrgFdbPortLearnStatusSet
(
    unsigned char                     devNum,
    unsigned char                     port,
    NAM_BOOL                          status,
    CPSS_PORT_LOCK_CMD_ENT            cmd
);

extern unsigned int osTimerWkAfter
(
    unsigned int  time
);

extern unsigned int npd_fdb_reset_fdbCount_by_port
(
    unsigned int slot_no,
    unsigned int port_no
);
extern unsigned int npd_fdb_reset_fdbCount_by_vlan
(
    unsigned short vlanId
);
extern unsigned int npd_get_slot_index_from_eth_index
(
    unsigned int eth_index
);

extern unsigned int npd_get_port_index_from_eth_index
(
    unsigned int eth_index
);
extern unsigned int  npd_get_slot_no_from_index
(
    unsigned int slot_index
);
extern unsigned int  npd_get_port_no_from_index
(
    unsigned int slot_index,
    unsigned int port_index
);
extern unsigned int cpssDxChBrgFdbAAandTAToCpuGet
(
    unsigned char devNum,
    unsigned int * enablePtr
);

extern unsigned int cpssDxChBrgFdbActionActiveDevGet
(
    unsigned char devNum,
    unsigned int * actDevPtr,
    unsigned int * actDevMaskPtr
);

extern unsigned int cpssDxChBrgFdbActionActiveInterfaceGet
(
    unsigned char devNum,
    unsigned int * actIsTrunkPtr,
    unsigned int * actIsTrunkMaskPtr,
    unsigned int * actTrunkPortPtr,
    unsigned int * actTrunkPortMaskPtr
);

extern unsigned int cpssDxChBrgFdbActionActiveVlanGet
(
    unsigned char devNum,
    unsigned short * vlanIdPtr,
    unsigned short * vlanMaskPtr
);

extern unsigned int cpssDxChBrgFdbActionModeGet
(
    unsigned char devNum,
    CPSS_FDB_ACTION_MODE_ENT * modePtr
);

extern unsigned int cpssDxChBrgFdbMacTriggerModeGet
(
    unsigned char devNum,
    CPSS_MAC_ACTION_MODE_ENT * modePtr
);

extern unsigned int cpssDxChPortEnableGet
(
     unsigned char devNum,
     unsigned char  portNum,
     unsigned int * statePtr
);

extern unsigned int cpssDxChPortEnableSet
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned int  enable
);

extern struct trunk_s* npd_find_trunk
(
   unsigned short trunkId
);

extern int npd_system_verify_basemac
(
    char * macAddr
);

extern unsigned int cpssDxChBrgFdbPortLearnStatusGet
(
     unsigned char devNum,
     unsigned char  port,
     unsigned int  * statusPtr,
     CPSS_PORT_LOCK_CMD_ENT * cmdPtr
);

extern unsigned int cpssDxChBrgFdbNaToCpuPerPortSet
(
     unsigned char devNum,
     unsigned char port,
     unsigned int  enable
);

extern unsigned int cpssDxChBrgFdbNaToCpuPerPortGet
(
     unsigned char devNum,
     unsigned char port,
     unsigned int * enablePtr
);

extern unsigned int cpssDxChBrgVlanNaToCpuEnable
(
     unsigned char devNum,
     unsigned short vlanId,
     unsigned int  enable
);
extern unsigned int npd_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
);
extern void npd_syslog_err
(
	char *format,
	...
);
#endif


#ifdef DRV_LIB_BCM
extern unsigned long nam_asic_get_instance_num(void);

extern int bcm_l2_age_timer_set(int unit,int age_seconds);

extern int bcm_l2_addr_delete_by_vlan(int unit,unsigned short vid,unsigned int  flags);

extern int bcm_l2_addr_delete_by_port(int unit,int mod,int port,unsigned int  flags);

extern int nbm_probe_chassis_module_id(unsigned char slotno);

extern int bcm_l2_addr_delete_by_trunk(int unit,int tid,unsigned int  flags);

extern int bcm_l2_addr_delete_by_vlan_trunk(int unit,unsigned short vid,int tid,unsigned int  flags);

extern int bcm_l2_age_timer_get(int unit,int * age_seconds);

extern int bcm_l2_addr_delete_by_vlan_port(int unit,unsigned short vid,int mod,int port,unsigned int flags);

#endif


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
extern unsigned int npd_product_check_spi_port
(
	unsigned char devNum,
	unsigned char portNum
);

#ifndef AX_PLATFORM_DISTRIBUTED
/* Add new entry for static FDB  */
int nam_static_fdb_entry_mac_vlan_port_set_distributed
(
	unsigned char macAddr[],
	unsigned short vlanId,
	unsigned int devNum,
	unsigned int portNum
); 

/* for show debug fdb */
unsigned int nam_show_fdb_debug 
(
	NPD_FDB_DBG *fdb,
	unsigned int size
);
#endif

#endif

