/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfBridgeGen.h
*
* DESCRIPTION:
*       Generic API for Bridge
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfBridgeGenh
#define __tgfBridgeGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgCount.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>
    #include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgCount.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdbHash.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgEgrFlt.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgMc.h>
    #include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgVlan.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgSrcId.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
#endif /* EXMXPM_FAMILY */

/* define ports : of type CPSS_INTERFACE_INFO_STC */
#define PRV_TGF_INTERFACE_PORT_MAC(device,port)   \
    {                           \
        CPSS_INTERFACE_PORT_E,  \
        {                       \
            device,             \
            port,               \
        },                      \
        0                       \
    }

/* define trunk : of type CPSS_INTERFACE_INFO_STC */
#define PRV_TGF_INTERFACE_TRUNK_MAC(trunkId)   \
    {                           \
        CPSS_INTERFACE_TRUNK_E, \
        {                       \
            0,                  \
            0,                  \
        },                      \
        trunkId                 \
    }

/* define trunk : of type PRV_TGF_OUTLIF_INFO_STC */
#define PRV_TGF_OUTLIF_LL_MAC(device,port)                        \
{                                                                 \
    PRV_TGF_OUTLIF_TYPE_LL_E,                                     \
    PRV_TGF_INTERFACE_PORT_MAC(device,port), /*interface*/        \
}

/* default (empty) mac entry : of type PRV_TGF_BRG_MAC_ENTRY_STC */
#define PRV_TGF_BRG_FDB_MAC_ENTRY_DEFAULT_MAC           \
    {                                                   \
        {  /*PRV_TGF_MAC_ENTRY_KEY_STC*/               \
            PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E,         \
            {                                           \
                {                                       \
                    {{0x0,0x15,0x15,0x15,0x15,0x15}},/*mac*/\
                    1,                    /*vlan*/      \
                    1                     /*fid*/       \
                }                                       \
            }                                           \
        },                                              \
        PRV_TGF_INTERFACE_PORT_MAC(0,0), /*interface*/  \
        PRV_TGF_OUTLIF_LL_MAC(0,0),/*outlif*/           \
        0,   /*isStatic;                  */            \
        0,   /*daCommand;                 */            \
        0,   /*saCommand;                 */            \
        0,   /*daRoute;                   */            \
        0,   /*mirrorToRxAnalyzerPortEn;  */            \
        0,   /*userDefined;              */             \
        0,   /*daQosIndex;               */             \
        0,   /*saQosIndex;               */             \
        0,   /*daSecurityLevel;          */             \
        0,   /*saSecurityLevel;          */             \
        0,   /*appSpecificCpuCode;       */             \
        0,   /*pwId;                     */             \
        0,   /*spUnknown;                */             \
        0,   /*sourceId;                 */             \
    }

/* number of words that the FDB device table is using */
#define PRV_TGF_FDB_DEV_TABLE_SIZE_CNS    4


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/


/*
 * Typedef: enum PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_ENT
 *
 * Description:
 *      This enum defines station movement modes
 *
 * Fields:
 *     PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_PORT_TO_PORT_E - station moved from
 *          one port to another port
 *     PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_PORT_TO_TRUNK_E - station moved from
 *          port to trunk
 *     PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_TRUNK_TO_PORT_E - station moved from
 *          trunk to port
 *     PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_TRUNK_TO_TRUNK_E - station moved from
 *          one trunk to another trunk
 *     PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_IN_THE_TRUNK_E - station moved from
 *          one port of the trunk to another port in the SAME trunk.
 *
 */

typedef enum{
    PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_PORT_TO_PORT_E,
    PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_PORT_TO_TRUNK_E,
    PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_TRUNK_TO_PORT_E,
    PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_TRUNK_TO_TRUNK_E,

    PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_IN_THE_TRUNK_E
}PRV_TGF_BRG_FDB_STATION_MOVEMENT_MODE_ENT;

/*
 * Typedef: enum PRV_TGF_BRG_FDB_ACCESS_MODE_ENT
 *
 * Description:
 *      This enum defines access mode to the FDB table
 *
 * Fields:
 *     PRV_TGF_BRG_FDB_ACCESS_MODE_BY_MESSAGE_E - 'by message'
 *     PRV_TGF_BRG_FDB_ACCESS_MODE_BY_INDEX_E - 'by Index'
 *
 */

typedef enum{
    PRV_TGF_BRG_FDB_ACCESS_MODE_BY_MESSAGE_E,
    PRV_TGF_BRG_FDB_ACCESS_MODE_BY_INDEX_E
}PRV_TGF_BRG_FDB_ACCESS_MODE_ENT;

/*
 * Typedef: enum PRV_TGF_BRG_IP_CTRL_TYPE_ENT
 *
 * Description:
 *      This enum defines IP Control to CPU Enable Mode
 * Fields:
 *     PRV_TGF_BRG_IP_CTRL_NONE_E - Disable both IPv4 and Ipv6 Control traffic to CPU
 *     PRV_TGF_BRG_IP_CTRL_IPV4_E - Enable IPv4 Control traffic to CPU
 *     PRV_TGF_BRG_IP_CTRL_IPV6_E - Enable IPv6 Control traffic to CPU
 *     PRV_TGF_BRG_IP_CTRL_IPV4_IPV6_E - Enable both IPv4 and IPv6 traffic to CPU
 *
 */
typedef enum
{
    PRV_TGF_BRG_IP_CTRL_NONE_E,
    PRV_TGF_BRG_IP_CTRL_IPV4_E,
    PRV_TGF_BRG_IP_CTRL_IPV6_E,
    PRV_TGF_BRG_IP_CTRL_IPV4_IPV6_E
} PRV_TGF_BRG_IP_CTRL_TYPE_ENT;

/*
 * Typedef: enum PRV_TGF_FDB_ENTRY_TYPE_ENT
 *
 * Description:
 *      This enum defines the type of the FDB Entry
 * Fields:
 *     PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E   - The entry is MAC Address
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E - The entry is IPv4 Multicast
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E - The entry is IPv6 Multicast
 *     PRV_TGF_FDB_ENTRY_TYPE_VPLS_E       - The entry is VPLS entry
 *
 */
typedef enum
{
    PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E,
    PRV_TGF_FDB_ENTRY_TYPE_VPLS_E
} PRV_TGF_FDB_ENTRY_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_PACKET_CMD_ENT
 *
 * Description:
 *     This enum defines the packet command.
 *
 * Fields:
 *     PRV_TGF_PACKET_CMD_FORWARD_E           - forward packet
 *     PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E     - mirror packet to CPU
 *     PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E       - trap packet to CPU
 *     PRV_TGF_PACKET_CMD_DROP_HARD_E         - hard drop packet
 *     PRV_TGF_PACKET_CMD_ROUTE_E             - IP Forward the packets
 *     PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E  - Packet is routed and mirrored
 *     PRV_TGF_PACKET_CMD_DROP_SOFT_E         - soft drop packet
 *     PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E - Bridge and Mirror to CPU.
 *     PRV_TGF_PACKET_CMD_BRIDGE_E            - Bridge only
 *     PRV_TGF_PACKET_CMD_INTERV_E            - intervention to CPU
 *     PRV_TGF_PACKET_CMD_CNTL_E              - control (unconditionally trap to CPU)
 *     PRV_TGF_PACKET_CMD_NONE_E              - Do nothing. (disable)
 *
 */
typedef enum
{
    PRV_TGF_PACKET_CMD_FORWARD_E,
    PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E,
    PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E,
    PRV_TGF_PACKET_CMD_DROP_HARD_E,
    PRV_TGF_PACKET_CMD_DROP_SOFT_E,
    PRV_TGF_PACKET_CMD_ROUTE_E,
    PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E,
    PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E,
    PRV_TGF_PACKET_CMD_BRIDGE_E,
    PRV_TGF_PACKET_CMD_INTERV_E,
    PRV_TGF_PACKET_CMD_CNTL_E,
    PRV_TGF_PACKET_CMD_NONE_E
} PRV_TGF_PACKET_CMD_ENT;

/*
 * typedef: enum PRV_TGF_BRG_DROP_CNTR_MODE_ENT
 *
 * Description: Enumeration of Bridge Drop Counter reasons to be counted.
 *
 * Fields:
 *    PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E           - Count All
 *    PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E       - FDB Entry command Drop
 *    PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E      - Unknown MAC SA Drop
 *    PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E          - Invalid SA Drop
 *    PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E        - VLAN not valid Drop
 *    PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E    - Port not Member in VLAN Drop
 *    PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E          - VLAN Range Drop
 *    PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E   - Moved Static Address Drop
 *    PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E          - Rate Limiting Drop
 *    PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E          - Local Port Drop
 *    PRV_TGF_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E   - Spanning Tree port state Drop
 *    PRV_TGF_BRG_DROP_CNTR_IP_MC_E               - IP Multicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_NON_IP_MC_E           - NON-IP Multicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E    - DSATag due to Local dev Drop
 *    PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E       - IEEE Reserved Drop
 *    PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E  - Unregistered L2 NON-IPM Multicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E     - Unregistered L2 IPv6 Multicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E     - Unregistered L2 IPv4 Multicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E       - Unknown L2 Unicast Drop
 *    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E     - Unregistered L2 IPv4 Brodcast Drop
 *    PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E - Unregistered L2 NON-IPv4 Brodcast Drop
 *    PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E     - ARP SA mismatch Drop
 *    PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E       - SYN with data Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E   - TCP over MCAST or BCAST Drop
 *    PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E       - Bridge Access Matrix Drop
 *    PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E        - Secure Learning Drop
 *    PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E   - Acceptable Frame Type Drop
 *    PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E           - Fragmented ICMP Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E      - TCP Flags Zero Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E   - TCP Flags FIN, URG and PSH are all set Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E    - TCP Flags SYN and FIN are set Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E    - TCP Flags SYN and RST are set Drop
 *    PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E   - TCP/UDP Source
 *    PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E            - VLAN MRU Drop
 */
typedef enum
{
    PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E = 0,                /* 0  */
    PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E,                /* 1  */
    PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,               /* 2  */
    PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E,                   /* 3  */
    PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E,                 /* 4  */
    PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,             /* 5  */
    PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E,                   /* 6  */
    PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E,            /* 7  */
    PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,              /* 8  */
    PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E,                /* 9  */
    PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E,            /* 10 */
    PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E,                /* 11 */
    PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E,                 /* 12 */
    PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E,            /* 13 */
    PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E,                    /* 14 */
    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,               /* 15 */
    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,            /* 16 */
    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,             /* 17 */
    PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,             /* 18 */
    PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,            /* 19 */
    PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E,                     /* 20 */
    PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E,                   /* 21 */
    PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E,                   /* 22 */
    PRV_TGF_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E,            /* 23 */
    PRV_TGF_BRG_DROP_CNTR_IP_MC_E,                        /* 24 */
    PRV_TGF_BRG_DROP_CNTR_NON_IP_MC_E,                    /* 25 */
    PRV_TGF_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E,             /* 26 */
    PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E,                /* 27 */
    PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E,           /* 28 */
    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E,              /* 29 */
    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E,              /* 30 */
    PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E,                /* 31 */
    PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E,              /* 32 */
    PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E,          /* 33 */
    PRV_TGF_BRG_DROP_CNTR_DATA_ERROR_E,                   /* 34 */
    PRV_TGF_BRG_DROP_CNTR_IPMC_IANA_RANGE_E,              /* 35 */
    PRV_TGF_BRG_DROP_CNTR_IPMC_NOT_IANA_RANGE_E,          /* 36 */
    PRV_TGF_BRG_DROP_CNTR_VALUE_CHASIS_E,                 /* 37 */
    PRV_TGF_BRG_DROP_CNTR_BLOCKING_LEARNING_E             /* 38 */
} PRV_TGF_BRG_DROP_CNTR_MODE_ENT;

/*
 * typedef: enum PRV_TGF_BRG_VLAN_PORT_ISOLATION_CMD_ENT
 *
 * Description: Enumeration of port isolation commands
 *
 * Enumerations:
 *    PRV_TGF_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E - port isolation is disabled
 *    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_CMD_E      - L2 packets forwaring enabled
 *    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L3_CMD_E      - L3 packets forwaring enabled
 *    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E   - L2&L3 packets forwaring enabled
 *
 */
typedef enum
{
    PRV_TGF_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L3_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E
} PRV_TGF_BRG_VLAN_PORT_ISOLATION_CMD_ENT;

/*
 * typedef: enum PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT
 *
 * Description: Enumeration of tagging egress commands
 *
 * Enumerations:
 *    PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E              - if Tag0 and/or Tag1
 *                                                          were classified in
 *                                                          the incoming packet,
 *                                                          they are removed
 *                                                          from the packet.
 *    PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E                  - packet egress with
 *                                                          Tag0 (Tag0 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E                  - packet egress with
 *                                                          Tag1 (Tag1 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E - Outer Tag0, Inner
 *                                                          Tag1 (tag swap).
 *    PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E - Outer Tag1, Inner
 *                                                          Tag0 (tag swap).
 *    PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E             - TAG0 is added to
 *                                                          the packet
 *                                                          regardless of
 *                                                          whether Tag0 and
 *                                                          TAG1 were
 *                                                          classified in the
 *                                                          incoming packet.
 *    PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E         - the incoming packet
 *                                                          outer tag is
 *                                                          removed, regardless
 *                                                          of whether it is
 *                                                          Tag0 or Tag1. This
 *                                                          operation is a NOP
 *                                                          if the packet
 *                                                          arrived with neither
 *                                                          Tag0 nor Tag1
 *                                                          classified.
 * Note:
 *      Relevant for devices with TR101 feature support
 */
typedef enum
{
    PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
    PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E
} PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT;

/*
 * typedef: enum PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ENT
 *
 * Description: Enumeration of assigning L2 multicast VIDX for Broadcast,
 *              Unregistered Multicast, and unknown Unicast packets.
 *
 * Enumerations:
 *    PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E            - unregistered MC mode
 *    PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E - apply <Flood VIDX> from VLAN entry for ALL
 *                                                             flooded traffic
 *
 */
typedef enum
{
   PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E,
   PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E
} PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ENT;

/*
 * typedef: enum PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT
 *
 * Description: Enumeration of local switching traffic types.
 *
 * Enumerations:
 *    PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E - known UC
 *    PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E - unknown UC, BC, MC
 *
 */
typedef enum
{
   PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E,
   PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E
} PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_FDB_ACTION_MODE_ENT
 *
 * Description: Enum for FDB Action modes.
 *
 * Enumerations:
 *      PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E    - Automatic aging with removal
 *                                                 of aged out entries.
 *      PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E - Automatic or triggered aging
 *                                                 without removal of aged out
 *                                                 entries or triggered FDB
 *                                                 Upload.
 *      PRV_TGF_FDB_ACTION_DELETING_E            - Triggered Address deleting.
 *      PRV_TGF_FDB_ACTION_TRANSPLANTING_E       - Triggered Address
 *                                                 Transplanting.
 *      PRV_TGF_FDB_ACTION_FDB_UPLOAD_E          - Scan the FDB table and send
 *                                                 back to the CPU matching
 *                                                 entries (FDB Upload).
 */
typedef enum{
    PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E,
    PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E,
    PRV_TGF_FDB_ACTION_DELETING_E,
    PRV_TGF_FDB_ACTION_TRANSPLANTING_E,
    PRV_TGF_FDB_ACTION_FDB_UPLOAD_E
} PRV_TGF_FDB_ACTION_MODE_ENT;

/*
 * typedef: enum PRV_TGF_MAC_ACTION_MODE_ENT
 *
 * Description: Enumeration of MAC address table action mode
 *
 * Enumerations:
 *      PRV_TGF_ACT_AUTO_E - Action is done automatically
 *      PRV_TGF_ACT_TRIG_E - Action is done via trigger from CPU
 */
typedef enum
{
    PRV_TGF_ACT_AUTO_E = 0,
    PRV_TGF_ACT_TRIG_E

} PRV_TGF_MAC_ACTION_MODE_ENT;

/*
 * typedef: struct PRV_TGF_BRG_VLAN_INFO_STC
 *
 * Description: A structure to hold generic VLAN entry info
 *
 * Fields:
 *      cpuMember            - is CPU member
 *      unkSrcAddrSecBreach  - Unknown Source Address is security breach event.
 *      unregNonIpMcastCmd   - The command of Unregistered non IP Multicast packets
 *      unregIpv4McastCmd    - The command of Unregistered IPv4 Multicast packets
 *      unregIpv6McastCmd    - The command of Unregistered IPv6 Multicast packets
 *      unkUcastCmd          - The command of Unknown Unicast packets
 *      unregIpv4BcastCmd    - The command of Unregistered IPv4 Broadcast packets
 *      unregNonIpv4BcastCmd - The command of Unregistered non-IPv4-Broadcast
 *      ipv4IgmpToCpuEn      - Enable IGMP Trapping or Mirroring to CPU
 *      mirrToRxAnalyzerEn   - Mirror packets to Rx Analyzer port
 *      mirrorToTxAnalyzerEn - Mirror packets to Tx Analyzer port
 *      ipv6IcmpToCpuEn      - Enable/Disable ICMPv6 trapping or mirroring
 *      ipCtrlToCpuEn        - IPv4/6 control traffic trapping/mirroring to the CPU
 *      ipv4IpmBrgMode       - IPV4 Multicast Bridging mode
 *      ipv6IpmBrgMode       - IPV6 Multicast Bridging mode
 *      ipv4IpmBrgEn         - IPv4 Multicast Bridging Enable
 *      ipv6IpmBrgEn         - IPv6 Multicast Bridging Enable
 *      ipv6SiteIdMode       - IPv6 Site Id Mode
 *      ipv4UcastRouteEn     - Enable/Disable IPv4 Unicast Routing on the vlan
 *      ipv4McastRouteEn     - Enable/Disable IPv4 Multicast Routing on the vlan
 *      ipv6UcastRouteEn     - Enable/Disable IPv6 Unicast Routing on the vlan
 *      ipv6McastRouteEn     - Enable/Disable IPv6 Multicast Routing on the vlan
 *      stgId                - stp group, range 0-255
 *      autoLearnDisable     - Enable/Disable automatic learning for this VLAN
 *      naMsgToCpuEn         - Enable/Disable new address message sending to CPU
 *      mruIdx               - The index that this Vlan's MRU configuration packets
 *      bcastUdpTrapMirrEn   - Enables trapping/mirroring of Broadcast UDP packets
 *      vrfId                - Virtual Router ID
 *      egressUnregMcFilterCmd - The action applied on Multicast packets
 *      floodVidx            - MC group to which the flooded packet is to be transmitted
 *      floodVidxMode        - Mode for assigning L2 multicast VIDX for
 *                             BC, Unregistered MC, and unknown UC packets
 *      portIsolationMode    - Port isolation Mode
 *      ucastLocalSwitchingEn - Enable/disable local switching of known UC traffic
 *      mcastLocalSwitchingEn - Enable/disable local switching of MC, unknown UC
 *                              and BC traffic on this VLAN
 *
 */
typedef struct
{
    GT_BOOL                                 cpuMember;
    GT_BOOL                                 unkSrcAddrSecBreach;
    CPSS_PACKET_CMD_ENT                     unregNonIpMcastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv4McastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv6McastCmd;
    CPSS_PACKET_CMD_ENT                     unkUcastCmd;
    CPSS_PACKET_CMD_ENT                     unregIpv4BcastCmd;
    CPSS_PACKET_CMD_ENT                     unregNonIpv4BcastCmd;
    GT_BOOL                                 ipv4IgmpToCpuEn;
    GT_BOOL                                 mirrToRxAnalyzerEn;
    GT_BOOL                                 mirrorToTxAnalyzerEn;
    GT_BOOL                                 ipv6IcmpToCpuEn;
    PRV_TGF_BRG_IP_CTRL_TYPE_ENT            ipCtrlToCpuEn;
    CPSS_BRG_IPM_MODE_ENT                   ipv4IpmBrgMode;
    CPSS_BRG_IPM_MODE_ENT                   ipv6IpmBrgMode;
    GT_BOOL                                 ipv4IpmBrgEn;
    GT_BOOL                                 ipv6IpmBrgEn;
    CPSS_IP_SITE_ID_ENT                     ipv6SiteIdMode;
    GT_BOOL                                 ipv4UcastRouteEn;
    GT_BOOL                                 ipv4McastRouteEn;
    GT_BOOL                                 ipv6UcastRouteEn;
    GT_BOOL                                 ipv6McastRouteEn;
    GT_U32                                  stgId;
    GT_BOOL                                 autoLearnDisable;
    GT_BOOL                                 naMsgToCpuEn;
    GT_U32                                  mruIdx;
    GT_BOOL                                 bcastUdpTrapMirrEn;
    GT_U32                                  vrfId;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT        egressUnregMcFilterCmd;
    GT_U16                                  floodVidx;
    PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ENT    floodVidxMode;
    PRV_TGF_BRG_VLAN_PORT_ISOLATION_CMD_ENT portIsolationMode;
    GT_BOOL                                 ucastLocalSwitchingEn;
    GT_BOOL                                 mcastLocalSwitchingEn;
} PRV_TGF_BRG_VLAN_INFO_STC;

/*
 * typedef: struct PRV_TGF_MAC_ENTRY_KEY_STC
 *
 * Description: Key parameters of the MAC Address Entry
 *
 * Fields:
 *  entryType   - the type of the entry MAC Addr or IP Multicast
 *  macAddr     - mac address (for <entry type> = MAC Address entry)
 *  vlanId      - Vlan Id (for both types of the entry) [0..0xFFF]
 *  fId         - Forwarding Id
 *  dip         - For IPv4 entry type it's destination address
 *  sip         - For IPv4 entry type it's source address
 *
*/
typedef struct
{
    PRV_TGF_FDB_ENTRY_TYPE_ENT         entryType;
    union  {
       struct {
           GT_ETHERADDR     macAddr;
           GT_U16           vlanId;
           GT_U32           fId;
       } macVlan;

       struct {
           GT_U8            sip[4];
           GT_U8            dip[4];
           GT_U16           vlanId;
           GT_U32           fId;
       } ipMcast;

    } key;

} PRV_TGF_MAC_ENTRY_KEY_STC;

/*
 * typedef: struct PRV_TGF_BRG_MAC_ENTRY_STC
 *
 * Description: FDB Entry
 *
 * Fields:
 *   key             - key data
 *   dstInterface    - interface info
 *   dstOutlif       - outlif info: port/trunk/vidx/dit
 *   isStatic        - static/dynamic entry
 *   daCommand       - action taken when a packet's DA matches this entry
 *   saCommand       - action taken when a packet's SA matches this entry
 *   daRoute         - send packet to the IPv4 or MPLS
 *   mirrorToRxAnalyzerPortEn - configurable analyzer port.
 *   sourceId        - Source ID
 *   userDefined     - user defined field
 *   daQosIndex      - destination Qos attribute
 *   saQosIndex      - source Qos attribute
 *   daSecurityLevel - security level assigned to the MAC DA
 *   saSecurityLevel - security level assigned to the MAC SA
 *   appSpecificCpuCode - specific CPU code
 *   pwId            - Pseudo-Wire ID
 *   spUnknown       - relevant when auto-learning is disabled
 *
 */
typedef struct
{
       PRV_TGF_MAC_ENTRY_KEY_STC    key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       PRV_TGF_OUTLIF_INFO_STC      dstOutlif;
       GT_BOOL                      isStatic;
       PRV_TGF_PACKET_CMD_ENT       daCommand;
       PRV_TGF_PACKET_CMD_ENT       saCommand;
       GT_BOOL                      daRoute;
       GT_BOOL                      mirrorToRxAnalyzerPortEn;
       GT_U32                       userDefined;
       GT_U32                       daQosIndex;
       GT_U32                       saQosIndex;
       GT_U32                       daSecurityLevel;
       GT_U32                       saSecurityLevel;
       GT_BOOL                      appSpecificCpuCode;
       GT_U32                       pwId;
       GT_BOOL                      spUnknown;
       GT_U32                       sourceId;
} PRV_TGF_BRG_MAC_ENTRY_STC;

/*
 * Typedef: structure PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC
 *
 * Description: structure that hold an array of ports' Tagging commands
 *
 * Fields:
 *      portsCmd - array of ports' tagging commands
 *
 * Note:
 *      Array index is port number.
 */
typedef struct
{
    PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT portsCmd[CPSS_MAX_PORTS_NUM_CNS];
} PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC;

/*
 * typedef: enum PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT
 *
 * Description: Enumeration of Tag1 removal mode when VID is zero.
 *
 * Enumerations:
 *    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E - don’t remove Tag1
 *
 *    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E - enable only if
 *                          ingress was NOT double tag (single tag or untagged)
 *
 *    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E - enable regardless of ingress
 *                          tagging
 */
typedef enum
{
    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E,
    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E,
    PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E
}PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT;


/*
 * Typedef: enumeration PRV_TGF_BRG_GEN_L1_RATE_LIMIT_ENT
 *
 * Description: Enumeration for L1 rate limit counting mode upon byte counting.
 *              When rate limit counting mode is byte count this bit controls
 *              the addition of 20 bytes for L1 (IPG + Preamble) for the packet
 *              size.
 *
 * Fields:
 *      PRV_TGF_BRG_GEN_L1_RATE_LIMIT_EXCL_E - Exclude L1; Rate limit byte
 *                                                 count does not include the
 *                                                 L1 value.
 *      PRV_TGF_BRG_GEN_L1_RATE_LIMIT_INCL_E - Include L1; Rate limit byte
 *                                                 count includes the L1 value.
 *
 */
typedef enum
{
    PRV_TGF_BRG_GEN_L1_RATE_LIMIT_EXCL_E = 0,
    PRV_TGF_BRG_GEN_L1_RATE_LIMIT_INCL_E
}PRV_TGF_BRG_GEN_L1_RATE_LIMIT_ENT;

/*
 * Typedef: struct PRV_TGF_BRG_GEN_RATE_LIMIT_STC
 *
 * Description:
 *      Describe rate limit parameters for DxCh devices.
 *
 * Fields:
 *     rateLimitModeL1  - L1 rate limit counting mode upon byte counting
 *     dropMode     - drop mode (soft or hard)
 *     rMode        - rate mode (bytes or packets)
 *     win10Mbps    - size of time window for 10Mbps port speed
 *     win100Mbps   - size of time window for 100Mbps port speed
 *     win1000Mbps  - size of time window for 1000Mbps port speed
 *     win10Gbps    - size of time window for 10Gbps port speed
 *
 * Comments:
 *   Window sizes specified in microseconds
 *
 *       1000 Mbps: range - 256-16384 uSec    granularity - 256 uSec
 *       100  Mbps: range - 256-131072 uSec   granularity - 256 uSec
 *       10   Mbps: range - 256-1048570 uSec  granularity - 256 uSec
 *       10   Gbps: range - 25.6-104857  uSec  granularity - 25.6 uSec
 *       For DxCh3 and above: The granularity doesn't depend from PP core clock.
 *       For DxCh1, DxCh2:
 *       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
 *       for they changed linearly with the clock value.
 */
typedef struct
{
    PRV_TGF_BRG_GEN_L1_RATE_LIMIT_ENT   rateLimitModeL1;
    CPSS_DROP_MODE_TYPE_ENT         dropMode;
    CPSS_RATE_LIMIT_MODE_ENT        rMode;
    GT_U32                          win10Mbps;
    GT_U32                          win100Mbps;
    GT_U32                          win1000Mbps;
    GT_U32                          win10Gbps;
} PRV_TGF_BRG_GEN_RATE_LIMIT_STC;


/*
 * Typedef: struct CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC
 *
 * Description:
 *      Describe a port rate limit parameters.
 *
 * Fields:
 *     enableBc         - enable Bc packets rate limiting
 *     enableMc         - enable Mc packets rate limiting
 *                        for xCat2 it controls unregistered Mc packets only
 *     enableMcReg      - enable Mc registered packets rate limiting 
 *                        (APPLICABLE DEVICES: xCat2)
 *     enableUcUnk      - enable Uc Unknown packets rate limiting
 *     enableUcKnown    - enable Uc Known   packets rate limiting
 *     enableTcpSyn     - enable TCP SYN packets rate limiting
 *     rateLimit        - rate limit specified in 64 bytes/packets
 *                        depends on ratelimit mode
 *                        range 0 - 65535
 *
 * Comments:
 *     None
 */
typedef struct
{
    GT_BOOL   enableBc;
    GT_BOOL   enableMc;
    GT_BOOL   enableMcReg;
    GT_BOOL   enableUcUnk;
    GT_BOOL   enableUcKnown;
    GT_BOOL   enableTcpSyn;
    GT_U32    rateLimit;
} PRV_TGF_BRG_GEN_RATE_LIMIT_PORT_STC;

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Updates VRF Id in HW vlan entry
*
* INPUTS:
*       vlanId             - VLAN Id
*       vrfId              - VRF Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanVrfIdSet
(
    IN GT_U16                               vlanId,
    IN GT_U32                               vrfId
);

/*******************************************************************************
* prvTgfBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW
*
* INPUTS:
*       devNum             - device number
*       vlanId             - VLAN Id
*       portsMembersPtr    - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr    - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr        - (pointer to) VLAN specific information
*       portsTaggingCmdPtr - (pointer to) ports tagging commands in the vlan
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEntryWrite
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_PORTS_BMP_STC                  *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC                  *portsTaggingPtr,
    IN PRV_TGF_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);

/*******************************************************************************
* prvTgfBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry
*
* INPUTS:
*       vlanId - VLAN id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEntryInvalidate
(
    IN GT_U16               vlanId
);

/*******************************************************************************
* prvTgfBrgVlanMemberAdd
*
* DESCRIPTION:
*       Set specific member at VLAN entry
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN Id
*       portNum  - port number
*       isTagged - GT_TRUE/GT_FALSE, to set the port as tagged/untagged
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_FAIL      - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMemberAdd
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U8                portNum,
    IN GT_BOOL              isTagged
);

/*******************************************************************************
* prvTgfBrgVlanTagMemberAdd
*
* DESCRIPTION:
*       Set specific tagged member at VLAN entry
*
* INPUTS:
*       vlanId          - VLAN Id
*       portNum         - port number
*       portTaggingCmd  - port tagging command
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_FAIL      - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTagMemberAdd
(
    IN GT_U16                            vlanId,
    IN GT_U8                             portNum,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
);

/*******************************************************************************
* prvTgfBrgVlanMemberRemove
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMemberRemove
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U8                portNum
);

/*******************************************************************************
* prvTgfBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       vlanId  - VLAN Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong devNum port, or vlanId
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortVidSet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  GT_U16              vlanId
);

/*******************************************************************************
* prvTgfBrgVlanPortVidGet
*
* DESCRIPTION:
*       Get port's default VLAN Id
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       vlanIdPtr - VLAN Id
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong devNum port, or vlanId
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortVidGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    OUT GT_U16             *vlanIdPtr
);

/*******************************************************************************
* prvTgfBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       precedence - precedence type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        portNum,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* INPUTS:
*       index       - hw mac entry index
*       skip        - entry skip control
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong saCommand,daCommand
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryWrite
(
    IN GT_U32                        index,
    IN GT_BOOL                       skip,
    IN PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*
* INPUTS:
*       index  - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       devNumPtr   - (pointer to) is device number associated with the entry
*       macEntryPtr - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parametres
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryRead
(
    IN  GT_U32                        index,
    OUT GT_BOOL                      *validPtr,
    OUT GT_BOOL                      *skipPtr,
    OUT GT_BOOL                      *agedPtr,
    OUT GT_U8                        *devNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* INPUTS:
*       index  - hw mac entry index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryInvalidate
(
    IN GT_U32               index
);

/*******************************************************************************
* prvTgfBrgFdbEntryByMessageMustSucceedSet
*
* DESCRIPTION:
*       when add/delete FDB entry 'by message' operation can not succeed.
*       the operation MUST finish but is may not succeed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
* INPUTS:
*       mustSucceed - GT_TRUE - operation must succeed
*                     GT_FALSE - operation may fail (must finish but not succeed)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the previous state of the flag
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfBrgFdbEntryByMessageMustSucceedSet
(
    IN GT_BOOL     mustSucceed
);

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table
*
* INPUTS:
*       macEntryPtr - (pointer to) mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntrySet
(
    IN PRV_TGF_BRG_MAC_ENTRY_STC     *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryGet
*
* DESCRIPTION:
*       Get existing entry in Hardware MAC address table
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       macEntryPtr - (pointer to) mac table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryGet
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) key parameters of the mac entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDelete
(
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
);

/*******************************************************************************
* prvTgfBrgFdbFlush
*
* DESCRIPTION:
*       Flush FDB table (with/without static entries)
*
* INPUTS:
*       includeStatic - include static entries
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlush
(
    IN GT_BOOL                        includeStatic
);

/*******************************************************************************
* prvTgfBrgFdbActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
*       NOTE: caller should call prvTgfBrgFdbActionDoneWait(...) to check that
*       operation done before checking for the action outcomes...
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device':
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionStart
(
    IN PRV_TGF_FDB_ACTION_MODE_ENT  mode
);

/*******************************************************************************
* prvTgfBrgFdbActionDoneWait
*
* DESCRIPTION:
*       wait for the FDB triggered action to end
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       aaTaClosed - indication that the caller closed the AA,TA before the
*                   action -->
*                   GT_TRUE - the caller closed AA,TA before calling
*                               prvTgfBrgFdbActionStart(...)
*                             so action should be ended quickly.
*                             timeout is set to 50 milliseconds
*                   GT_FALSE - the caller NOT closed AA,TA before calling
*                               prvTgfBrgFdbActionStart(...)
*                             so action may take unpredictable time.
*                             timeout is set to 50 seconds !!!
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_TIMEOUT               - after timed out. see timeout definition
*                                  according to aaTaClosed parameter.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionDoneWait
(
    IN  GT_BOOL  aaTaClosed
);

/*******************************************************************************
* prvTgfBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* INPUTS:
*       mode - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionModeSet
(
    IN PRV_TGF_FDB_ACTION_MODE_ENT  mode
);

/******************************************************************************
* prvTgfBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*
* INPUTS:
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAgingTimeoutSet
(
    IN GT_U32 timeout
);

/*******************************************************************************
* prvTgfBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* INPUTS:
*       enable - GT_TRUE - enable refreshing
*                GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_BOOL enable
);

/*******************************************************************************
* prvTgfBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* INPUTS:
*       mode - action mode:
*                 PRV_TGF_ACT_AUTO_E - Action is done Automatically.
*                 PRV_TGF_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacTriggerModeSet
(
    IN PRV_TGF_MAC_ACTION_MODE_ENT  mode
);

/*******************************************************************************
* prvTgfBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* INPUTS:
*       enable - GT_TRUE -  Actions are enabled
*                GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionsEnableSet
(
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryStatusGet
(
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       index - hw mac entry index
*       age   - Age flag that is used for the two-step Aging process.
*               GT_FALSE - The entry will be aged out in the next pass.
*               GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryAgeBitSet
(
    IN GT_U32       index,
    IN GT_BOOL      age
);


/*******************************************************************************
* prvTgfBrgFdbPortGroupCount
*
* DESCRIPTION:
*       This function count number of valid ,skip entries - for specific port group
*
* INPUTS:
*       devNum  - physical device number.
*       portGroupId - port group Id
*           when CPSS_PORT_GROUP_UNAWARE_MODE_CNS meaning read from first port group
* OUTPUTS:
*       numOfValidPtr - (pointer to) number of entries with valid bit set
*                                   (number of valid entries)
*       numOfSkipPtr  - (pointer to) number of entries with skip bit set
*                                   (number of skipped entries)
*                                    entry not counted when valid = 0
*       numOfAgedPtr  - (pointer to) number of entries with age bit = 0 !!!
*                                   (number of aged out entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfStormPreventionPtr - (pointer to) number of entries with SP bit set
*                                   (number of SP entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfErrorsPtr - (pointer to) number of entries with read error
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbPortGroupCount
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,

    OUT GT_U32  *numOfValidPtr,
    OUT GT_U32  *numOfSkipPtr,
    OUT GT_U32  *numOfAgedPtr,
    OUT GT_U32  *numOfStormPreventionPtr,
    OUT GT_U32  *numOfErrorsPtr
);

/*******************************************************************************
* prvTgfBrgFdbCount
*
* DESCRIPTION:
*       This function count number of valid ,skip entries.
*
*       in multi ports port groups device the function summarize the entries
*       from all the port groups (don't care about duplications)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       numOfValidPtr - (pointer to) number of entries with valid bit set
*                                   (number of valid entries)
*       numOfSkipPtr  - (pointer to) number of entries with skip bit set
*                                   (number of skipped entries)
*                                    entry not counted when valid = 0
*       numOfAgedPtr  - (pointer to) number of entries with age bit = 0 !!!
*                                   (number of aged out entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfStormPreventionPtr - (pointer to) number of entries with SP bit set
*                                   (number of SP entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfErrorsPtr - (pointer to) number of entries with read error
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbCount
(
    OUT GT_U32  *numOfValidPtr,
    OUT GT_U32  *numOfSkipPtr,
    OUT GT_U32  *numOfAgedPtr,
    OUT GT_U32  *numOfStormPreventionPtr,
    OUT GT_U32  *numOfErrorsPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacCompare
*
* DESCRIPTION:
*       compare 2 mac entries.
*
* INPUTS:
*       mac1Ptr - (pointer to) mac 1
*       mac2Ptr - (pointer to) mac 2
*
* OUTPUTS:
*       > 0  - if mac1Ptr is  bigger than mac2Ptr
*       == 0 - if mac1Ptr is equal to mac2Ptr
*       < 0  - if mac1Ptr is smaller than mac2Ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_TIMEOUT      - on timeout waiting for the QR
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
int prvTgfBrgFdbMacCompare
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *mac1Ptr,
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *mac2Ptr
);

/*******************************************************************************
* prvTgfBrgFdbMacKeyCompare
*
* DESCRIPTION:
*       compare 2 mac keys.
*
* INPUTS:
*       key1Ptr - (pointer to) key 1
*       key2Ptr - (pointer to) key 2
*
* OUTPUTS:
*       > 0  - if key1Ptr is  bigger than key2Ptr
*       == 0 - if key1Ptr is equal to str2
*       < 0  - if key1Ptr is smaller than key2Ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_TIMEOUT      - on timeout waiting for the QR
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
int prvTgfBrgFdbMacKeyCompare
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *key1Ptr,
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *key2Ptr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryHashCalc
*
* DESCRIPTION:
*       calculate the hash index for the key
*
*       NOTE:
*       see also function prvTgfBrgFdbMacEntryIndexFind(...) that find actual
*       place according to actual HW capacity
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       indexPtr - (pointer to) index
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryHashCalc
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT GT_U32                       *indexPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryIndexFind
*
* DESCRIPTION:
*   function calculate hash index for this mac address , and then start to read
*       from HW the entries to find an existing entry that match the key.
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       indexPtr - (pointer to) :
*                   when GT_OK (found) --> index of the entry
*                   when GT_NOT_FOUND (not found) --> first index that can be used
*                   when GT_FULL (not found) --> index od start of bucket
*                   on other return value --> not relevant
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found, but bucket is not FULL
*       GT_FULL         - entry not found, but bucket is FULL
*       GT_BAD_PARAM    - wrong parameters.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryIndexFind
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT GT_U32                       *indexPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryFind
*
* DESCRIPTION:
*       scan the FDB table and find entry with same mac+vlan
*       entry MUST be valid and non-skipped
*
* INPUTS:
*       searchKeyPtr - (pointer to) the search key (mac+vlan)
* OUTPUTS:
*       indexPtr    - (pointer to) index
*                       when NULL --> ignored
*       agedPtr     - (pointer to) is entry aged
*                       when NULL --> ignored
*       devNumPtr   - (pointer to) is device number associated with the entry
*                       when NULL --> ignored
*       macEntryPtr - (pointer to) extended Mac table entry
*                       when NULL --> ignored
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_FAIL         - on error
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFind
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *searchKeyPtr,
    OUT GT_U32                       *indexPtr,
    OUT GT_BOOL                      *agedPtr,
    OUT GT_U8                        *devNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);


/*******************************************************************************
* prvTgfBrgFdbAccessModeSet
*
* DESCRIPTION:
*       Set access mode to the FDB : by message or by index
*
* INPUTS:
*       mode - access mode : by message or by index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       previous state
*
* COMMENTS:
*
*******************************************************************************/
PRV_TGF_BRG_FDB_ACCESS_MODE_ENT prvTgfBrgFdbAccessModeSet
(
    IN PRV_TGF_BRG_FDB_ACCESS_MODE_ENT  mode
);


/*******************************************************************************
* prvTgfBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device
*
* INPUTS:
*       vlanId - vlan Id
*       status - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanLearningStateSet
(
    IN GT_U16                         vlanId,
    IN GT_BOOL                        status
);

/*******************************************************************************
* prvTgfBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to the Multicast Group entry
*
* INPUTS:
*       devNum  - PP's device number
*       vidx    - multicast group index
*       portNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong devNum
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcMemberAdd
(
    IN GT_U8                          devNum,
    IN GT_U16                         vidx,
    IN GT_U8                          portNum
);

/*******************************************************************************
* prvTgfBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW
*
* INPUTS:
*       devNum        - PP's device number
*       vidx          - multicast group index
*       portBitmapPtr - pointer to the bitmap of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or ports bitmap value.
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcEntryWrite
(
    IN GT_U8                          devNum,
    IN GT_U16                         vidx,
    IN CPSS_PORTS_BMP_STC            *portBitmapPtr
);

/*******************************************************************************
* prvTgfBrgMcEntryRead
*
* DESCRIPTION:
*       Reads the Multicast Group entry from the HW
*
* INPUTS:
*       devNum - PP's device number.
*       vidx   - multicast group index
*
* OUTPUTS:
*       portBitmapPtr   - pointer to the bitmap of ports
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value.
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcEntryRead
(
    IN  GT_U8                         devNum,
    IN  GT_U16                        vidx,
    OUT CPSS_PORTS_BMP_STC           *portBitmapPtr
);

/*******************************************************************************
* prvTgfBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets
*
* INPUTS:
*       enable - enable\disable VLAN egress filtering
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEgressFilteringEnable
(
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - points to (enable/disable) bridged known unicast packets filtering
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong dev or port
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* prvTgfBrgCntDropCntrModeSet
*
* DESCRIPTION:
*       Sets Drop Counter Mode
*
* INPUTS:
*       dropMode - Drop Counter mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on HW error
*       GT_BAD_PARAM     - on wrong dropMode
*       GT_NOT_SUPPORTED - on non-supported dropMode
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrModeSet
(
    IN  PRV_TGF_BRG_DROP_CNTR_MODE_ENT dropMode
);

/*******************************************************************************
* prvTgfBrgCntDropCntrModeGet
*
* DESCRIPTION:
*       Gets the Drop Counter Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       dropModePtr - (pointer to) the Drop Counter mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on HW error
*       GT_BAD_PARAM     - on wrong parameters
*       GT_BAD_PTR       - on NULL pointer
*       GT_NOT_SUPPORTED - on non-supported dropMode
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrModeGet
(
    OUT  PRV_TGF_BRG_DROP_CNTR_MODE_ENT *dropModePtr
);

/*******************************************************************************
* prvTgfBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - enable\disable ingress filtering
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngFltEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfBrgCntDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device
*
* INPUTS:
*       dropCnt - the number of packets that were dropped
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrSet
(
    IN  GT_U32                        dropCnt
);

/*******************************************************************************
* prvTgfBrgCntDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device
*
* INPUTS:
*       None
*
* OUTPUTS:
*       dropCntPtr - (pointer to) the number of packets that were dropped
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrGet
(
    OUT GT_U32                       *dropCntPtr
);

/*******************************************************************************
* prvTgfBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* INPUTS:
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - enable/disable of local switching
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U16                                              vlanId,
    IN  PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT   trafficType,
    IN  GT_BOOL                                             enable
);

/*******************************************************************************
* prvTgfBrgGenUcLocalSwitchingEnable
*
* DESCRIPTION:
*       Enable/disable local switching back through the ingress interface
*       for for known Unicast packets
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk
*                            are assigned with a soft drop command.
*                 GT_TRUE  = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk may be
                             forwarded back to their source.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenUcLocalSwitchingEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfBrgPortEgressMcLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgPortEgressMcLocalEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range
*
* INPUTS:
*       vidRange - VID range for VLAN filtering (0 - 4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRangeSet
(
    IN  GT_U16                        vidRange
);

/*******************************************************************************
* prvTgfBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRangeGet
(
    OUT GT_U16                       *vidRangePtr
);


/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgDefVlanEntryWriteWithTaggingCmd
*
* DESCRIPTION:
*       Set vlan entry, with the all ports of the test as
*       in the vlan with specified tagging command
*
* INPUTS:
*       vlanId     - the vlan Id
*       taggingCmd - GT_TRUE - tagging command for all members
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWriteWithTaggingCmd
(
    IN GT_U16                             vlanId,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT  taggingCmd
);

/*******************************************************************************
* prvTgfBrgDefVlanEntryWrite
*
* DESCRIPTION:
*       Set vlan entry, with the all ports of the test as untagged in the vlan
*
* INPUTS:
*       vlanId - the vlan Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWrite
(
    IN GT_U16                         vlanId
);

/*******************************************************************************
* prvTgfBrgDefVlanEntryWithPortsSet
*
* DESCRIPTION:
*       Set vlan entry, with the requested ports, tagging
*
* INPUTS:
*       vlanId     - the vlan id
*       portsArray - array of ports
*       devArray   - array of devices (when NULL -> assume all ports belong to devNum)
*       tagArray   - array of tagging for the ports (when NULL -> assume all ports untagged)
*       numPorts   - number of ports in the arrays
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWithPortsSet
(
    IN GT_U16                         vlanId,
    IN GT_U8                          portsArray[],
    IN GT_U8                          devArray[],
    IN GT_U8                          tagArray[],
    IN GT_U32                         numPorts
);

/*******************************************************************************
* prvTgfBrgDefVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry and reset all it's content
*
* INPUTS:
*       vlanId - VLAN id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryInvalidate
(
    IN GT_U16               vlanId
);

/*******************************************************************************
* prvTgfBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       vlanId   - vlan id
*       mruIndex - MRU profile index [0..7]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMruProfileIdxSet
(
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
);

/*******************************************************************************
* prvTgfBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       mruIndex - MRU profile index [0..7]
*       mruValue - MRU value in bytes [0..0xFFFF]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMruProfileValueSet
(
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
);

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnPortSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on port
*
* INPUTS:
*       macAddr    - mac address
*       vlanId     - vlan Id
*       dstDevNum  - destination device
*       dstPortNum - destination port
*       isStatic   - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnPortSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U8                          dstDevNum,
    IN GT_U8                          dstPortNum,
    IN GT_BOOL                        isStatic
);

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnVidxSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on Vidx
*
* INPUTS:
*       macAddr    - mac address
*       vlanId     - vlan Id
*       vidx       - vidx number
*       isStatic   - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnVidxSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U16                         vidx,
    IN GT_BOOL                        isStatic
);

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnVidSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on Vlan ID
*
* INPUTS:
*       macAddr  - mac address
*       vlanId   - vlan Id
*       vid      - vlan Id
*       isStatic - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnVidSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U16                         vid,
    IN GT_BOOL                        isStatic
);

/*******************************************************************************
* prvTgfBrgVidxEntrySet
*
* DESCRIPTION:
*       Set vidx entry with the requested ports
*
* INPUTS:
*       vidx         - vidx number
*       portsArray   - array of ports
*       devicesArray - array of devices
*       numPorts     - number of ports in the arrays
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       When devicesArray is NULL assume all ports belong to devNum
*
*******************************************************************************/
GT_STATUS prvTgfBrgVidxEntrySet
(
    IN GT_U16                         vidx,
    IN GT_U8                          portsArray[],
    IN GT_U8                          devicesArray[],
    IN GT_U8                          numPorts
);

/*******************************************************************************
* prvTgfBrgVlanPvidSet
*
* DESCRIPTION:
*       Set port's Vlan Id to all tested ports
*
* INPUTS:
*       vlanId - vlan Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPvidSet
(
    IN GT_U16                         pvid
);

/*******************************************************************************
* prvTgfBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* INPUTS:
*       direction   - ingress/egress direction
*       entryIndex  - entry index for TPID table
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong entryIndex, direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTpidEntrySet
(
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    IN  GT_U16              etherType
);

/*******************************************************************************
* prvTgfBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* INPUTS:
*       direction  - ingress/egress direction
*       entryIndex - entry index for TPID table
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong entryIndex, direction
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTpidEntryGet
(
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    OUT GT_U16              *etherTypePtr
);

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent entries in the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
);

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32              *tpidBmpPtr
);

/*******************************************************************************
* prvTgfBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       ethMode        - TAG0/TAG1 selector
*       tpidEntryIndex - TPID table entry index (0-7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
);

/*******************************************************************************
* prvTgfBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag protocol ID) table per egress port.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number (CPU port is supported)
*       ethMode - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
);

/*******************************************************************************
* prvTgfBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* INPUTS:
*       brgMode - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanBridgingModeSet
(
    IN CPSS_BRG_MODE_ENT    brgMode
);

/*******************************************************************************
* prvTgfBrgGenIeeeReservedMcastTrapEnable
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU Multicast packets, with MAC_DA in
*       the IEEE reserved Multicast range (01-80-C2-00-00-xx).
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_FALSE -
*                   Trapping or mirroring to CPU of packet with
*                   MAC_DA = 01-80-C2-00-00-xx disabled.
*                GT_TRUE -
*                   Trapping or mirroring to CPU of packet, with
*                   MAC_DA = 01-80-C2-00-00-xx enabled according to the setting
*                   of IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
*                   by cpssDxChBrgGenIeeeReservedMcastProtCmdSet function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenIeeeReservedMcastTrapEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet
*
* DESCRIPTION:
*       Select the IEEE Reserved Multicast profile (table) associated with port
*
* INPUTS:
*       dev          - device number
*       port         - port number (including CPU port)
*       profileIndex - profile index (0..3). The parameter defines profile (table
*                      number) for the 256 protocols.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev, port
*       GT_OUT_OF_RANGE          - for profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet
(
    IN GT_U8                dev,
    IN GT_U8                port,
    IN GT_U32               profileIndex
);

/*******************************************************************************
* prvTgfBrgGenIeeeReservedMcastProtCmdSet
*
* DESCRIPTION:
*       Enables forwarding, trapping, or mirroring to the CPU any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* INPUTS:
*       dev          - device number
*       profileIndex - profile index (0..3). Parameter is relevant only for
*                      DxChXcat and above. The parameter defines profile (table
*                      number) for the 256 protocols. Profile bound per port by
*                      <prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_SOFT_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on bad device number, protocol or control packet command
*                      of profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       BPDU packets with MAC DA = 01-80-C2-00-00-00 are not affect
*       by this mechanism. BPDUs are implicitly trapped to the CPU
*       if the ingress port span tree is not disabled.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenIeeeReservedMcastProtCmdSet
(
    IN GT_U8                dev,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    IN CPSS_PACKET_CMD_ENT  cmd
);

/*******************************************************************************
* prvTgfBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* INPUTS:
*       mode - the assignment mode of the packet Source ID.
*              CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*              supported only for DxChXcat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
);

/*******************************************************************************
* prvTgfBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* INPUTS:
*       devNum          - device number
*       portNum         - Physical port number, CPU port
*       defaultSrcId    - Port's Default Source ID (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U32  defaultSrcId
);

/*******************************************************************************
* prvTgfBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGroupPortDelete
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
);

/*******************************************************************************
* prvTgfBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGroupPortAdd
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
);

/*******************************************************************************
* prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       mode - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   mode
);

/*******************************************************************************
* prvTgfBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       None
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRemoveVlanTag1IfZeroModeGet
(
    OUT PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   *modePtr
);

/*******************************************************************************
* prvTgfBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       devTableArr[4] - (array of) bmp of devices to set.
*                       DxCh devices use only devTableBmp[0]
*                       ExMxPm devices use devTableBmp[0..3]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbDeviceTableSet
(
    IN  GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS]
);

/*******************************************************************************
* prvTgfBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon.
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it, the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       none
*
* OUTPUTS:
*       devTableArr - pointer to (Array of) bmp (bitmap) of devices.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbDeviceTableGet
(
    OUT GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS]
);

/*******************************************************************************
* prvTgfBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* prvTgfBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* prvTgfBrgVlanTr101Check
*
* DESCRIPTION:
*    check if the device supports the tr101
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the tr101
*       GT_FALSE - the device not supports the tr101
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfBrgVlanTr101Check(
    void
);

/*******************************************************************************
* prvTgfBrgGenPortIeeeReservedMcastLearningEnableSet
*
* DESCRIPTION:
*     Enable/Disable MAC SA learning on reserved IEEE MC packets, that are
*     trapped to CPU.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*        devNum         - device number
*        portNum        - port number 
*        enable         - GT_TRUE - learning is performed also for IEEE MC
*                               trapped packets.
*                       - GT_FALSE - No learning is performed for IEEE MC
*                               trapped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortIeeeReservedMcastLearningEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* prvTgfBrgGenRateLimitGlobalCfgSet
*
* DESCRIPTION:
*       Configures global ingress rate limit parameters - rate limiting mode,
*       Four configurable global time window periods are assigned to ports as
*       a function of their operating speed:
*       10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* INPUTS:
*       devNum         - device number
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   Supported windows for DxCh devices:
*       1000 Mbps: range - 256-16384 uSec    granularity - 256 uSec
*       100  Mbps: range - 256-131072 uSec   granularity - 256 uSec
*       10   Mbps: range - 256-1048570 uSec  granularity - 256 uSec
*       10   Gbps: range - 25.6-104857 uSec  granularity - 25.6 uSec
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenRateLimitGlobalCfgSet
(
    IN GT_U8                            devNum,
    IN PRV_TGF_BRG_GEN_RATE_LIMIT_STC   *brgRateLimitPtr
);

/*******************************************************************************
* prvTgfBrgGenRateLimitGlobalCfgGet
*
* DESCRIPTION:
*     Get global ingress rate limit parameters
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenRateLimitGlobalCfgGet
(
    IN  GT_U8                            devNum,
    OUT PRV_TGF_BRG_GEN_RATE_LIMIT_STC   *brgRateLimitPtr
);

/*******************************************************************************
* prvTgfBrgGenPortRateLimitSet
*
* DESCRIPTION:
*       Configures port ingress rate limit parameters
*       Each port maintains rate limits for unknown unicast packets,
*       known unicast packets, multicast packets and broadcast packets,
*       single configurable limit threshold value, and a single internal counter.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_OUT_OF_RANGE          - on out of range rate limit values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortRateLimitSet
(
    IN  GT_U8                                   devNum,
    IN GT_U8                                    port,
    IN PRV_TGF_BRG_GEN_RATE_LIMIT_PORT_STC      *portGfgPtr
);


/*******************************************************************************
* prvTgfBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       for multi-port groups device :
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
);


/*******************************************************************************
* prvTgfBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU 
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from 
*                          Tag1 VLAN. 
*                 GT_FALSE: vid1 field of the NA AU message is taken from 
*                          Tag0 VLAN. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfBridgeGenh */


