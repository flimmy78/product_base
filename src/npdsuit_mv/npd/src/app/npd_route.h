#ifndef __NPD_ROUTE_H__
#define __NPD_ROUTE_H__

#define IN
#define OUT
#define INOUT
#define GT_IPADDR unsigned long 

#define BUFLENGTH (4096)


typedef unsigned long GT_BOOL;
typedef unsigned long GT_STATUS;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;

/***** generic return codes **********************************/
#define GT_ERROR           (-1)
#define GT_OK              (0x00)   /* Operation succeeded */
#define GT_FAIL            (0x01)   /* Operation failed    */

#define GT_BAD_VALUE       (0x02)   /* Illegal value        */
#define GT_OUT_OF_RANGE    (0x03)   /* Value is out of range*/
#define GT_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#define GT_BAD_PTR         (0x05)   /* Illegal pointer value                 */
#define GT_BAD_SIZE        (0x06)   /* Illegal size                          */
#define GT_BAD_STATE       (0x07)   /* Illegal state of state machine        */
#define GT_SET_ERROR       (0x08)   /* Set operation failed                  */
#define GT_GET_ERROR       (0x09)   /* Get operation failed                  */
#define GT_CREATE_ERROR    (0x0A)   /* Fail while creating an item           */
#define GT_NOT_FOUND       (0x0B)   /* Item not found                        */
#define GT_NO_MORE         (0x0C)   /* No more items found                   */
#define GT_NO_SUCH         (0x0D)   /* No such item                          */
#define GT_TIMEOUT         (0x0E)   /* Time Out                              */
#define GT_NO_CHANGE       (0x0F)   /* The parameter is already in this value*/
#define GT_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#define GT_NOT_IMPLEMENTED (0x11)   /* This request is not implemented       */
#define GT_NOT_INITIALIZED (0x12)   /* The item is not initialized           */
#define GT_NO_RESOURCE     (0x13)   /* Resource not available (memory ...)   */
#define GT_FULL            (0x14)   /* Item is full (Queue or table etc...)  */
#define GT_EMPTY           (0x15)   /* Item is empty (Queue or table etc...) */
#define GT_INIT_ERROR      (0x16)   /* Error occurred while INIT process      */
#define GT_NOT_READY       (0x1A)   /* The other side is not ready yet       */
#define GT_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#define GT_OUT_OF_CPU_MEM  (0x1C)   /* Cpu memory allocation failed.         */
#define GT_ABORTED         (0x1D)   /* Operation has been aborted.           */

extern pthread_mutex_t nexthopHashMutex;
/*
 * typedef: enum CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT (type)
 *
 * Description: Each Look Up Translation Entry may point to a Route/s entry/ies
 *              Each (block) route entry block has a type.
 *
 * Enumerations:
 *      CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E - Equal-Cost-MultiPath block,
 *          where traffic is split among of the route entries according to a
 *          hash function. regular route entires are consider ECMP type with
 *          size of 1!
 *      CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E - Quality-of-Service block, where
 *          the route entry is according to the QoSProfile-to-Route-Block-Offset
 *          Table.
 */

typedef enum
{
    IP_ECMP_ROUTE_ENTRY_GROUP_E = 0,
    IP_QOS_ROUTE_ENTRY_GROUP_E = 1
}IP_ROUTE_ENTRY_METHOD_ENT;

/*
 * typedef: enum CPSS_IPV6_PREFIX_SCOPE_ENT
 *
 * Description: Address scope for IPv6 addresses
 *
 * Enumerations:
 *  CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E   - linked local scope
 *  CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E   - site local scope
 *  CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E - unique local scope
 *  CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E       - unique global scope
 */

typedef enum
{
    IPV6_PREFIX_SCOPE_LINK_LOCAL_E   = 0,
    IPV6_PREFIX_SCOPE_SITE_LOCAL_E   = 1,
    IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E = 2,
    IPV6_PREFIX_SCOPE_GLOBAL_E       = 3 
}IPV6_PREFIX_SCOPE_ENT;

/*
 * Typedef: struct CPSS_DXCH_IP_LTT_ENTRY_STC
 *
 * Description: LTT (lookup translation table) entry
 *
 * Fields:
 *      routeType              - Route Entry Type.
 *      numOfPaths             - The number of route paths bound to the lookup.
 *                               This value ranges from 0 (a single path) to
 *                               7 (8 ECMP or QoS route paths).
 *      routeEntryBaseIndex    - If numOfPaths = 0, this is the direct index to
 *                               the single Route entry. If numOfPaths is
 *                               greater than ?? this is the base index to a
 *                               block of contiguous Route entries.
 *                               The Route Entry Index range: 0?095.
 *      ucRPFCheckEnable         - Enable Unicast RPF check for this Entry.
 *      sipSaCheckMismatchEnable - Enable Unicast SIP MAC SA match check.
 *      ipv6MCGroupScopeLevel  - This is the IPv6 Multicast group scope level.
 *
 */

typedef struct IP_LTT_ENTRY_STCT
{
    IP_ROUTE_ENTRY_METHOD_ENT           routeType;
    unsigned int                        numOfPaths;
    unsigned int                        routeEntryBaseIndex;
    GT_BOOL                             ucRPFCheckEnable;
    GT_BOOL                             sipSaCheckMismatchEnable;
    IPV6_PREFIX_SCOPE_ENT          ipv6MCGroupScopeLevel;
}IP_LTT_ENTRY_STC;

#if 1
/*
 * typedef: struct CPSS_DXCH_PCL_ACTION_STC
 *cpoy from cpssDxChPcl.h and change some
 *
 * Comment:
 */

typedef GT_U16	GT_TRUNK_ID;

typedef struct
{
    unsigned int  ipclConfigIndex;
    unsigned int  pcl0_1OverrideConfigIndex;
    unsigned int  pcl1OverrideConfigIndex;
} CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC;
typedef struct
{
    unsigned int      cpuCode;
    GT_BOOL                       mirrorToRxAnalyzerPort;
    GT_BOOL                       mirrorTcpRstAndFinPacketsToCpu;
} CPSS_DXCH_PCL_ACTION_MIRROR_STC;

typedef struct
{
    GT_BOOL   enableMatchCount;
    GT_U32    matchCounterIndex;
} CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC;
typedef struct
{
    unsigned int      modifyDscp;
    unsigned int      modifyUp;
    union
    {
        struct
        {
            GT_U32                                      profileIndex;
            GT_BOOL                                     profileAssignIndex;
            unsigned int profilePrecedence;
        } ingress;
        struct
        {
            GT_U8                              dscp;
            GT_U8                              up;
        } egress;
    } qos;
} CPSS_DXCH_PCL_ACTION_QOS_STC;

typedef struct{
    unsigned int     type;

    struct{
        GT_U8   devNum;
        GT_U8   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    GT_U16       vidx;
    GT_U16       vlanId;
    GT_U8        devNum;
    GT_U16       fabricVidx;
    GT_U32       index;
}CPSS_INTERFACE_INFO_STC;

typedef struct
{
    unsigned int   redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            GT_BOOL                  vntL2Echo;
            GT_BOOL                  tunnelStart;
            GT_U32                   tunnelPtr;
            unsigned int 			 tunnelType;
        } outIf;
        GT_U32                   routerLttIndex;
        GT_U32                                            vrfId;
    } data;

} CPSS_DXCH_PCL_ACTION_REDIRECT_STC;
typedef struct
{
    unsigned int  policerEnable;
    GT_U32   policerId;

} CPSS_DXCH_PCL_ACTION_POLICER_STC;
typedef struct
{
    unsigned int          modifyVlan;
    GT_BOOL                                       nestedVlan;
    GT_U16                                        vlanId;
    unsigned int   precedence;

} CPSS_DXCH_PCL_ACTION_VLAN_STC;
typedef struct
{
    GT_BOOL  doIpUcRoute;
    GT_U32   arpDaIndex;
    GT_BOOL  decrementTTL;
    GT_BOOL  bypassTTLCheck;
    GT_BOOL  icmpRedirectCheck;

} CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC;
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC;

typedef struct
{
    unsigned int                    pktCmd;
    GT_BOOL                                actionStop;
    GT_BOOL                                bypassBrigge;
    GT_BOOL                                bypassIngressPipe;
    GT_BOOL                                egressPolicy;
    CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC lookupConfig;
    CPSS_DXCH_PCL_ACTION_MIRROR_STC        mirror;
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    CPSS_DXCH_PCL_ACTION_QOS_STC           qos;
    CPSS_DXCH_PCL_ACTION_REDIRECT_STC      redirect;
    CPSS_DXCH_PCL_ACTION_POLICER_STC       policer;
    CPSS_DXCH_PCL_ACTION_VLAN_STC          vlan;
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
    CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC     sourceId;
} CPSS_DXCH_PCL_ACTION_STC;
#endif

typedef union IP_TCAM_ROUTE_ENTRY_INFO_UNTT
{
	CPSS_DXCH_PCL_ACTION_STC pclIpUcAction;
	IP_LTT_ENTRY_STC ipLttEntry;
}IP_TCAM_ROUTE_ENTRY_INFO_UNT;


typedef union
{
    GT_U32  u32Ip;
    GT_U8   arIP[4];
}_GT_IPADDR;

struct rttbl_info
{
	unsigned int DIP;	/* destination address in TCAM*/
	unsigned int nexthop;	/* Next-Hop ip address*/
	unsigned int masklen;	/* destination ip mask in TCAM*/
	unsigned int ifindex;	/* Next-Hop interface*/
	unsigned int rt_type;
};

struct netlink_req{
  struct nlmsghdr nl;
  struct rtmsg    rt;
  char            buf[8192];
} req;


typedef enum
{
    IP_CNT_SET0_E   = 0,
    IP_CNT_SET1_E   = 1,
    IP_CNT_SET2_E   = 2,
    IP_CNT_SET3_E   = 3,
    IP_CNT_NO_SET_E = 4
}IP_CNT_SET_ENT;

typedef enum
{
    IP_DISREGARD_PORT_TRUNK_CNT_MODE_E = 0,
    IP_PORT_CNT_MODE_E                 = 1,
    IP_TRUNK_CNT_MODE_E                = 2
}IP_PORT_TRUNK_CNT_MODE_ENT;

typedef enum
{
    IP_DISREGARD_VLAN_CNT_MODE_E = 0,
    IP_USE_VLAN_CNT_MODE_E       = 1
}IP_VLAN_CNT_MODE_ENT;

typedef enum
{
   IP_PROTOCOL_IPV4_E     = 0,
   IP_PROTOCOL_IPV6_E     = 1,
   IP_PROTOCOL_IPV4V6_E   = 2
}IP_PROTOCOL_STACK_ENT;

typedef struct IP_COUNTER_SET_INTERFACE_CFG_STCT
{
    IP_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;
    IP_PROTOCOL_STACK_ENT           ipMode;
    IP_VLAN_CNT_MODE_ENT       vlanMode;
    GT_U8                                devNum;
    union
    {
        GT_U8                            port;
        unsigned short                      trunk;
    }portTrunk;
    GT_U16                               vlanId;
}IP_COUNTER_SET_INTERFACE_CFG_STC;

typedef enum
{
    IP_CNT_SET_INTERFACE_MODE_E    = 0,
    IP_CNT_SET_ROUTE_ENTRY_MODE_E      = 1
}IP_CNT_SET_MODE_ENT;


typedef struct IP_COUNTER_SET_STCT
{
    unsigned int inUcPkts;
    unsigned int inMcPkts;
    unsigned int inUcNonRoutedExcpPkts;
    unsigned int inUcNonRoutedNonExcpPkts;
    unsigned int inMcNonRoutedExcpPkts;
    unsigned int inMcNonRoutedNonExcpPkts;
    unsigned int inUcTrappedMirrorPkts;
    unsigned int inMcTrappedMirrorPkts;
    unsigned int mcRfpFailPkts;
    unsigned int outUcRoutedPkts;
}IP_COUNTER_SET_STC;

typedef struct route_drv_info
{
     unsigned int dip;
	 unsigned int nexthop;
	 unsigned int masklen;
     unsigned int baseindex;/*for marvell platform,this is the next hop route index*/
     unsigned int ifindex;/*tmp value for broadcom platform*/
     unsigned char dev;
	 unsigned char port;
	 void* data;/*used for user defined*/
}drv_infos;

typedef struct host_drv_info
{
	 unsigned char dev;
	 unsigned char port;
	 unsigned char mac[6];
     unsigned int ip;
     unsigned int baseindex;/*for marvell platform,this is the next hop route index*/
     unsigned int ifindex;/*tmp value for broadcom platform*/
	 void* data;/*used for user defined*/
}host_infos;


extern unsigned int  set_LPM_TBL_Entry
(
	unsigned int DIP,
	unsigned int nexthop,
	unsigned int masklen,
	unsigned int ifindex,
	unsigned int rt_type
); 
extern unsigned int lpm_entry_search
(
    unsigned int DIP,
    unsigned int masklen,
    unsigned int *entryIndex
);
extern unsigned int npd_route_nexthop_tblindex_get
(
	unsigned int ifIndex,
	unsigned int ipAddr,
	unsigned int *tblIndex
);

extern 	unsigned int	 nam_ipCnt_mode_set
(
	IN	unsigned char    				   devNum,
	IN	IP_CNT_SET_ENT 					   cntSet,
	IN	IP_CNT_SET_MODE_ENT			   cntSetMode,
	IN	IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
	
);

extern int  nam_route_set_ucrpf_enable
(
    unsigned int rfpen
);

extern void nam_route_get_ucrpf
(
    unsigned int* ucrpf
);

extern int nam_route_ipCnt_get
(
   unsigned char devNum,
   IP_CNT_SET_ENT cntSet,
   IP_COUNTER_SET_STC*  counters
);


/*******************************************************************************
* nam_thread_create
*
* DESCRIPTION:
*       Create Linux thread and start it.
*
* INPUTS:
*       name    - task name, valid when accessChip is TRUE
*       start_addr - task/thread Function to execute
*       arglist    - pointer to list of parameters for task/thread function
*	  accessChip - this thread/task need access chip register or not
*	  needJoin - this thread need pthread_join operation
*
* OUTPUTS:
*
* RETURNS:
*       0   - on success
*       1 - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern void nam_thread_create
(
	char	*name,
	unsigned (*entry_point)(void*),
	void	*arglist,
	unsigned char accessChip,
	unsigned char needJoin
);


int Npd_route_init();
GT_STATUS get_tblIndex_by_dip_mask(unsigned int DIP,unsigned int masklen,unsigned int *tblIndex);
GT_STATUS npd_route_get_route_tblIndex_by_dip(unsigned int DIP,unsigned int *tblIndex);

int new_addr_gratuitous_arp_solicit(void* ptr,int totlemsglen);


extern DBusConnection *npd_dbus_connection;
#endif

