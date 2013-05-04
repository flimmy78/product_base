/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfInlifGen.h
*
* DESCRIPTION:
*       Generic API for Inlif
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfInlifGenh
#define __tgfInlifGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssCommonDefs.h>

/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum  PRV_TGF_INLIF_TYPE_ENT
 *
 * Description: Inlif Type enumeration
 *
 * Fields:
 *       PRV_TGF_INLIF_TYPE_PORT_E     - Support Port Inlif fields
 *       PRV_TGF_INLIF_TYPE_VLAN_E     - Support VLAN Inlif fields
 *       PRV_TGF_INLIF_TYPE_EXTERNAL_E - Support TTI Inlif fields
 *
 * Comments:
 */
typedef enum
{
    PRV_TGF_INLIF_TYPE_PORT_E,
    PRV_TGF_INLIF_TYPE_VLAN_E,
    PRV_TGF_INLIF_TYPE_EXTERNAL_E   
} PRV_TGF_INLIF_TYPE_ENT;

/*
 * typedef: struct PRV_TGF_INLIF_ENTRY_STC
 *
 * Description: Struct for InLIF Entry Fields
 *
 * Fields:
 *      bridgeEnable - Enable or disable the L2 bridge
 *      autoLearnEnable - Enable or disable auto-learning
 *      naMessageToCpuEnable - Enable\disable Sending New Source MAC address message to the CPU
 *      naStormPreventionEnable - Enable or disable new source address storm prevention
 *      unkSaUcCommand - the command applied to Unknown MAC source address unicast packets
 *      unkSaUcCommand - the command applied to Unknown MAC destination address unicast packets
 *      unkSaNotSecurBreachEnable - When set, new source address is not considered as a security breach
 *      untaggedMruIndex - Determines the Maximum Received Units (MRU) of packets on the interface
 *      unregNonIpMcCommand - The command of unregistered non IP Multicast packets
 *      unregIpMcCommand - The command of unregistered IPv4 Multicast packets
 *      unregIpv4BcCommand - The command of unregistered IPv4 Broadcast packets
 *      unregNonIpv4BcCommand - The command of unregistered non-IPv4-Broadcast packets
 *      ipv4UcRouteEnable - GT_TRUE = enable IPv4 Unicast Routing on the Interface
 *      ipv4McRouteEnable - GT_TRUE = enable IPv4 Multicast Routing on the interface
 *      ipv6UcRouteEnable - GT_TRUE = enable IPv6 Unicast Routing on the Interface
 *      ipv6McRouteEnable - GT_TRUE = enable IPv6 Multicast Routing on the interface
 *      mplsRouteEnable - GT_TRUE = enable MPLS switching on the interface
 *      vrfId - Determines the Virtual Router ID for packets received on this interface
 *      ipv4IcmpRedirectEnable - redirect command in the router engine
 *      ipv6IcmpRedirectEnable - redirect command in the Router engine
 *      bridgeRouterInterfaceEnable - Bridge routed Interface
 *      ipSecurityProfile - Determines the IP security profile of packets
 *      ipv4IgmpToCpuEnable - Enable\disable trapping/mirroring all IPv4 IGMP packets
 *      ipv6IcmpToCpuEnable - Enable\disable trapping/mirroring all IPv6 ICMP packets
 *      udpBcRelayEnable - GT_TRUE = IPv4 Broadcast UDP packets, are trapped or mirrored
 *                         based on their destination UDP port trapped to the CPU
 *      arpBcToCpuEnable - Enable\disable trapping/mirroring of ARP Broadcast packets
 *      arpBcToMeEnable - GT_TRUE = The router will perform a lookup on the ARP packet
 *      ripv1MirrorEnable - GT_TRUE = RIPv1 packets are mirrored or trapped to the CPU
 *      ipv4LinkLocalMcCommandEnable - GT_TRUE = IPv4 packets with the LL MC DA
 *      ipv6LinkLocalMcCommandEnable - GT_TRUE = IPv6 packets with the LL MC DA
 *      ipv6NeighborSolicitationEnable - GT_TRUE = IPv6 Neighbor Solicitation
 *      mirrorToAnalyzerEnable - GT_TRUE = the packet is mirrored to the Ingress Analyzer port
 *      mirrorToCpuEnable - GT_TRUE = the packet is mirrored to the CPU
 *
 */
typedef struct
{
    GT_BOOL             bridgeEnable; 
    GT_BOOL             autoLearnEnable; 
    GT_BOOL             naMessageToCpuEnable; 
    GT_BOOL             naStormPreventionEnable; 
    CPSS_PACKET_CMD_ENT unkSaUcCommand; 
    CPSS_PACKET_CMD_ENT unkDaUcCommand; 
    GT_BOOL             unkSaNotSecurBreachEnable; 
    GT_U32              untaggedMruIndex; 
    CPSS_PACKET_CMD_ENT unregNonIpMcCommand; 
    CPSS_PACKET_CMD_ENT unregIpMcCommand;   
    CPSS_PACKET_CMD_ENT unregIpv4BcCommand; 
    CPSS_PACKET_CMD_ENT unregNonIpv4BcCommand; 
    GT_BOOL             ipv4UcRouteEnable; 
    GT_BOOL             ipv4McRouteEnable; 
    GT_BOOL             ipv6UcRouteEnable; 
    GT_BOOL             ipv6McRouteEnable; 
    GT_BOOL             mplsRouteEnable; 
    GT_U32              vrfId;
    GT_BOOL             ipv4IcmpRedirectEnable; 
    GT_BOOL             ipv6IcmpRedirectEnable; 
    GT_BOOL             bridgeRouterInterfaceEnable; 
    GT_U32              ipSecurityProfile; 
    GT_BOOL             ipv4IgmpToCpuEnable;
    GT_BOOL             ipv6IcmpToCpuEnable; 
    GT_BOOL             udpBcRelayEnable; 
    GT_BOOL             arpBcToCpuEnable; 
    GT_BOOL             arpBcToMeEnable; 
    GT_BOOL             ripv1MirrorEnable; 
    GT_BOOL             ipv4LinkLocalMcCommandEnable; 
    GT_BOOL             ipv6LinkLocalMcCommandEnable; 
    GT_BOOL             ipv6NeighborSolicitationEnable; 
    GT_BOOL             mirrorToAnalyzerEnable;
    GT_BOOL             mirrorToCpuEnable;
} PRV_TGF_INLIF_ENTRY_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfInlifGenh */


