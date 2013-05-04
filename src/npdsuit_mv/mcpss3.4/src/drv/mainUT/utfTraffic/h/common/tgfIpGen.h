/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfIpGen.h
*
* DESCRIPTION:
*       Generic API for IP
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfIpGenh
#define __tgfIpGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfInlifGen.h>
#include <common/tgfMplsGen.h>
#include <common/tgfPclGen.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
    #include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
    #include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
    #include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpStat.h>
    #include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTables.h>
    #include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
    #include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpCtrl.h>
    #include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpm.h>
    #include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * Typedef: enum PRV_TGF_IP_ROUTING_MODE_ENT
 *
 * Description: enumerator for IP_TCAM_ROUTE_ENTRY_INFO
 * see CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT
 *
 * Fields:
 *  PRV_TGF_IP_ROUTE_ENTRY_INFO_PCL_ACTION_E   - for Policy based routing mode
 *                     where the routing is done through PCL
 *  PRV_TGF_IP_ROUTE_ENTRY_INFO_IP_LTT_ENTRY_E - for LTT based routing mode
 *                     where the routing is done through LTT
 *  PRV_TGF_IP_ROUTING_MODE_UNDEFINED_E        - on the case of fail of detection
 */
typedef enum
{
    PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E,
    PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E,

    PRV_TGF_IP_ROUTING_MODE_UNDEFINED_E
} PRV_TGF_IP_ROUTING_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT
 *
 * Description: Each route entry block has a type.
 *
 * Enumerations:
 *      PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E - Equal-Cost-MultiPath block
 *      PRV_TGF_IP_COS_ROUTE_ENTRY_GROUP_E  - Class-of-Service block
 *      PRV_TGF_IP_REGULAR_ROUTE_ENTRY_E    - single next hop entry
 *
 */
typedef enum
{
    PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E,
    PRV_TGF_IP_COS_ROUTE_ENTRY_GROUP_E,
    PRV_TGF_IP_REGULAR_ROUTE_ENTRY_E

} PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT;

/*
 * typedef: enum PRV_TGF_RPF_IF_MODE_ENT
 *
 * Description:
 *      Different modes for unicast RPF check
 *
 * Enumerations:
 *      PRV_TGF_RPF_IF_MODE_DISABLED_E - RPF check is bypassed
 *      PRV_TGF_RPF_IF_MODE_PORT_E     - packet source port/trunk is compared to the entry <RPF interface>
 *      PRV_TGF_RPF_IF_MODE_L2_VLAN_E  - packet's VLAN is compared to the entry <RPF interface>
 *
 */
typedef enum
{
    PRV_TGF_RPF_IF_MODE_DISABLED_E = 0,
    PRV_TGF_RPF_IF_MODE_PORT_E     = 1,
    PRV_TGF_RPF_IF_MODE_L2_VLAN_E  = 2

} PRV_TGF_RPF_IF_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_HOP_LIMIT_MODE_ENT
 *
 * Description: Enumeration of Hop Limit Manipulation mode
 *
 * Enumerations:
 *  PRV_TGF_IP_HOP_LIMIT_NOT_DECREMENTED_E - Hop Limit of routed packets
 *                                           associated with this Next Hop
 *                                           Entry is not decremented when
 *                                           the packet is transmitted from
 *                                           this device.
 *  PRV_TGF_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E - Hop Limit of routed packets
 *                                              associated with this Next Hop
 *                                              Entry is decremented by 1 when
 *                                              the packet is transmitted from
 *                                              this device.
 *
 */
typedef enum
{
    PRV_TGF_IP_HOP_LIMIT_NOT_DECREMENTED_E    = 0,
    PRV_TGF_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E = 1
} PRV_TGF_IP_HOP_LIMIT_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_TTL_MODE_ENT
 *
 * Description: Enumeration of TTL Manipulation mode.
 *
 * Enumerations:
 *
 *  PRV_TGF_IP_TTL_NOT_DECREMENTED_E = TTL of routed packets associated
 *              with this Next Hop Entry is not decremented when the packet is
 *              transmitted from this device.
 *  PRV_TGF_IP_TTL_DECREMENTED_BY_ONE_E = TTL of routed packets associated
 *              with this Next Hop Entry is decremented by 1 when the packet is
 *              transmitted from this device.
 *  PRV_TGF_IP_TTL_DECREMENTED_BY_ENTRY_E = TTL of routed packets
 *              associated with this Next Hop Entry is decremented by <TTL> when
 *              the packet is transmitted from this device.
 *  PRV_TGF_IP_TTL_COPY_ENTRY_E = TTL of routed packets associated with
 *              this Next Hop Entry is assigned by this entry to a new TTL
 *              value, when the packet is transmitted from this device.
 *
 */
typedef enum
{
    PRV_TGF_IP_TTL_NOT_DECREMENTED_E      = 0,
    PRV_TGF_IP_TTL_DECREMENTED_BY_ONE_E   = 1,
    PRV_TGF_IP_TTL_DECREMENTED_BY_ENTRY_E = 2,
    PRV_TGF_IP_TTL_COPY_ENTRY_E           = 3
} PRV_TGF_IP_TTL_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_MC_RPF_FAIL_COMMAND_MODE_ENT (type)
 *
 * Description: decides on the way the RPF fail command is chosen when an RPF
 *              fail occurs:
 *
 * Enumerations:
 *      PRV_TGF_IP_MC_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E - use the MC route entry
 *      PRV_TGF_IP_MC_MLL_RPF_FAIL_COMMAND_MODE_E         - Use the MLL entry
 *
 */
typedef enum
{
    PRV_TGF_IP_MC_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E,
    PRV_TGF_IP_MC_MLL_RPF_FAIL_COMMAND_MODE_E
} PRV_TGF_IP_MC_RPF_FAIL_COMMAND_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_MLL_PAIR_READ_WRITE_FORM_ENT
 *
 * Description: the possible forms to write the MLL Pair entry
 *
 * Enumerations:
 *  PRV_TGF_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E - reads/writes just the
 *                                                    first mll of the pair
 *  PRV_TGF_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E - reads/writes
 *                         just the second Mll of the pair and the next pointer
 *  PRV_TGF_IP_MLL_PAIR_READ_WRITE_WHOLE_E - reads/writes the whole mll pair
 */
typedef enum
{
    PRV_TGF_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E,
    PRV_TGF_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E,
    PRV_TGF_IP_MLL_PAIR_READ_WRITE_WHOLE_E
} PRV_TGF_IP_MLL_PAIR_READ_WRITE_FORM_ENT;

/*
 * Typedef: struct PRV_TGF_IP_UC_ROUTE_ENTRY_STC
 *
 * Description: Unicast Route Entry
 *
 * Fields:
 *  cmd                - Route entry command
 *  cpuCodeIdx         - the cpu code index
 *  appSpecificCpuCodeEnable- Enable CPU code overwritting
 *  unicastPacketSipFilterEnable - Enable: Assign the command HARD DROP if the
 *                       SIP lookup matches this entry
 *                       Disable: Do not assign the command HARD DROP due to
 *                       the SIP lookup matching this entry
 *  ttlHopLimitDecEnable - Enable TTL/Hop Limit Decrement
 *  ttlHopLimDecOptionsExtChkByPass- TTL/HopLimit Decrement and option/
 *                       Extention check bypass.
 *  ingressMirror      - mirror to ingress analyzer.
 *  qosProfileMarkingEnable- Enable Qos profile assignment.
 *  qosProfileIndex    - the qos profile
 *  qosPrecedence      - whether this packet Qos parameters can be overridden
 *                       after this assigment.
 *  modifyUp           - whether to change the packets UP and how.
 *  modifyDscp         - whether to change the packets DSCP and how.
 *  countSet           - The counter set this route entry is linked to
 *  trapMirrorArpBcEnable - enable Trap/Mirror ARP Broadcasts with DIP matching
 *                       this entry
 *  sipAccessLevel     - The security level associated with the SIP.
 *  dipAccessLevel     - The security level associated with the DIP.
 *  ICMPRedirectEnable - Enable performing ICMP Redirect Exception Mirroring.
 *  scopeCheckingEnable- Enable IPv6 Scope Checking.
 *  siteId             - The site id of this route entry.
 *  mtuProfileIndex    - One of the eight global configurable MTU sizes (0..7).
 *  isTunnelStart      - weather this nexthop is tunnel start enrty
 *  nextHopVlanId      - the output vlan id
 *  nextHopInterface   - the output interface this next hop sends to
 *  nextHopARPPointer  - The ARP Pointer indicating the routed packet MAC DA
 *  nextHopTunnelPointer- the tunnel pointer
 *
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT     cmd;
    GT_U32                  cpuCodeIndex;
    GT_BOOL                 appSpecificCpuCodeEnable;
    GT_BOOL                 unicastPacketSipFilterEnable;
    GT_BOOL                 ingressMirror;
    CPSS_IP_CNT_SET_ENT     countSet;
    GT_BOOL                 trapMirrorArpBcEnable;
    GT_U32                  sipAccessLevel;
    GT_U32                  dipAccessLevel;
    GT_BOOL                 ICMPRedirectEnable;
    GT_BOOL                 scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT     siteId;
    GT_U32                  mtuProfileIndex;
    GT_U16                  nextHopVlanId;

    GT_BOOL                                     ttlHopLimitDecEnable;
    GT_BOOL                                     ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                                     qosProfileMarkingEnable;
    GT_U32                                      qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    GT_BOOL                                     isTunnelStart;
    CPSS_INTERFACE_INFO_STC                     nextHopInterface;
    GT_U32                                      nextHopARPPointer;
    GT_U32                                      nextHopTunnelPointer;

    GT_BOOL                                 bypassTtlExceptionCheckEnable;
    GT_BOOL                                 ageRefresh;
    PRV_TGF_RPF_IF_MODE_ENT                 ucRpfOrIcmpRedirectIfMode;
    GT_BOOL                                 sipSaCheckEnable;
    PRV_TGF_OUTLIF_INFO_STC                 nextHopIf;
    PRV_TGF_IP_HOP_LIMIT_MODE_ENT           hopLimitMode;
    GT_U32                                  ttl;
    PRV_TGF_IP_TTL_MODE_ENT                 ttlMode;
    GT_U32                                  mplsLabel;
    PRV_TGF_MPLS_CMD_ENT                    mplsCommand;
    PRV_TGF_QOS_PARAM_MODIFY_STC            qosParamsModify;
    PRV_TGF_QOS_PARAM_STC                   qosParams;
} PRV_TGF_IP_UC_ROUTE_ENTRY_STC;

/*
 * Typedef: struct PRV_TGF_IP_ROUTE_ENTRY_STC
 *
 * Description: LTT (lookup translation table) entry
 *
 * Fields:
 *      routeType              - Route Entry Type
 *      numOfPaths             - The number of route paths bound to the lookup
 *      routeEntryBaseIndex    - index to the single Route entry
 *      blockSize              - the size of the route entries group
 *      ucRPFCheckEnable         - Enable Unicast RPF check for this Entry
 *      sipSaCheckMismatchEnable - Enable Unicast SIP MAC SA match check
 *      ipv6MCGroupScopeLevel  - This is the IPv6 Multicast group scope level
 *
 */
typedef struct
{
    PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT   routeType;
    GT_U32                              numOfPaths;
    GT_U32                              routeEntryBaseIndex;
    GT_U32                              blockSize;
    GT_BOOL                             ucRPFCheckEnable;
    GT_BOOL                             sipSaCheckMismatchEnable;
    CPSS_IPV6_PREFIX_SCOPE_ENT          ipv6MCGroupScopeLevel;
} PRV_TGF_IP_ROUTE_ENTRY_STC;

/*
 * Typedef: struct PRV_TGF_IP_COUNTER_SET_STC
 *
 * Description: IPvX Counter Set
 *
 * Fields:
 *      inUcPkts                 - number of ingress unicast packets
 *      inMcPkts                 - number of ingress multicast packets
 *      inUcNonRoutedExcpPkts    - number of ingress Unicast packets that were
 *                                 not routed due to one or more exceptions.
 *      inUcNonRoutedNonExcpPkts - number of ingress Unicast packets that were
 *                                 not routed but didn't recive any excption.
 *      inMcNonRoutedExcpPkts    - number of ingress multicast packets that were
 *                                 not routed due to one or more exceptions.
 *      inMcNonRoutedNonExcpPkts - number of ingress multicast packets that were
 *                                 not routed but didn't recive any excption.
 *      inUcTrappedMirrorPkts    - number of ingress unicast packets that were
 *                                 trapped or mirrored by the router engine.
 *      inMcTrappedMirrorPkts    - number of ingress multicast packets that were
 *                                 trapped or mirrored by the router engine.
 *      mcRfpFailPkts            - number of multicast packets with fail RPF.
 *      outUcRoutedPkts          - the number of egress unicast routed packets.
 */
typedef struct PRV_TGF_IP_COUNTER_SET_STCT
{
    GT_U32 inUcPkts;
    GT_U32 inMcPkts;
    GT_U32 inUcNonRoutedExcpPkts;
    GT_U32 inUcNonRoutedNonExcpPkts;
    GT_U32 inMcNonRoutedExcpPkts;
    GT_U32 inMcNonRoutedNonExcpPkts;
    GT_U32 inUcTrappedMirrorPkts;
    GT_U32 inMcTrappedMirrorPkts;
    GT_U32 mcRfpFailPkts;
    GT_U32 outUcRoutedPkts;
} PRV_TGF_IP_COUNTER_SET_STC;

/*
 * Typedef: struct PRV_TGF_IP_LTT_ENTRY_STC
 *
 * Description: LTT (lookup translation table) entry
 *
 * Fields:
 *      routeType                - route Entry Type.
 *      numOfPaths               - the number of route paths bound to the lookup
 *      routeEntryBaseIndex      - index to Route entry
 *      ucRPFCheckEnable         - enable Unicast RPF check for this Entry
 *      sipSaCheckMismatchEnable - enable Unicast SIP MAC SA match check
 *      ipv6MCGroupScopeLevel    - this is the IPv6 Multicast group scope level
 *      blockSize                - the size of the route entries group
 *
 */
typedef struct
{
    PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT   routeType;
    GT_U32                              numOfPaths;
    GT_U32                              routeEntryBaseIndex;
    GT_BOOL                             ucRPFCheckEnable;
    GT_BOOL                             sipSaCheckMismatchEnable;
    CPSS_IPV6_PREFIX_SCOPE_ENT          ipv6MCGroupScopeLevel;
    GT_U32                              blockSize;
} PRV_TGF_IP_LTT_ENTRY_STC;

/*
 * typedef: struct PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT
 *
 * Description:
 *      This union holds the two possible accosiactions a prefix can have
 *      for Cheetha+ devices it will be a PCL action.
 *      for Cheetha2 devices it will be a LTT entry.
 *
 * Fields:
 *  pclIpUcActionPtr - a pointer to PCL action
 *  ipLttEntryPtr    - a pointer to a LTT entry
 *
 */
typedef union
{
    PRV_TGF_PCL_ACTION_STC        pclIpUcAction;
    PRV_TGF_IP_LTT_ENTRY_STC      ipLttEntry;
} PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT;

/*
 * Typedef: struct PRV_TGF_IPV4_PREFIX_STC
 *
 * Description: IPv4 prefix entry
 *
 * Fields:
 *      vrId               - virtual router id
 *      ipAddr             - the IP address of this prefix
 *      isMcSource         - enable\disable writing MC source ip
 *      mcGroupIndexRow    - mc Group index row
 *      mcGroupIndexColumn - mc Group index column
 *
 */
typedef struct
{
    GT_U32      vrId;
    GT_IPADDR   ipAddr;
    GT_BOOL     isMcSource;
    GT_U32      mcGroupIndexRow;
    GT_U32      mcGroupIndexColumn;
} PRV_TGF_IPV4_PREFIX_STC;

/*
 * Typedef: struct PRV_TGF_IPV6_PREFIX_STC
 *
 * Description: IPv6 prefix entry
 *
 * Fields:
 *      vrId            - virtual router id
 *      ipAddr          - the IP address of this prefix
 *      isMcSource      - enable\disable writing MC source ip
 *      mcGroupIndexRow - mc Group index row
 *
 */
typedef struct
{
    GT_U32      vrId;
    GT_IPV6ADDR ipAddr;
    GT_BOOL     isMcSource;
    GT_U32      mcGroupIndexRow;
} PRV_TGF_IPV6_PREFIX_STC;

/*
 * Typedef: struct PRV_TGF_IP_MC_ROUTE_ENTRY_STC
 *
 * Description:     Representation of the IP_MC_ROUTE_ENTRY in HW,
 *
 * Fields:
 *  cmd                - Route entry command
 *  cpuCodeIdx         - the cpu code index
 *  appSpecificCpuCodeEnable- Enable CPU code overwritting
 *  ttlHopLimitDecEnable - Enable TTL/Hop Limit Decrement
 *  ttlHopLimDecOptionsExtChkByPass- TTL/HopLimit Decrement
 *  ingressMirror      - mirror to ingress analyzer
 *  qosProfileMarkingEnable- Enable Qos profile assignment
 *  qosProfileIndex    - the qos profile
 *  qosPrecedence      - whether this packet Qos parameters can be overridden
 *  modifyUp           - whether to change the packets UP and how
 *  modifyDscp         - whether to change the packets DSCP and how
 *  countSet           - The counter set this route entry is linked to
 *  multicastRPFCheckEnable  - Enable the Multicast RPF check
 *  multicastRPFVlan   - RPF check VLAN-ID
 *  multicastRPFFailCommandMode - MC RPF fail command mode
 *  RPFFailCommand     - the route entry RPF fail command
 *  scopeCheckingEnable- Enable IPv6 Scope Checking
 *  siteId             - The site id of this route entry
 *  mtuProfileIndex    - One of the eight global configurable MTU sizes
 *  internalMLLPointer - pointer of the first Mll entry
 *  externalMLLPointer - pointer of the first external Mll entry
 *
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                         cmd;
    GT_U32                                      cpuCodeIndex;
    GT_BOOL                                     appSpecificCpuCodeEnable;
    GT_BOOL                                     ttlHopLimitDecEnable;
    GT_BOOL                                     ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                                     ingressMirror;
    GT_BOOL                                     qosProfileMarkingEnable;
    GT_U32                                      qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_IP_CNT_SET_ENT                         countSet;
    GT_BOOL                                     multicastRPFCheckEnable;
    GT_U16                                      multicastRPFVlan;
    PRV_TGF_IP_MC_RPF_FAIL_COMMAND_MODE_ENT     mcRPFFailCmdMode;
    CPSS_PACKET_CMD_ENT                         RPFFailCommand;
    GT_BOOL                                     scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                         siteId;
    GT_U32                                      mtuProfileIndex;
    GT_U32                                      internalMLLPointer;
    GT_U32                                      externalMLLPointer;
    GT_BOOL                                     bypassTtlExceptionCheckEnable;
    GT_BOOL                                     ageRefresh;
    GT_U32                                      ipv6ScopeLevel;
    PRV_TGF_RPF_IF_MODE_ENT                     mcRpfIfMode;
    CPSS_INTERFACE_INFO_STC                     mcRpfIf;
    PRV_TGF_OUTLIF_INFO_STC                     nextHopIf;
    PRV_TGF_IP_HOP_LIMIT_MODE_ENT               hopLimitMode;
    PRV_TGF_QOS_PARAM_MODIFY_STC                qosParamsModify;
    PRV_TGF_QOS_PARAM_STC                       qosParams;
    GT_U32                                      ttl;
    PRV_TGF_IP_TTL_MODE_ENT                     ttlMode;
    GT_U32                                      mplsLabel;
    PRV_TGF_MPLS_CMD_ENT                        mplsCommand;
} PRV_TGF_IP_MC_ROUTE_ENTRY_STC;

/*
 * Typedef: struct PRV_TGF_IP_MLL_STC
 *
 * Description:     Representation of the CPSS_DXCH_IP_MLL_STC in HW,
 *
 * Fields:
 *
 *  mllRPFFailCommand    - the MLL entry RPF fail command
 *  isTunnelStart        - weather this nexthop is tunnel start enrty
 *  nextHopInterface     - the output interface this mll entry sends to
 *  nextHopVlanId        - VLAN-ID used for the downstream next hop
 *  nextHopTunnelPointer - the tunnel pointer in case this is a tunnel start
 *  ttlHopLimitThreshold - minimal hop limit
 *  excludeSrcVlan       - if GT_TRUE then if the packet's vlan equals the mll
 *                         entry vlan id then skip this entry.
 *  last                 - if GT_TRUE then there are more mll entries in this
 *                         list if GT_FALSE this is the end.
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT     mllRPFFailCommand;
    GT_BOOL                 isTunnelStart;
    CPSS_INTERFACE_INFO_STC nextHopInterface;
    GT_U16                  nextHopVlanId;
    GT_U32                  nextHopTunnelPointer;
    GT_U16                  ttlHopLimitThreshold;
    GT_BOOL                 excludeSrcVlan;
    GT_BOOL                 last;
} PRV_TGF_IP_MLL_STC;

/*
 * Typedef: struct PRV_TGF_IP_MLL_PAIR_STC
 *
 * Description:     Representation of the IP muticast Link List pair in HW
 *
 * Fields:
 *  firstMllNode  - the first Mll of the pair
 *  secondMllNode - the second Mll of the pair
 *  nextPointer   - pointer to the next MLL entry
 *
 */
typedef struct
{
    PRV_TGF_IP_MLL_STC      firstMllNode;
    PRV_TGF_IP_MLL_STC      secondMllNode;
    GT_U16                  nextPointer;
} PRV_TGF_IP_MLL_PAIR_STC;

/*
 * typedef: enum PRV_TGF_IP_PORT_TRUNK_CNT_MODE_ENT
 *
 * Description: the counter Set Port/Trunk mode
 *
 * Enumerations:
 *  PRV_TGF_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E - This counter-set counts all
 *                                                 packets regardless of their
 *                                                 In / Out port or Trunk.
 *  PRV_TGF_IP_PORT_CNT_MODE_E - This counter-set counts packets Received /
 *                                 Transmitted via binded Port + Dev.
 *  PRV_TGF_IP_TRUNK_CNT_MODE_E - This counter-set counts packets Received /
 *                                 Transmitted via binded Trunk.
 */
typedef enum
{
    PRV_TGF_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E = 0,
    PRV_TGF_IP_PORT_CNT_MODE_E                 = 1,
    PRV_TGF_IP_TRUNK_CNT_MODE_E                = 2
}PRV_TGF_IP_PORT_TRUNK_CNT_MODE_ENT;

/*
 * typedef: enum PRV_TGF_IP_VLAN_CNT_MODE_ENT
 *
 * Description: the counter Set vlan mode
 *
 * Enumerations:
 *  PRV_TGF_IP_DISREGARD_VLAN_CNT_MODE_E - This counter-set counts all packets
 *                                           regardless of their In / Out vlan.
 *  PRV_TGF_IP_USE_VLAN_CNT_MODE_E - This counter-set counts packets Received
 *                                     / Transmitted via binded vlan.
 */
typedef enum
{
    PRV_TGF_IP_DISREGARD_VLAN_CNT_MODE_E = 0,
    PRV_TGF_IP_USE_VLAN_CNT_MODE_E       = 1
}PRV_TGF_IP_VLAN_CNT_MODE_ENT;

/*
 * Typedef: struct PRV_TGF_IP_COUNTER_SET_INTERFACE_CFG_STC
 *
 * Description: IPvX Counter Set interface mode configuration
 *
 * Fields:
 *      portTrunkCntMode - the counter Set Port/Trunk mode
 *      ipMode           - the cnt set IP interface mode , which protocol stack
 *                         ipv4 ,ipv6 or both (which is actully disregarding the
 *                         ip protocol)
 *      vlanMode         - the counter Set vlan mode.
 *      devNum           - the binded devNum , relevant only if
 *                         portTrunkCntMode = PRV_TGF_IP_PORT_CNT_MODE_E
 *      port             - the binded por , relevant only if
 *                         portTrunkCntMode = PRV_TGF_IP_PORT_CNT_MODE_E
 *      trunk            - the binded trunk , relevant only if
 *                         portTrunkCntMode = PRV_TGF_IP_TRUNK_CNT_MODE_E
 *      portTrunk        - the above port/trunk
 *      vlanId           - the binded vlan , relevant only if
 *                         vlanMode = PRV_TGF_IP_USE_VLAN_CNT_MODE_E
 */
typedef struct PRV_TGF_IP_COUNTER_SET_INTERFACE_CFG_STCT
{
    PRV_TGF_IP_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;
    CPSS_IP_PROTOCOL_STACK_ENT           ipMode;
    PRV_TGF_IP_VLAN_CNT_MODE_ENT       vlanMode;
    GT_U8                                devNum;
    union
    {
        GT_U8                            port;
        GT_TRUNK_ID                      trunk;
    }portTrunk;
    GT_U16                               vlanId;
}PRV_TGF_IP_COUNTER_SET_INTERFACE_CFG_STC;

/*
 * typedef: struct PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC
 *
 * Description:
 *      This struct hold the capacity configuration required from the Tcam LPM
 *      manager. According to these requirements the TCAM entries will be
 *      fragmented.
 *
 * Fields:
 *  numOfIpv4Prefixes           - The total number of Ipv4 Uc prefixes and
 *                                ipv4 MC group prefixes required.
 *  numOfIpv4McSourcePrefixes   - The number of Ipv4 MC source prefixes required.
 *  numOfIpv6Prefixes           - The total number of Ipv6 Uc prefixes,
 *                                ipv6 MC group prefixes and ipv6 MC sources
 *                                prefixes required.
 */
typedef struct PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STCT
{
    GT_U32  numOfIpv4Prefixes;
    GT_U32  numOfIpv4McSourcePrefixes;
    GT_U32  numOfIpv6Prefixes;
}PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC;

/*
 * typedef: struct PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC
 *
 * Description:
 *      This struct hold the indexes available for the Tcam LPM manager.
 *      it defines the From-To indexes in the TCAM the lpm manager can use.
 *
 * Fields:
 *  firstIndex - this is the first index availble (from)
 *  lastIndex  - this is the last index availble (to - including)
 *
 */
typedef struct PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STCT
{
    GT_U32  firstIndex;
    GT_U32  lastIndex;
}PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC;


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfIpRoutingModeGet
*
* DESCRIPTION:
*       Return the current routing mode
*
* INPUTS:
*       none
*
* OUTPUTS:
*       routerModePtr - the current routing mode
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on devNum not active.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRoutingModeGet
(
    OUT PRV_TGF_IP_ROUTING_MODE_ENT *routerModePtr
);

/*******************************************************************************
* prvTgfIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw
*
* INPUTS:
*       devNum              - the device number
*       baseRouteEntryIndex - the index from which to write the array
*       routeEntriesArray   - the uc route entries array
*       numOfRouteEntries   - the number route entries in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on devNum not active.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpUcRouteEntriesWrite
(
    IN GT_U32                         baseRouteEntryIndex,
    IN PRV_TGF_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                         numOfRouteEntries
);

/*******************************************************************************
* prvTgfIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw
*
* INPUTS:
*       devNum              - the device number
*       baseRouteEntryIndex - the index from which to start reading
*       routeEntriesArray   - the uc route entries array
*       numOfRouteEntries   - the number route entries in the array
*
* OUTPUTS:
*       routeEntriesArray - the uc route entries array read
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpUcRouteEntriesRead
(
    IN    GT_U8                          devNum,
    IN    GT_U32                         baseRouteEntryIndex,
    INOUT PRV_TGF_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN    GT_U32                         numOfRouteEntries
);

/*******************************************************************************
* prvTgfIpRouterArpAddrWrite
*
* DESCRIPTION:
*    Write a ARP MAC address to the router ARP / Tunnel start Table
*
* INPUTS:
*       devNum         - the device number
*       routerArpIndex - The Arp Address index
*       arpMacAddrPtr  - the ARP MAC address to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on devNum not active.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - request is not supported for this device type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterArpAddrWrite
(
    IN GT_U32                         routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
);

/*******************************************************************************
* prvTgfIpRouterArpAddrRead
*
* DESCRIPTION:
*    Read a ARP MAC address from the router ARP / Tunnel start Table
*
* INPUTS:
*       devNum         - the device number
*       routerArpIndex - The Arp Address index
*
* OUTPUTS:
*       arpMacAddrPtr - the ARP MAC address to read
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on devNum not active
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - request is not supported for this device type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterArpAddrRead
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
);

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix
*
* INPUTS:
*       lpmDBId        - The LPM DB id
*       vrId           - The virtual router id
*       ipAddr         - The destination IP address of this prefix
*       prefixLen      - The number of bits that are actual valid in the ipAddr
*       nextHopInfoPtr - the route entry info accosiated with this UC prefix
*       override       - override an existing entry for this mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_OUT_OF_RANGE   - on prefix length is too big
*       GT_ERROR          - on the vrId was not created yet
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory
*       GT_FAIL           - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                lpmDBId,
    IN GT_U32                                vrId,
    IN GT_IPADDR                             ipAddr,
    IN GT_U32                                prefixLen,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_BOOL                               override
);

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM DB
*
* INPUTS:
*       lpmDBId   - The LPM DB id
*       vrId      - The virtual router id
*       ipAddr    - The destination IP address of the prefix
*       prefixLen - The number of bits that are actual valid in the ipAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - prefix length is too big
*       GT_ERROR        - vrId was not created yet
*       GT_NO_SUCH      - given prefix doesn't exitst in the VR
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixDel
(
    IN GT_U32                         lpmDBId,
    IN GT_U32                         vrId,
    IN GT_IPADDR                      ipAddr,
    IN GT_U32                         prefixLen
);

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* INPUTS:
*       lpmDBId - The LPM DB id
*       vrId    - The virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix
*
* INPUTS:
*       lpmDBId        - The LPM DB id
*       vrId           - The virtual router id
*       ipAddr         - The destination IP address of this prefix
*       prefixLen      - The number of bits that are actual valid in the ipAddr
*       nextHopInfoPtr - the route entry info accosiated with this UC prefix
*       override       - override an existing entry for this mask
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                lpmDBId,
    IN GT_U32                                vrId,
    IN GT_IPV6ADDR                           ipAddr,
    IN GT_U32                                prefixLen,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_BOOL                               override,
    IN GT_BOOL                               defragmentationEnable
);

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM DB
*
* INPUTS:
*       lpmDBId   - The LPM DB id
*       vrId      - The virtual router id
*       ipAddr    - The destination IP address of the prefix
*       prefixLen - The number of bits that are actual valid in the ipAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - prefix length is too big
*       GT_ERROR                 - vrId was not created yet
*       GT_NO_SUCH               - given prefix doesn't exitst in the VR
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixDel
(
    IN GT_U32                         lpmDBId,
    IN GT_U32                         vrId,
    IN GT_IPV6ADDR                    ipAddr,
    IN GT_U32                         prefixLen
);

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* INPUTS:
*       lpmDBId - The LPM DB id
*       vrId    - The virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* prvTgfCountersIpSet
*
* DESCRIPTION:
*       Set route entry mode and reset IP couters
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       counterIndex - counter index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCountersIpSet
(
    IN GT_U8                          portNum,
    IN GT_U32                         counterIndex
);

/*******************************************************************************
* prvTgfCountersIpGet
*
* DESCRIPTION:
*       This function gets the values of the various counters in the IP
*       Router Management Counter-Set.
*
* INPUTS:
*       devNum       - device number
*       counterIndex - counter index
*       enablePrint  - Enable/Disable output log
*
* OUTPUTS:
*       ipCountersPtr - (pointer to) IP counters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameter
*       GT_FAIL      - on error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCountersIpGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_BOOL                       enablePrint,
    OUT PRV_TGF_IP_COUNTER_SET_STC   *ipCountersPtr
);

/*******************************************************************************
* prvTgfIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port
*
* INPUTS:
*       devNum     - the device number
*       portNum    - the port to enable on
*       ucMcEnable - routing type to enable Unicast/Multicast
*       protocol   - what type of traffic to enable ipv4 or ipv6 or both
*       enable     - enable IP routing for this port
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
GT_STATUS prvTgfIpPortRoutingEnable
(
    IN GT_U8                          portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT  ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT     protocol,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfIpVlanUcRoutingEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 multicast/unicast Routing on Vlan
*
* INPUTS:
*       devNum     - device number
*       vlanId     - Vlan ID
*       ucMcEnable - routing type to enable Unicast/Multicast
*       protocol   - ipv4 or ipv6
*       enable     - enable\disable ip unicast routing
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
GT_STATUS prvTgfIpVlanRoutingEnable
(
    IN GT_U16                         vlanId,
    IN CPSS_IP_UNICAST_MULTICAST_ENT  ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT     protocol,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfInlifEntrySet
*
* DESCRIPTION:
*       Set Inlif Entry
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type
*       inlifIndex    - inlif index
*       inlifEntryPtr - (pointer to) the Inlif Fields
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfInlifEntrySet
(
    IN PRV_TGF_INLIF_TYPE_ENT         inlifType,
    IN GT_U32                         inlifIndex,
    IN PRV_TGF_INLIF_ENTRY_STC       *inlifEntryPtr

);

/*******************************************************************************
* prvTgfIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*       This function adds a virtual router in system for specific LPM DB
*
* INPUTS:
*       lpmDBId                   - LPM DB id
*       vrId                      - virtual's router ID
*       defIpv4UcNextHopInfoPtr   - (pointer to) ipv4 uc next hop info
*       defIpv6UcNextHopInfoPtr   - (pointer to) ipv6 uc next hop info
*       defIpv4McRouteLttEntryPtr - (pointer to) ipv4 mc LTT entry info
*       defIpv6McRouteLttEntryPtr - (pointer to) ipv6 mc LTT entry info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success on success
*       GT_NOT_FOUND      - on the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory.
*       GT_BAD_STATE      - on the existing VR is not empty.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterAdd
(
    IN GT_U32                                lpmDbId,
    IN GT_U32                                vrId,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv4UcNextHopInfoPtr,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv6UcNextHopInfoPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv4McRouteLttEntryPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv6McRouteLttEntryPtr
);

/*******************************************************************************
* prvTgfIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*       This function adds shared virtual router in system for specific LPM DB
*
* INPUTS:
*       lpmDBId                   - LPM DB id
*       vrId                      - virtual's router ID
*       defIpv4UcNextHopInfoPtr   - the route Ipv4 uc entry info
*       defIpv6UcNextHopInfoPtr   - the route Ipv6 uc entry info
*       defIpv4McRouteLttEntryPtr - the ipv4 mc LTT entry
*       defIpv6McRouteLttEntryPtr - the ipv6 mc LTT entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success on success
*       GT_NOT_FOUND      - on the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory.
*       GT_BAD_STATE      - on the existing VR is not empty.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                lpmDbId,
    IN GT_U32                                vrId,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv4UcNextHopInfoPtr,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv6UcNextHopInfoPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv4McRouteLttEntryPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv6McRouteLttEntryPtr
);

/*******************************************************************************
* prvTgfIpLpmVirtualRouterDel
*
* DESCRIPTION:
*       This function removes a virtual router in system for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual's router ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_NOT_FOUND - on the LPM DB id is not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterDel
(
    IN GT_U32                         lpmDbId,
    IN GT_U32                         vrId
);

/*******************************************************************************
* prvTgfIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry
*
* INPUTS:
*       devNum       - device number
*       lttTtiRow    - the entry's row index in LTT table
*       lttTtiColumn - the entry's column index in LTT table
*       lttEntryPtr  - (pointer to) the lookup translation table entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong input parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLttWrite
(
    IN GT_U32                         lttTtiRow,
    IN GT_U32                         lttTtiColumn,
    IN PRV_TGF_IP_LTT_ENTRY_STC      *lttEntryPtr
);

/*******************************************************************************
* prvTgfIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam
*
* INPUTS:
*       devNum              - device number
*       routerTtiTcamRow    - TCAM row to set
*       routerTtiTcamColumn - TCAM column to set
*       prefixPtr           - (pointer to) prefix to set
*       maskPtr             - (pointer to) mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on fail
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv4PrefixSet
(
    IN GT_U32                         routerTtiTcamRow,
    IN GT_U32                         routerTtiTcamColumn,
    IN PRV_TGF_IPV4_PREFIX_STC       *prefixPtr,
    IN PRV_TGF_IPV4_PREFIX_STC       *maskPtr
);

/*******************************************************************************
* prvTgfIpv6PrefixSet
*
* DESCRIPTION:
*    Set an ipv6 UC or MC prefix to the Router Tcam
*
* INPUTS:
*       devNum           - device number
*       routerTtiTcamRow - TCAM row to set
*       prefixPtr        - (pointer to) prefix to set
*       maskPtr          - (pointer to) mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on fail
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv6PrefixSet
(
    IN GT_U32                  routerTtiTcamRow,
    IN PRV_TGF_IPV6_PREFIX_STC *prefixPtr,
    IN PRV_TGF_IPV6_PREFIX_STC *maskPtr
);

/*******************************************************************************
* prvTgfIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam
*
* INPUTS:
*       devNum              - the device number
*       routerTtiTcamRow    - the TCAM row to invalidate
*       routerTtiTcamColumn - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv4PrefixInvalidate
(
    IN GT_U32                         routerTtiTcamRow,
    IN GT_U32                         routerTtiTcamColumn
);

/*******************************************************************************
* prvTgfIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam
*
* INPUTS:
*       devNum           - the device number
*       routerTtiTcamRow - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv6PrefixInvalidate
(
    IN GT_U32 routerTtiTcamRow
);

/*******************************************************************************
* prvTgfIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Write an array of MC route entries to hw
*
* INPUTS:
*       devNum          - device number
*       ucMcBaseIndex   - base Index in the Route entries table
*       numOfEntries    - number of route entries to write
*       entriesArrayPtr - MC route entry array to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpMcRouteEntriesWrite
(
    IN GT_U32                         ucMcBaseIndex,
    IN GT_U32                         numOfEntries,
    IN PRV_TGF_IP_MC_ROUTE_ENTRY_STC *entriesArrayPtr
);

/*******************************************************************************
* prvTgfIpMllPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair
*       mllPairEntryPtr   - (pointer to) the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpMllPairWrite
(
    IN GT_U32                                   mllPairEntryIndex,
    IN PRV_TGF_IP_MLL_PAIR_READ_WRITE_FORM_ENT  mllPairWriteForm,
    IN PRV_TGF_IP_MLL_PAIR_STC                 *mllPairEntryPtr
);

/*******************************************************************************
* prvTgfIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB
*
* INPUTS:
*       lpmDBId   - the LPM DB id
*       devList   - the array of device ids to add to the LPM DB
*       numOfDevs - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBDevListAdd
(
    IN GT_U32                         lpmDBId,
    IN GT_U8                          devList[],
    IN GT_U32                         numOfDevs
);

/*******************************************************************************
* prvTgfIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP MC route for a particular/all source and group address
*
* INPUTS:
*       lpmDBId             - LPM DB id
*       vrId                - virtual private network identifier
*       ipGroup             - IP MC group address
*       ipGroupPrefixLen    - number of bits that are actual valid in ipGroup
*       ipSrc               - root address for source base multi tree protocol
*       ipSrcPrefixLen      - number of bits that are actual valid in ipSrc
*       mcRouteLttEntryPtr  - LTT entry pointing to the MC route entry
*       override            - whether to override an mc Route pointer
*       defragmentationEnable - whether to enable performance costing
*                             de-fragmentation process
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success
*       GT_OUT_OF_RANGE    - if one of prefixes' lengths is too big
*       GT_ERROR           - if the virtual router does not exist
*       GT_OUT_OF_CPU_MEM  - if failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM   - if failed to allocate TCAM memory
*       GT_NOT_IMPLEMENTED - if this request is not implemented
*       GT_FAIL            - otherwise
*       GT_BAD_PTR         - if one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
);

/*******************************************************************************
* prvTgfIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success, or
*       GT_OUT_OF_RANGE - If one of prefixes' lengths is too big, or
*       GT_ERROR        - if the virtual router does not exist, or
*       GT_NOT_FOUND    - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntryDel
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen
);

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* INPUTS:
*       lpmDBId   - The LPM DB id.
*       vrId      - The virtual router id.
*       ipAddr    - The destination IP address to look for.
*       prefixLen - The number of bits that are actual valid in the
*                   ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr     - if found, this is the route entry info.
*                            accosiated with this UC prefix.
*       tcamRowIndexPtr    - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK           - if the required entry was found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                               lpmDBId,
    IN  GT_U32                               vrId,
    IN  GT_IPADDR                            ipAddr,
    IN  GT_U32                               prefixLen,
    OUT PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    OUT GT_U32                               *tcamRowIndexPtr,
    OUT GT_U32                               *tcamColumnIndexPtr
);

/*******************************************************************************
* prvTgfIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr      - the LTT entry pointer pointing to the MC route
*                                 entry associated with this MC route.
*       tcamGroupRowIndexPtr    - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr      - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr   - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK           - if found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
);

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* INPUTS:
*       lpmDBId   - The LPM DB id.
*       vrId      - The virtual router id.
*       ipAddr    - The destination IP address to look for.
*       prefixLen - The number of bits that are actual valid in the
*                   ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr     - If  found, the route entry info accosiated with
*                            this UC prefix.
*       tcamRowIndexPtr    - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK           - if the required entry was found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                               lpmDBId,
    IN  GT_U32                               vrId,
    IN  GT_IPV6ADDR                          ipAddr,
    IN  GT_U32                               prefixLen,
    OUT PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    OUT GT_U32                               *tcamRowIndexPtr,
    OUT GT_U32                               *tcamColumnIndexPtr
);

/*******************************************************************************
* prvTgfIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the entry for.
*       ipGroupPrefixLen - ipGroup prefix length.
*       ipSrc            - The ip Source address to get the entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK           - if found.
*       GT_OUT_OF_RANGE - if one of prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntrySearch
(
    IN  GT_U32                   lpmDBId,
    IN  GT_U32                   vrId,
    IN  GT_IPV6ADDR              ipGroup,
    IN  GT_U32                   ipGroupPrefixLen,
    IN  GT_IPV6ADDR              ipSrc,
    IN  GT_U32                   ipSrcPrefixLen,
    OUT PRV_TGF_IP_LTT_ENTRY_STC *mcRouteLttEntryPtr,
    OUT GT_U32                   *tcamGroupRowIndexPtr,
    OUT GT_U32                   *tcamSrcRowIndexPtr
);

/*******************************************************************************
* prvTgfIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
);

/*******************************************************************************
* prvTgfIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* prvTgfIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* prvTgfIpSetMllCntInterface
*
* DESCRIPTION:
*      Sets a mll counter set's bounded inteface.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2.
*       interfaceCfgPtr - the mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpSetMllCntInterface
(
    IN GT_U32                                    mllCntSet,
    IN PRV_TGF_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceCfgPtr
);

/*******************************************************************************
* prvTgfIpMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum     - the device number.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpMllCntGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mllCntSet,
    OUT GT_U32  *mllOutMCPktsPtr
);

/*******************************************************************************
* prvTgfIpMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpMllCntSet
(
    IN GT_U8    devNum,
    IN GT_U32   mllCntSet,
    IN GT_U32   mllOutMCPkts
);

/*******************************************************************************
* prvTgfIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Sets 40 MSBs of Router MAC SA Base address on specified device.
*
* INPUTS:
*       devNum - the device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
);

/*******************************************************************************
* prvTgfIpRouterMacSaModifyEnable
*
* DESCRIPTION:
*      Per Egress port bit Enable Routed packets MAC SA Modification
*
* INPUTS:
*       devNum   - the device number
*       portNum  - physical or CPU port number.
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterMacSaModifyEnable
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    IN  GT_BOOL                    enable
);

/*******************************************************************************
* prvTgfIpPortRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode, per port, in which the device sets the packet's MAC SA
*       least significant bytes.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       The least significant bytes are set by:
*       Port mode is set by cpssDxChIpRouterPortMacSaLsbSet().
*       Vlan mode is set by cpssDxChIpRouterVlanMacSaLsbSet().
*
*******************************************************************************/
GT_STATUS prvTgfIpPortRouterMacSaLsbModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode
);

/*******************************************************************************
* prvTgfIpRouterPortMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to Eggress Port number.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterPortMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U8   saMac
);

/*******************************************************************************
* prvTgfIpRouterVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlan,
    IN GT_U8   saMac
);

/*******************************************************************************
* prvTgfIpRouterNextHopTableAgeBitsEntryRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       activityBitPtr    - (pointer to) Age bit value of requested Next-hop entry.
*                           field. Range 0..1.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/

GT_STATUS prvTgfIpRouterNextHopTableAgeBitsEntryRead
(
    GT_U32     *activityBitPtr
);

/*******************************************************************************
* prvTgfIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBCapacityGet
(
    IN  GT_U32                                     lpmDBId,
    OUT GT_BOOL                                    *partitionEnablePtr,
    OUT PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    OUT PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
);

/*******************************************************************************
* prvTgfIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBCapacityUpdate
(
    IN GT_U32                                     lpmDBId,
    IN PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfIpGenh */

