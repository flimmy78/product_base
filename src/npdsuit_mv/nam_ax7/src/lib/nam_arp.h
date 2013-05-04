#ifndef __NAM_ARP_H__
#define __NAM_ARP_H__

#ifndef NULL
#define NULL	(0UL)
#endif

#define IN
#define OUT
#define INOUT

#define _1K 	1024

#define MAC_ADDR_LEN	6

#define NEXTHOP_TABLE_SIZE	(4 * _1K)
#define ARP_MAC_TABLE_SIZE	(4 * _1K)

#define ARP_EGRESS_IDX_MIN   (0x186A0) /* Egress object start index    */

typedef unsigned long GT_BOOL;

#define NAM_ARP_SNOOPING_ERR_NONE	0
#define NAM_ARP_SNOOPING_ERR_NULL_PTR	(NAM_ARP_SNOOPING_ERR_NONE + 1)
#define NAM_ARP_SNOOPING_ERR_BADPARAM	(NAM_ARP_SNOOPING_ERR_NONE + 2)
#define NAM_ARP_SNOOPING_ERR_GENERAL		(NAM_ARP_SNOOPING_ERR_NONE + 3)
#define NAM_ARP_SNOOPING_ACTION_TRAP2CPU (NAM_ARP_SNOOPING_ERR_NONE + 5)
#define NAM_ARP_SNOOPING_ACTION_HARD_DROP  (NAM_ARP_SNOOPING_ERR_NONE + 12)

typedef struct {
	unsigned char mac[MAC_ADDR_LEN];
}ethernet_mac;

enum nam_arp_snoop_op_ent {
	NAM_ARP_SNOOP_ADD_ITEM = 0,
	NAM_ARP_SNOOP_DEL_ITEM,
	NAM_ARP_SNOOP_UPDATE_ITEM,
	NAM_ARP_SNOOP_OCCUPY_ITEM,
	NAM_ARP_SNOOP_RESET_ITEM, /* means not delete ARP, just reset to default value( action TRAP_TO_CPU)*/
	NAM_ARP_SNOOP_ACTION_MAX
};

/*
 * typedef: enum CPSS_DXCH_IP_CPU_CODE_INDEX_ENT
 *
 * Description:  this value is added to the CPU code assignment in corse of trap
 *               or mirror
 *
 * Enumerations:
 *  CPSS_DXCH_IP_CPU_CODE_IDX_0_E - CPU code added index 0
 *  CPSS_DXCH_IP_CPU_CODE_IDX_1_E - CPU code added index 1
 *  CPSS_DXCH_IP_CPU_CODE_IDX_2_E - CPU code added index 2
 *  CPSS_DXCH_IP_CPU_CODE_IDX_3_E - CPU code added index 3
 */

typedef enum
{
    CPSS_DXCH_IP_CPU_CODE_IDX_0_E   = 0,
    CPSS_DXCH_IP_CPU_CODE_IDX_1_E   = 1,
    CPSS_DXCH_IP_CPU_CODE_IDX_2_E   = 2,
    CPSS_DXCH_IP_CPU_CODE_IDX_3_E   = 3
}CPSS_DXCH_IP_CPU_CODE_INDEX_ENT;

#ifdef DRV_LIB_CPSS_3_4
typedef enum
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_E   = 0,
    CPSS_DXCH_IP_UC_ECMP_RPF_E      = 1
}CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT;
#define GT_U8 unsigned char
#define GT_U16 unsigned short
#define GT_U32 unsigned int
#define GT_BOOL unsigned long

typedef struct CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STCT
{
    unsigned int                 cmd;
    unsigned int     cpuCodeIdx;
    GT_BOOL                             appSpecificCpuCodeEnable;
    GT_BOOL                             unicastPacketSipFilterEnable;
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_BOOL                             qosProfileMarkingEnable;
    GT_U32                              qosProfileIndex;
    unsigned int qosPrecedence;
    unsigned int       modifyUp;
    unsigned int       modifyDscp;
    unsigned int                 countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    GT_U32                              sipAccessLevel;
    GT_U32                              dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_BOOL                             scopeCheckingEnable;
    unsigned int                 siteId;
    GT_U32                              mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    GT_U16                              nextHopVlanId;
    CPSS_INTERFACE_INFO_STC             nextHopInterface;
    GT_U32                              nextHopARPPointer;
    GT_U32                              nextHopTunnelPointer;

}CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC;
typedef struct CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STCT
{
    GT_U16      vlanArray[8];

}CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC;

/*
 * Typedef: struct CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC
 *
 * Description: Unicast route entry
 *
 * Fields:
 *      type      - type of the route entry (refer to CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT)
 *      entry     - the route entry information
 *
 */
typedef union CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNTT
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     regularEntry;
    CPSS_DXCH_IP_UC_ECMP_RPF_FORMAT_STC        ecmpRpfCheck;

}CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT;
typedef struct CPSS_DXCH_IP_UC_ROUTE_ENTRY_STCT
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT    type;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_UNT         entry;

}CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC;
#endif
/*
 * typedef: enum CPSS_IP_CNT_SET_ENT
 *
 * Description: Each UC/MC Route Entry result can be linked with one the 
 *              below IP counter sets.
 *
 * Enumerations:
 *  CPSS_IP_CNT_SET0_E   - counter set 0
 *  CPSS_IP_CNT_SET1_E   - counter set 1
 *  CPSS_IP_CNT_SET2_E   - counter set 2
 *  CPSS_IP_CNT_SET3_E   - counter set 3
 *  CPSS_IP_CNT_NO_SET_E - do not link route entry with a counter set
 */

typedef enum
{
    CPSS_IP_CNT_SET0_E   = 0,
    CPSS_IP_CNT_SET1_E   = 1,
    CPSS_IP_CNT_SET2_E   = 2,
    CPSS_IP_CNT_SET3_E   = 3,
    CPSS_IP_CNT_NO_SET_E = 4
}CPSS_IP_CNT_SET_ENT;

/*
 * Typedef: enum CPSS_PACKET_CMD_ENT
 *
 * Description:
 *     This enum defines the packet command.
 * Fields:
 *     CPSS_PACKET_CMD_FORWARD_E           - forward packet
 *     CPSS_PACKET_CMD_MIRROR_TO_CPU_E     - mirror packet to CPU
 *     CPSS_PACKET_CMD_TRAP_TO_CPU_E       - trap packet to CPU
 *     CPSS_PACKET_CMD_DROP_HARD_E         - hard drop packet
 *     CPSS_PACKET_CMD_ROUTE_E             - IP Forward the packets
 *     CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E  - Packet is routed and mirrored to
 *                                           the CPU.
 *     CPSS_PACKET_CMD_DROP_SOFT_E         - soft drop packet
 *     CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E - Bridge and Mirror to CPU.
 *     CPSS_PACKET_CMD_BRIDGE_E            - Bridge only
 *     CPSS_PACKET_CMD_NONE_E              - Do nothing. (disable)
 *     CPSS_PACKET_CMD_INVALID_E        - invalid packet command
 *
 *  Comments:
 */

typedef enum
{
    CPSS_PACKET_CMD_FORWARD_E          , /* 00 */
    CPSS_PACKET_CMD_MIRROR_TO_CPU_E    , /* 01 */
    CPSS_PACKET_CMD_TRAP_TO_CPU_E      , /* 02 */
    CPSS_PACKET_CMD_DROP_HARD_E        , /* 03 */
    CPSS_PACKET_CMD_DROP_SOFT_E        , /* 04 */
    CPSS_PACKET_CMD_ROUTE_E            , /* 05 */
    CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E , /* 06 */
    CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E, /* 07 */
    CPSS_PACKET_CMD_BRIDGE_E           , /* 08 */
    CPSS_PACKET_CMD_NONE_E             , /* 09 */
    CPSS_PACKET_CMD_INVALID_E            /* 10 */
} CPSS_PACKET_CMD_ENT;

/*
 * Typedef: enum CPSS_IP_SITE_ID_ENT
 *
 * Description:
 *      This enum defines IPv6 Site ID (Used by Router for Ipv6 scope checking)
 * Fields:
 *     CPSS_IP_SITE_ID_INTERNAL_E - Internal 
 *     CPSS_IP_SITE_ID_EXTERNAL_E - External
 *
 *  Comments:
 */

typedef enum
{
    CPSS_IP_SITE_ID_INTERNAL_E,
    CPSS_IP_SITE_ID_EXTERNAL_E
} CPSS_IP_SITE_ID_ENT;

/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT
 *
 * Description:
 *      Enumerator for the packet's attribute assignment precedence
 *      for the subsequent assignment mechanism.
 *
 * Fields:
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E   - Soft precedence:
 *                        The packet's attribute assignment can be overridden
 *                        by the subsequent assignment mechanism
 *      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E   - Hard precedence:
 *                        The packet's attribute assignment is locked
 *                        to the last value of attribute assigned to the packet
 *                        and cannot be overridden.
 *
 *  Comments:
 *
 */

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;

/*
 * Typedef: enum CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT
 *
 * Description:
 *     Enumerator for modification of packet's attribute
 *     like User Priority and DSCP.
 *
 * Fields:
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E  - Keep
 *                      previous packet's attribute modification command.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,       - disable
 *                      modification of the packet's attribute.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E         - enable
 *                      modification of the packet's attribute.
 *     CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E        - invalid value
 *  Comments:
 *
 */

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;


typedef struct {
    CPSS_PACKET_CMD_ENT                 cmd;
    CPSS_DXCH_IP_CPU_CODE_INDEX_ENT     cpuCodeIdx;
    GT_BOOL                             appSpecificCpuCodeEnable;
    GT_BOOL                             unicastPacketSipFilterEnable;
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_BOOL                             qosProfileMarkingEnable;
    unsigned int                              	qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_IP_CNT_SET_ENT                 countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    unsigned int                        sipAccessLevel;
    unsigned int                        dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_BOOL                             scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                 siteId;
    unsigned int                        mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    unsigned short                      nextHopVlanId;
    CPSS_INTERFACE_INFO_STC             nextHopInterface;
    unsigned int                        nextHopARPPointer;
    unsigned int                        nextHopTunnelPointer;
}nam_nexthop_entry_s;


#ifdef DRV_LIB_BCM

#define BCM_EGRESS_IDX_MIN   (0x186A0) /* Egress object start index    */

#define NAM_L3_L2ONLY           (1 << 0)   /* L2 switch only on interface. */
#define NAM_L3_UNTAG            (1 << 1)   /* Packet goes out untagged. */
#define NAM_L3_S_HIT            (1 << 2)   /* Source IP address match. */
#define NAM_L3_D_HIT            (1 << 3)   /* Destination IP address match. */
#define NAM_L3_HIT              (BCM_L3_S_HIT | BCM_L3_D_HIT) 
#define NAM_L3_HIT_CLEAR        (1 << 4)   /* Clear HIT bit. */
#define NAM_L3_ADD_TO_ARL       (1 << 5)   /* Add interface address MAC to ARL. */
#define NAM_L3_WITH_ID          (1 << 6)   /* ID is provided. */
#define NAM_L3_NEGATE           (1 << 7)   /* Negate a set. */
#define NAM_L3_REPLACE          (1 << 8)   /* Replace existing entry. */
#define NAM_L3_TGID             (1 << 9)   /* Port belongs to trunk. */
#define NAM_L3_RPE              (1 << 10)  /* Pick up new priority (COS). */
#define NAM_L3_IPMC             (1 << 11)  /* Set IPMC for real IPMC entry. */
#define NAM_L3_L2TOCPU          (1 << 12)  /* Packet to CPU unrouted, XGS12: Set*/

#endif

/**********************************************************************************
 * nam_arp_table_index_init
 *
 * Initialize ARP Next-Hop table index and ARP Mac table index
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
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_table_index_init
(
	void
);

/**********************************************************************************
 * nam_arp_get_mactbl_index
 *
 * Get arp mac table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_GENERAL - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_get_mactbl_index
(
	OUT index_elem	 *val
);

/**********************************************************************************
 * nam_arp_free_mactbl_index
 *
 * Turn back arp mac table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		0 - if no error occurred
 *		-1  - if fail to free index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_free_mactbl_index
(
	IN index_elem	 val
);

/**********************************************************************************
 * nam_arp_get_nexthop_tbl_index
 *
 * Get route next-hop table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_GENERAL - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_get_nexthop_tbl_index
(
	OUT index_elem	 *val
);

/**********************************************************************************
 * nam_arp_free_nexthop_tbl_index
 *
 * Turn back next-hop table index
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		0 - if no error occurred
 *		-1  - if fail to free index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_free_nexthop_tbl_index
(
	IN index_elem	 val
);

/**********************************************************************************
 * nam_arp_snooping_op_item
 *
 * ARP snooping database add to/delete from ASIC or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *	
 *	OUTPUT:
 *		tblIndex - next-hop table index which used to hold next-hop item
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_NULL_PTR - if input parameters have null pointer
 *		NAM_ARP_SNOOPING_ERR_BADPARAM - if parameters wrong
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_snooping_op_item
(
	IN 	struct arp_snooping_item_s *dbItem,
	IN 	unsigned int action,
	OUT	unsigned int *tblIndex
);

/**********************************************************************************
 * nam_arp_snooping_get_item
 *
 * Get route next-hop table item by index
 *
 *	INPUT:
 *		devNum - device number
 *		tblIndex - table index to Next-Hop table
 *
 *	OUTPUT:
 *		val - arp snooping item info
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_GENERAL - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
int nam_arp_snooping_get_item
(
	IN	unsigned char devNum,
	IN	unsigned int  tblIndex,
	OUT struct arp_snooping_item_s *val
);

/**********************************************************************************
 * nam_set_system_arpsmaccheck_enable
 *
 *	set if enable to excute smac check or not for arp
 *
 *	INPUT:
 *		isenable - binary value to indicate enable or disable
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *
 **********************************************************************************/

unsigned int nam_arp_smac_check_enable
(
	unsigned int isenable
);


/*****************************************************************************************************
 *									external variable or function declearation											  *
 *																												  *
 *****************************************************************************************************/
#ifdef DRV_LIB_CPSS
extern unsigned long cpssDxChIpRouterArpAddrRead
(
	IN	unsigned char		devNum,
	IN	unsigned int		routerArpIndex,
	OUT ethernet_mac		*arpMacAddrPtr
);

extern unsigned long cpssDxChIpRouterArpAddrWrite
(
    IN unsigned char	devNum,
    IN unsigned int     routerArpIndex,
    IN ethernet_mac     *arpMacAddrPtr
);
#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDxChIpUcRouteEntriesRead
(
    IN unsigned char	devNum,
    IN unsigned int 	baseRouteEntryIndex,
    INOUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN unsigned long   	numOfRouteEntries
);

extern unsigned long cpssDxChIpUcRouteEntriesWrite
(
    IN unsigned char	devNum,
    IN unsigned int     baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN unsigned long    numOfRouteEntries
);

#else
extern unsigned long cpssDxChIpUcRouteEntriesRead
(
    IN unsigned char	devNum,
    IN unsigned int 	baseRouteEntryIndex,
    OUT nam_nexthop_entry_s *routeEntriesArray,
    IN unsigned long   	numOfRouteEntries
);

extern unsigned long cpssDxChIpUcRouteEntriesWrite
(
    IN unsigned char	devNum,
    IN unsigned int     baseRouteEntryIndex,
    IN nam_nexthop_entry_s *routeEntriesArray,
    IN unsigned long    numOfRouteEntries
);
#endif
extern unsigned long cpssDxChBrgGenArpSaCheckEnable
(
    IN unsigned char dev,
    IN unsigned long enable
);
/*******************************************************************************
* nam_arp_solicit_drv_send
*
* DESCRIPTION:
*       Send arp solicit packet from cpu via driver layer
*
* INPUTS:
*       devNum - device number
*	  portNum - port number on Dx device
*	  vid - vlan id of specified vlan to send arp
*	  isTagged - port is vlan tagged or untagged
*	  smac - source mac address
*	  dmac - destination mac address
*	  sip - source ip address
*	  dip - destination ip address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern unsigned long nam_arp_solicit_drv_send
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short vid,
	unsigned char isTagged,
	unsigned char *smac,
	unsigned char *dmac,
	unsigned int sip,
	unsigned int dip
);

#endif

/**********************************************************************************
 * nam_arp_solicit_send
 *	Send arp solicit packet via cpss driver
 * 
 *
 *	INPUT:
 *		item - arp snooping item info
 *		sysMac - system mac address
 *		gateway - L3 interface ip address (as source ip)
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NULL_PTR - if null pointer found
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_GENERAL - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int nam_arp_solicit_send
(
	struct arp_snooping_item_s *item,
	unsigned char *sysMac,
	unsigned int gateway
);



/**********************************************************************************
 * nam_arp_aging_dest_mac_broadcast
 *
 *	set if enable to excute smac check or not for arp
 *
 *	INPUT:
 *		isBroadCast - value to indicate broadcast or unicast
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *
 **********************************************************************************/

unsigned int nam_arp_aging_dest_mac_broadcast
(
	unsigned int isBroadCast
);
int nam_arp_nexthop_tbl_index_get
(
    index_elem val
);
int nam_arp_mactbl_index_get
(
	index_elem val
);

#endif
