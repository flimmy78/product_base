/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfPclGen.h
*
* DESCRIPTION:
*       Generic API for PCL
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfPclGenh
#define __tgfPclGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
#endif /* EXMXPM_FAMILY */

#include <common/tgfCommon.h>

/* default PCL id for PCL-ID table configuration */
#define PRV_TGF_PCL_DEFAULT_ID_MAC(_direction, _lookup, _port) \
    ((_direction * 128) + (_lookup * 64) + _port)

/* index for base pcl-ids for virtual routers */
/* since the AppDemo define the default virtual router 0 , we currently must use it */
#define PRV_TGF_PCL_PBR_ID_BASE_CNS   0
/* macro to get pcl-id for relative virtual router */
#define PRV_TGF_PCL_PBR_ID_MAC(index) ((index) + PRV_TGF_PCL_PBR_ID_BASE_CNS)
/* relative index for virtual router to be used in PBR (single VR)*/
#define PRV_TGF_PCL_PBR_VIRTUAL_ROUTER_INDEX_CNS    0
/* pcl-id for the VR for tests */
#define PRV_TGF_PCL_PBR_ID_FOR_VIRTUAL_ROUTER_CNS   (PRV_TGF_PCL_PBR_ID_MAC(PRV_TGF_PCL_PBR_VIRTUAL_ROUTER_INDEX_CNS))


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
 *
 * Description:
 *    Type of access to Ingress / Egress PCL configuration Table
 *
 * Enumerators:
 *   PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - by PORT access mode
 *   PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - by VLAN access mode
 *
 */
typedef enum
{
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E,
    PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E
} PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_LOOKUP_TYPE_ENT
 *
 * Description: Types of lookups 0 and 1
 *
 * Enumerations:
 *    PRV_TGF_PCL_LOOKUP_TYPE_SINGLE_E - single lookup
 *    PRV_TGF_PCL_LOOKUP_TYPE_DOUBLE_E - double lookup
 */
typedef enum
{
    PRV_TGF_PCL_LOOKUP_TYPE_SINGLE_E,
    PRV_TGF_PCL_LOOKUP_TYPE_DOUBLE_E
} PRV_TGF_PCL_LOOKUP_TYPE_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_ACTION_REDIRECT_CMD_ENT
 *
 * Description: enumerator for PCL redirection target
 *
 * Fields:
 *  PRV_TGF_PCL_ACTION_REDIRECT_CMD_NONE_E        - no redirection
 *  PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E      - redirection to output interface
 *  PRV_TGF_PCL_ACTION_REDIRECT_CMD_ROUTER_E      - Ingress control pipe
 *  PRV_TGF_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E - redirect to virtual router
 *
 */
typedef enum
{
    PRV_TGF_PCL_ACTION_REDIRECT_CMD_NONE_E,
    PRV_TGF_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    PRV_TGF_PCL_ACTION_REDIRECT_CMD_ROUTER_E,
    PRV_TGF_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
} PRV_TGF_PCL_ACTION_REDIRECT_CMD_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT
 *
 * Description: enumerator for PCL redirection types of the passenger packet.
 *
 * Fields:
 *  PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E - the passenger packet is Ethernet
 *  PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E       - the passenger packet is IP
 *
 */

typedef enum
{
    PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E,
    PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E
} PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT
 *
 * Description: enumerator Controls the index used for IPCL lookup
 *
 * Fields:
 *      PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E   - retain
 *      PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E - override
 *
 */
typedef enum
{
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E,
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E
} PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_POLICER_ENABLE_ENT
 *
 * Description: enumerator for policer options.
 *
 * Fields:
 *    PRV_TGF_PCL_POLICER_DISABLE_ALL_E              - Meter and Counter are disabled
 *    PRV_TGF_PCL_POLICER_ENABLE_METER_AND_COUNTER_E - Both Meter and Counter enabled
 *    PRV_TGF_PCL_POLICER_ENABLE_METER_ONLY_E        - Meter only enabled
 *    PRV_TGF_PCL_POLICER_ENABLE_COUNTER_ONLY_E      - Counter only enabled
 *
 */

typedef enum
{
    PRV_TGF_PCL_POLICER_DISABLE_ALL_E              = GT_FALSE,
    PRV_TGF_PCL_POLICER_ENABLE_METER_AND_COUNTER_E = GT_TRUE,
    PRV_TGF_PCL_POLICER_ENABLE_METER_ONLY_E,
    PRV_TGF_PCL_POLICER_ENABLE_COUNTER_ONLY_E
} PRV_TGF_PCL_POLICER_ENABLE_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT
 *
 * Description:
 *     This enum describes possible formats of Policy rules
 *
 * Enumerations:
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E    - Standard L2
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E - Standard L2+IPv4/v6 QoS
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E   - Standard IPv4+L4
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E  - Standard IPV6 DIP
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E  - Extended L2+IPv4 + L4
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E   - Extended L2+IPv6
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E   - Extended L4+IPv6
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E     - Standard L2
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E  - Standard L2+IPv4/v6 QoS
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E    - Standard IPv4+L4
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E   - Extended L2+IPv4 + L4
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E    - Extended L2+IPv6
 *     PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E    - Extended L4+IPv6
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E       - Standard UDB styled
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E       - Extended UDB styled
 *     PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E       - External 80-bytes
 *     PRV_TGF_PCL_RULE_FORMAT_LAST_E                  - last element
 *
 */
typedef enum
{
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E,
    PRV_TGF_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_STD_UDB_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E,
    PRV_TGF_PCL_RULE_FORMAT_INGRESS_EXT_80B_E,
    PRV_TGF_PCL_RULE_FORMAT_LAST_E
} PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_PACKET_TYPE_ENT
 *
 * Description: Packet types
 *
 * Enumerations:
 *    PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E       - IPV4 TCP
 *    PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E       - IPV4 UDP
 *    PRV_TGF_PCL_PACKET_TYPE_MPLS_E           - MPLS
 *    PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E  - IPV4 Fragment
 *    PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E     - IPV4 Other
 *    PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E - Ethernet Other
 *    PRV_TGF_PCL_PACKET_TYPE_UDE_E            - User Defined Ethernet Type (UDE 0)
 *    PRV_TGF_PCL_PACKET_TYPE_IPV6_E           - IPV6
 *    PRV_TGF_PCL_PACKET_TYPE_UDE_1_E            - UDE 1
 *    PRV_TGF_PCL_PACKET_TYPE_UDE_2_E            - UDE 2
 *    PRV_TGF_PCL_PACKET_TYPE_UDE_3_E            - UDE 3
 *    PRV_TGF_PCL_PACKET_TYPE_UDE_4_E            - UDE 4
 *
 */
typedef enum
{
    PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E,
    PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E,
    PRV_TGF_PCL_PACKET_TYPE_MPLS_E,
    PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E,
    PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E,
    PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
    PRV_TGF_PCL_PACKET_TYPE_UDE_E,
    PRV_TGF_PCL_PACKET_TYPE_IPV6_E,
    PRV_TGF_PCL_PACKET_TYPE_UDE_1_E,
    PRV_TGF_PCL_PACKET_TYPE_UDE_2_E,
    PRV_TGF_PCL_PACKET_TYPE_UDE_3_E,
    PRV_TGF_PCL_PACKET_TYPE_UDE_4_E,

    PRV_TGF_PCL_PACKET_TYPE_LAST_E/* must be last */

} PRV_TGF_PCL_PACKET_TYPE_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_OFFSET_TYPE_ENT
 *
 * Description: Offset types for packet headers parsing used for user defined
 *              bytes configuration
 *
 * Enumerations:
 *    PRV_TGF_PCL_OFFSET_L2_E                 - offset from start of mac(L2) header
 *    PRV_TGF_PCL_OFFSET_L3_E                 - offset from start of L3 header.
 *    PRV_TGF_PCL_OFFSET_L4_E                 - offset from start of L4 header
 *    PRV_TGF_PCL_OFFSET_IPV6_EXT_HDR_E       - offset from start of IPV6 Extention Header
 *    PRV_TGF_PCL_OFFSET_TCP_UDP_COMPARATOR_E - the user defined byte used
 *                                              for TCP or UDP comparator
 *    PRV_TGF_PCL_OFFSET_L3_MINUS_2_E         - offset from start of L3 header minus 2
 *    PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E       - offset from ethernet type of MPLS minus 2
 *    PRV_TGF_PCL_OFFSET_INVALID_E            - invalid UDB, conains 0 always
 *
 */
typedef enum
{
    PRV_TGF_PCL_OFFSET_L2_E,
    PRV_TGF_PCL_OFFSET_L3_E,
    PRV_TGF_PCL_OFFSET_L4_E,
    PRV_TGF_PCL_OFFSET_IPV6_EXT_HDR_E,
    PRV_TGF_PCL_OFFSET_TCP_UDP_COMPARATOR_E,
    PRV_TGF_PCL_OFFSET_L3_MINUS_2_E,
    PRV_TGF_PCL_OFFSET_MPLS_MINUS_2_E,
    PRV_TGF_PCL_OFFSET_INVALID_E
} PRV_TGF_PCL_OFFSET_TYPE_ENT;

/*
 * Typedef: enum PRV_TGF_UDB_ERROR_CMD_ENT
 *
 * Description:
 *     This enum defines packet command taken in the UDB error case
 *
 * Fields:
 *     PRV_TGF_UDB_ERROR_CMD_LOOKUP_E      - continue Policy Lookup
 *     PRV_TGF_UDB_ERROR_CMD_TRAP_TO_CPU_E - trap packet to CPU
 *     PRV_TGF_UDB_ERROR_CMD_DROP_HARD_E   - hard drop packet
 *     PRV_TGF_UDB_ERROR_CMD_DROP_SOFT_E   - soft drop packet
 *
 */
typedef enum
{
    PRV_TGF_UDB_ERROR_CMD_LOOKUP_E,
    PRV_TGF_UDB_ERROR_CMD_TRAP_TO_CPU_E,
    PRV_TGF_UDB_ERROR_CMD_DROP_HARD_E,
    PRV_TGF_UDB_ERROR_CMD_DROP_SOFT_E
} PRV_TGF_UDB_ERROR_CMD_ENT;

/*
 * Typedef: enum PRV_TGF_PCL_RULE_OPTION_ENT
 *
 * Description:
 *      Enumerator for the TCAM rule write option flags.
 *
 * Fields:
 *      PRV_TGF_PCL_RULE_OPTION_WRITE_INVALID_E   - write all fields
 *                  of rule to TCAM but rule state is invalid
 *                  (no match during lookups).
 *                  The cpssDxChPclRuleValidStatusSet can turn the rule
 *                  to valid state.
 *                  (APPLICABLE DEVICES: DxCh3, xCat, Lion, xCat2, ExMxPm)
 *      PRV_TGF_PCL_RULE_OPTION_PORT_LIST_ENABLED_E   - if set caused
 *                        write rule to TCAM using port-list format.
 *                  (APPLICABLE DEVICES: xCat2)
 *
 *  Comments:
 *
 */
typedef enum
{
    PRV_TGF_PCL_RULE_OPTION_WRITE_INVALID_E      = (0x1 << 0),
    PRV_TGF_PCL_RULE_OPTION_PORT_LIST_ENABLED_E  = (0x1 << 1)
} PRV_TGF_PCL_RULE_OPTION_ENT;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_LOOKUP_CONFIG_STC
 *
 * Description:
 *     This structure defines future lookups configuration
 *
 * Fields:
 *      ipclConfigIndex - index of Pcl Configuration table for next lookup
 *      pcl0_1OverrideConfigIndex - selection index of PCL Cfg Table for IPCL lookup0_1
 *      pcl1OverrideConfigIndex   - selection index of PCL Cfg Table for IPCL lookup1
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                               ipclConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl0_1OverrideConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl1OverrideConfigIndex;
} PRV_TGF_PCL_ACTION_LOOKUP_CONFIG_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_MIRROR_STC
 *
 * Description:
 *     This structure defines the mirroring related Actions.
 *
 * Fields:
 *       mirrorToRxAnalyzerPort         - Enables mirroring to the ingress analyzer port
 *       cpuCode                        - The CPU code assigned to packets
 *       mirrorTcpRstAndFinPacketsToCpu - TCP RST and FIN packets can be mirrored to CPU
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode;
    GT_BOOL                       mirrorToRxAnalyzerPort;
    GT_BOOL                       mirrorTcpRstAndFinPacketsToCpu;
} PRV_TGF_PCL_ACTION_MIRROR_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_MATCH_COUNTER_STC
 *
 * Description:
 *     This structure defines the using of rule match counter.
 *
 * Fields:
 *       enableMatchCount  - enables the binding to the Policy Rule Match Counter
 *       matchCounterIndex - index one of the 32 Policy Rule Match Counter
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL   enableMatchCount;
    GT_U32    matchCounterIndex;
} PRV_TGF_PCL_ACTION_MATCH_COUNTER_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_QOS_STC
 *
 * Description:
 *     This structure defines the packet's QoS attributes mark Actions
 *
 * Fields:
 *
 *    modifyDscp         - modify DSCP QoS attribute of the packet
 *    modifyUp           - modify UP QoS attribute of the packet
 *    modifyTc           - modify TC QoS attribute of the packet
 *    modifyDp           - modify DP QoS attribute of the packet
 *    modifyExp          - modify EXP QoS attribute of the packet
 *    dscp               - DSCP field set to the transmitted packets
 *    up                 - UP field set to the transmitted packets
 *    tc                 - TC field set to the transmitted packets
 *    dp                 - DP field set to the transmitted packets
 *    exp                - EXP field set to the transmitted packets
 *    profileIndex       - The QoS Profile Attribute of the packet
 *    profileAssignIndex - Enable marking of QoS Profile Attribute
 *    profilePrecedence  - Marking of the QoSProfile Precedence
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyTc;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyExp;
    GT_U32                                      tc;
    CPSS_DP_LEVEL_ENT                           dp;
    GT_U32                                      up;
    GT_U32                                      dscp;
    GT_U32                                      exp;
    GT_U32                                      profileIndex;
    GT_BOOL                                     profileAssignIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT profilePrecedence;
} PRV_TGF_PCL_ACTION_QOS_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_REDIRECT_STC
 *
 * Description:
 *     This structure defines the redirection related Actions
 *
 * Fields:
 *       redirectCmd  - redirection command
 *       data.outIf   - out interface redirection data
 *       outInterface - output interface (port, trunk, VID, VIDX)
 *       outlifPointer - pointer to the tunnel, dit or arp information
 *       arpPtr        - pointer to an ARP pointer
 *       ditPtr        - pointer to a DIT entry
 *       tunnelStartPtr - tunnel Start information
 *       tunnelType    - the packet type
 *       ptr           - pointer to a tunnel entry
 *       modifyMacDa  -  When enabled, the packet is marked
 *                       for MAC DA modification,
 *                       and the <ARP Index> specifies the new MAC DA.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (Lion and above devices only)
 *       modifyMacSa  -  When enabled, the packet is marked
 *                       for MAC SA modification - similar to routed packets.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (Lion and above devices only)
 *       vntL2Echo     - enables Virtual Network Tester Layer 2 Echo
 *       tunnelPtr     - the pointer to "Tunnel-start" entry
 *       data.routerLttIndex - index of IP router Lookup Translation Table entry
 *       data.vrfId    - virtual router ID
 *
 *  Comments:
 *
 */
typedef struct
{
    PRV_TGF_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;

    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC     outInterface;
            PRV_TGF_OUTLIF_TYPE_ENT     outlifType;

            union
            {
                GT_U32  arpPtr;
                GT_U32  ditPtr;

                struct
                {
                    PRV_TGF_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT tunnelType;
                    GT_U32                                      ptr;
                } tunnelStartPtr;

            } outlifPointer;

            GT_BOOL                      modifyMacDa;
            GT_BOOL                      modifyMacSa;
            GT_BOOL                      vntL2Echo;
        } outIf;

        GT_U32                   routerLttIndex;
        GT_U32                   vrfId;
    } data;

} PRV_TGF_PCL_ACTION_REDIRECT_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_POLICER_STC
 *
 * Description:
 *     This structure defines the policer related Actions
 *
 * Fields:
 *       policerEnable  - policer enable
 *       policerId      - policers table entry index
 *
 *  Comments:
 *
 */
typedef struct
{
    PRV_TGF_PCL_POLICER_ENABLE_ENT  policerEnable;
    GT_U32                          policerId;

} PRV_TGF_PCL_ACTION_POLICER_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_VLAN_STC
 *
 * Description:
 *     This structure defines the VLAN modification related Actions.
 *
 * Fields:
 *       modifyVlan - VLAN id modification command
 *       nestedVlan - nested VLAN
 *       vlanId     - VLAN id
 *       precedence - the VLAN Assignment precedence
 *       vlanCmd    - VLAN id modification command
 *       vidOrInLif - VLAN id or InLif index
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT          modifyVlan;
    GT_BOOL                                       nestedVlan;
    GT_U16                                        vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   precedence;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT          vlanCmd;
    GT_U32                                        vidOrInLif;

} PRV_TGF_PCL_ACTION_VLAN_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_IP_UC_ROUTE_STC
 *
 * Description:
 *     This structure defines the IP unicast route parameters
 *
 * Fields:
 *       doIpUcRoute       - configure IP Unicast Routing Actions
 *       arpDaIndex        - route Entry ARP Index to the ARP Table (10 bit)
 *       decrementTTL      - decrement IPv4 <TTL> or IPv6 <Hop Limit> enable
 *       bypassTTLCheck    - bypass Router engine TTL and Options Check
 *       icmpRedirectCheck - ICMP Redirect Check Enable
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  doIpUcRoute;
    GT_U32   arpDaIndex;
    GT_BOOL  decrementTTL;
    GT_BOOL  bypassTTLCheck;
    GT_BOOL  icmpRedirectCheck;

} PRV_TGF_PCL_ACTION_IP_UC_ROUTE_STC;

/*
 * Typedef: struct PRV_TGF_PCL_ACTION_SOURCE_ID_STC
 *
 * Description:
 *     This structure defines packet Source Id assignment
 *
 * Fields:
 *     assignSourceId - assign Source Id enable
 *     sourceIdValue  - the Source Id value to be assigned
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} PRV_TGF_PCL_ACTION_SOURCE_ID_STC;

/*
 * typedef: struct PRV_TGF_PCL_ACTION_FABRIC_STC
 *
 * Description: Fabric connectivity data
 *
 * Fields:
 *     flowId -  Flow ID assigned here may be used by a 98FX950 fabric adapter
 *
 */
typedef struct
{
    GT_U32   flowId;
} PRV_TGF_PCL_ACTION_FABRIC_STC;

/*
 * typedef: struct PRV_TGF_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       - packet command (forward, mirror-to-cpu,
 *                     hard-drop, soft-drop, or trap-to-cpu)
 *      actionStop   - Action Stop
 *      bypassBridge - Bridge engine processed or bypassed
 *      bypassIngressPipe - the ingress pipe bypassed or not
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      lookupConfig - configuration of IPCL lookups.
 *      mirror       - packet mirroring configuration
 *      matchCounter - match counter configuration
 *      qos          - packet QoS attributes modification configuration
 *      redirect     - packet Policy redirection configuration
 *      policer      - packet Policing configuration
 *      vlan         - packet VLAN modification configuration
 *      ipUcRoute    - special DxCh (not relevant for DxCh2 and above) Ip Unicast Route
 *                     action parameters configuration
 *      sourceId     - packet source Id assignment
 *      fabric       -  fabric connectivity configuration
 *
 * Comment:
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                  pktCmd;
    GT_BOOL                              actionStop;
    GT_BOOL                              bypassBridge;
    GT_BOOL                              bypassIngressPipe;
    GT_BOOL                              egressPolicy;
    PRV_TGF_PCL_ACTION_LOOKUP_CONFIG_STC lookupConfig;
    PRV_TGF_PCL_ACTION_MIRROR_STC        mirror;
    PRV_TGF_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    PRV_TGF_PCL_ACTION_QOS_STC           qos;
    PRV_TGF_PCL_ACTION_REDIRECT_STC      redirect;
    PRV_TGF_PCL_ACTION_POLICER_STC       policer;
    PRV_TGF_PCL_ACTION_VLAN_STC          vlan;
    PRV_TGF_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
    PRV_TGF_PCL_ACTION_SOURCE_ID_STC     sourceId;
    PRV_TGF_PCL_ACTION_FABRIC_STC        fabric;
} PRV_TGF_PCL_ACTION_STC;

/*
 * typedef: struct PRV_TGF_PCL_LOOKUP_CFG_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup Attributes
 *
 * Fields:
 *    enableLookup           - enable Lookup
 *    externalLookup         - external or internal TCAM
 *    dualLookup             - enable lookup0_1
 *    pclIdL01               - independent PCL ID for lookup0_1
 *    lookupType             - lookup type
 *    pclId                  - PCL-ID bits in the TCAM search key
 *    groupKeyTypes.nonIpKey - type of TCAM search key for NON-IP packets
 *    groupKeyTypes.ipv4Key  - type of TCAM search key for IPV4 packets
 *    groupKeyTypes.ipv6Key  - type of TCAM search key for IPV6 packets
 *
 */
typedef struct
{
    GT_BOOL                                 enableLookup;
    GT_BOOL                                 externalLookup;
    GT_BOOL                                 dualLookup;
    GT_U32                                  pclIdL01;
    PRV_TGF_PCL_LOOKUP_TYPE_ENT             lookupType;
    GT_U32                                  pclId;

    struct
    {
        PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    nonIpKey;
        PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    ipv4Key;
        PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT    ipv6Key;
    } groupKeyTypes;
} PRV_TGF_PCL_LOOKUP_CFG_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all key formats
 *
 * Fields:
 *     pclId         - PCL-ID attribute
 *     macToMe       - MAC To Me
 *     sourcePort    - port number from which the packet ingressed the device
 *     portListBmp   - Port list bitmap.
 *     isTagged      - Flag indicating the packet Tag state
 *     vid           - VLAN ID assigned to the packet
 *     up            - packet's 802.1p User Priority field
 *     qosProfile    - QoS profile assigned to the packet
 *     isIp          - indication that the packet is IP
 *     isIpv4        - indication that the packet is IPv4
 *     isArp         - indication that the packet is ARP
 *     isL2Valid     - indicates that L2 information in the
 *                     search key is valid
 *     isUdbValid    - user-defined bytes
 *
 */
typedef struct
{
    GT_U16     pclId;
    GT_U8      macToMe;
    GT_U8      sourcePort;
    GT_U32     portListBmp;
    GT_U8      isTagged;
    GT_U16     vid;
    GT_U8      up;
    GT_U8      qosProfile;
    GT_U8      isIp;
    GT_U8      isIpv4;
    GT_U8      isArp;
    GT_U8      isL2Valid;
    GT_U8      isUdbValid;
} PRV_TGF_PCL_RULE_FORMAT_COMMON_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended key formats
 *
 * Fields:
 *     isIpv6     - indication that the packet is IPv6
 *     ipProtocol - IP protocol/Next Header type
 *     dscp       - DSCP field of the IPv4/6 header
 *     isL4Valid  - L4 information is valid
 *     isUdbValid - user-defined bytes (UDBs) are valid
 *     l4Byte0    - L4 information is available for UDP and TCP
 *     l4Byte1    - L4 information is available for UDP and TCP
 *     l4Byte2    - L4 information is available for UDP and TCP
 *     l4Byte3    - L4 information is available for UDP and TCP
 *     l4Byte13   - L4 information is available for UDP and TCP
 *     ipHeaderOk - indicates a valid IP header
 *     isTagged   - packet has VLAN tag 0-untagged, 1-tagged
 *     sipBits    - bits 31:0 of IPV6 source IP or all IPV4 source IP address
 *
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      isUdbValid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      ipHeaderOk;
    GT_U8      isTagged;
    GT_U8      sipBits[4];
} PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes common segment of all standard IP key formats
 *
 * Fields:
 *     ipProtocol     - indicates the Layer 4 IP protocol
 *     dscp           - IP packets DSCP
 *     isL4Valid      - 1 - L4Valid, 0 - invalid
 *     l4Byte2        - L4 header byte 2
 *     l4Byte3        - L4 header byte 3
 *     ipHeaderOk     - 0 - invalid, 1 - valid
 *     ipv4Fragmented - indicates IPv4 fragment
 *
 */
typedef struct
{
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      ipHeaderOk;
    GT_U8      ipv4Fragmented;
} PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_IP_STC;

/*
 * typedef: struct PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_UDB_STC
 *
 * Description: PCL Rule Key fields common to ingress "UDB" key formats
 *
 * Fields:
 *     pclId           - PCL-ID attribute
 *     macToMe         - MAC To Me
 *     sourcePort      - port number from which the packet
 *                       ingressed the device
 *     portListBmp     - Port list bitmap.
 *     isTagged        - Flag indicating the packet Tag state
 *     vid             - VLAN ID assigned to the packet
 *     up              - packet's 802.1p User Priority field
 *     isIp            - indication that the packet is IP
 *     pktTagging      - packet's VLAN Tag format
 *     l3OffsetInvalid - indicates that the L3 offset was found
 *     l4ProtocolType  - 0=Other/Ivalid;
 *     pktType         - packet type
 *     ipHeaderOk      - 0 - invalid, 1 - valid
 *     macDaType       - known and Unknown Unicast
 *     l4OffsetInvalid - indicates that the L4 offset was found
 *     l2Encap         - L2 encapsulation of the packet
 *     isIpv6Eh        - indicates that an IPv6 extension exists
 *     isIpv6HopByHop  - indicates that the IPv6 Header is hop-by-hop
 *     isUdbValid      - user-defined bytes
 *     isL2Valid       - L2 fields
 *     dscpOrExp       - IP DSCP or MPLS EXP
 *
 */
typedef struct
{
    GT_U16       pclId;
    GT_U8        macToMe;
    GT_U8        sourcePort;
    GT_U32       portListBmp;
    GT_U16       vid;
    GT_U8        up;
    GT_U8        isIp;
    GT_U8        pktTagging;
    GT_U8        l3OffsetInvalid;
    GT_U8        l4ProtocolType;
    GT_U8        pktType;
    GT_U8        ipHeaderOk;
    GT_U8        macDaType;
    GT_U8        l4OffsetInvalid;
    GT_U8        l2Encap;
    GT_U8        isIpv6Eh;
    GT_U8        isIpv6HopByHop;
    GT_U8        isUdbValid;
    GT_U8        isL2Valid;
    GT_U8        dscpOrExp;
} PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_UDB_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard not-IP key
 * Fields:
 *     common    - common part for all formats
 *     etherType - EtherType
 *     l2Encap   - L2 encapsulation of the packet
 *     macDa     - ethernet Dst MAC address
 *     macSa     - ethernet Src MAC address
 *     udb       - user defined bytes
 *     udb15Dup  - an additional positon of udb15 in the external key
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC      common;
    GT_U16                                  etherType;
    GT_U8                                   l2Encap;
    GT_ETHERADDR                            macDa;
    GT_ETHERADDR                            macSa;
    GT_U8                                   udb[18];
    GT_U8                                   udb15Dup;
} PRV_TGF_PCL_RULE_FORMAT_STD_NOT_IP_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard IPV4/V6 L2_QOS key
 *
 * Fields:
 *     common            - common part for all formats
 *     commonStdIp       - common part for all stanard IP formats
 *     isIpv6ExtHdrExist - IPv6 extension header exists
 *     isIpv6HopByHop    - indicates that the IPv6 Original Extension Header
 *     macDa             - ethernet Dst MAC address
 *     macSa             - ethernet Src MAC address
 *     udb               - user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC          common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_IP_STC   commonStdIp;
    GT_U8                                       isIpv6ExtHdrExist;
    GT_U8                                       isIpv6HopByHop;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
    GT_U8                                       udb[20];
} PRV_TGF_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard IPV4_L4 key
 *
 * Fields:
 *     common      - common part for all formats
 *     commonStdIp - common part for all stanard IP formats
 *     isBc        - ethernet broadcast packet
 *     sip         - IPv4 source IP address field
 *     dip         - IPv4 destination IP address field
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     udb         - user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC          common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_IP_STC   commonStdIp;
    GT_U8                                       isBc;
    GT_IPADDR                                   sip;
    GT_IPADDR                                   dip;
    GT_U8                                       l4Byte0;
    GT_U8                                       l4Byte1;
    GT_U8                                       l4Byte13;
    GT_U8                                       udb[23];
} PRV_TGF_PCL_RULE_FORMAT_STD_IPV4_L4_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_STD_IPV6_DIP_STC
 *
 * Description:
 *     This structure describes the standard IPV6 DIP key
 *
 * Fields:
 *     common            - common part for all formats
 *     commonStdIp       - common part for all standard IP formats
 *     isIpv6ExtHdrExist - IPv6 extension header exists
 *     isIpv6HopByHop    - indicates that the IPv6 Original Extension Header
 *     dip               - 16 bytes IPV6 destination address.
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC          common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_IP_STC   commonStdIp;
    GT_U8                                       isIpv6ExtHdrExist;
    GT_U8                                       isIpv6HopByHop;
    GT_IPV6ADDR                                 dip;
} PRV_TGF_PCL_RULE_FORMAT_STD_IPV6_DIP_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the extended not-IPV6 key
 *
 * Fields:
 *     common         - common part for all formats
 *     commonExt      - common part for all extended formats
 *     sip            - IPv4 source IP address field
 *     dip            - IPv4 header destination IP address field
 *     etherType      - ether type
 *     l2Encap        - indicates L2 encapsulation
 *     macDa          - ethernet Dst MAC address
 *     macSa          - ethernet Src MAC address
 *     ipv4Fragmented - indicates IPv4 fragment
 *     udb            - user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC  commonExt;
    GT_IPADDR                               sip;
    GT_IPADDR                               dip;
    GT_U16                                  etherType;
    GT_U8                                   l2Encap;
    GT_ETHERADDR                            macDa;
    GT_ETHERADDR                            macSa;
    GT_U8                                   ipv4Fragmented;
    GT_U8                                   udb[23];
} PRV_TGF_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L2  key
 *
 * Fields:
 *     common            - common part for all formats
 *     commonExt         - common part for all extended formats
 *     sip               - IPv6 source IP address field
 *     dipBits           - IPV6 destination address highest 8 bits
 *     isIpv6ExtHdrExist - IPv6 extension header exists
 *     isIpv6HopByHop    - indicates that the IPv6 Original Extension Header
 *     macDa             - ethernet Dst MAC address
 *     macSa             - ethernet Src MAC address
 *     udb               - user defined bytes
 *     udb11Dup          -  the duplicated place of UDB11 in the key
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC  commonExt;
    GT_IPV6ADDR                             sip;
    GT_U8                                   dipBits;
    GT_U8                                   isIpv6ExtHdrExist;
    GT_U8                                   isIpv6HopByHop;
    GT_ETHERADDR                            macDa;
    GT_ETHERADDR                            macSa;
    GT_U8                                   udb[23];
    GT_U8                                   udb11Dup;
} PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L2_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L4_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L4  key
 *
 * Fields:
 *     common            - common part for all formats
 *     commonExt         - common part for all extended formats
 *     sip               - IPv6 source IP address field
 *     dip               - IPv6 destination IP address field
 *     isIpv6ExtHdrExist - IPv6 extension header exists
 *     isIpv6HopByHop    - indicates that the IPv6 Original Extension Header
 *     udb               - user defined bytes
 *     udb12Dup         -  duplicated place of UDB12 in the key
 *     udb13Dup         -  duplicated place of UDB13 in the key
 *     udb14Dup         -  duplicated place of UDB13 in the key
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC  commonExt;
    GT_IPV6ADDR                             sip;
    GT_IPV6ADDR                             dip;
    GT_U8                                   isIpv6ExtHdrExist;
    GT_U8                                   isIpv6HopByHop;
    GT_U8                                   udb[23];
    GT_U8                                   udb12Dup;
    GT_U8                                   udb13Dup;
    GT_U8                                   udb14Dup;
} PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L4_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all egress key formats
 *
 * Fields:
 *     pclId       - PCL-ID attribute
 *     isMpls      - indication that the packet is MPLS
 *     isIpv4      - indication that the packet is IPv4
 *     isArp       - indication that the packet is an ARP packet
 *     srcPort     - port number from which the packet ingressed the device
 *     portListBmp - Port list bitmap.
 *     isTagged    - flag indicating the packet Tag state
 *     vid         - VLAN ID assigned to the packet
 *     up          - packet's 802.1p User Priority field
 *     isIp        - indication that the packet is IP
 *     isL2Valid   - indication that L2 information in the search key is valid
 *     egrPacketType - packet type
 *     cpuCode     - CPU code forwarded to the CPU
 *     srcTrg      - packet was sent to the CPU by the ingress\egress pipe
 *     tc          - traffic class
 *     dp          - drop precedence
 *     egrFilterEnable - indication that packet is subject to egress filtering
 *     isUnknown   - indication that the packet’s MAC DA was not found in FDB
 *     rxSniff     - indication if packet is Ingress or Egress mirrored to Analyzer
 *     qosProfile  - QoS profile assigned to the packet
 *     srcTrunkId  - Id of source trunk
 *     srcIsTrunk  - indication that packets received from a Trunk
 *     isRouted    - indication that packet has been routed either by local device
 *     srcDev      - network port at which the packet was received
 *     sourceId    - source ID assigned to the packet
 *     isVidx      - indication that a packet is forwarded to a Multicast group
 *
 */
typedef struct
{
    GT_U16     pclId;
    GT_U8      isMpls;
    GT_U8      isIpv4;
    GT_U8      isArp;
    GT_U8      srcPort;
    GT_U32     portListBmp;
    GT_U8      isTagged;
    GT_U16     vid;
    GT_U8      up;
    GT_U8      isIp;
    GT_U8      isL2Valid;
    GT_U8      egrPacketType;
    union
    {
        struct
        {
            GT_U8 cpuCode;
            GT_U8 srcTrg;
        } toCpu;
        struct
        {
            GT_U8 tc;
            GT_U8 dp;
            GT_U8 egrFilterEnable;
        } fromCpu;
        struct
        {
            GT_U8 rxSniff;
        } toAnalyzer;
        struct
        {
            GT_U8 qosProfile;
            GT_U8 srcTrunkId;
            GT_U8 srcIsTrunk;
            GT_U8 isUnknown;
            GT_U8 isRouted;
        } fwdData;
    } egrPktType;
    GT_U8      srcDev;
    GT_U8      sourceId;
    GT_U8      isVidx;
} PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment of all egress standard IP key formats
 *
 * Fields:
 *     ipProtocol     - indicates the Layer 4 IP protocol / Next Header type
 *     dscp           - IP packets DSCP
 *     isL4Valid      - 1 - L4Valid, 0 - invalid
 *     l4Byte2        - L4 header byte 2
 *     l4Byte3        - L4 header byte 3
 *     l4Byte13       - L4 header byte 13
 *     ipv4Fragmented - indicates IPv4 fragment
 *     egrTcpUdpPortComparator - Tcp Udp Port Comparators
 *     dipBits        - DIP bits 31:0 in network order
 *     tosBits        - TOS bits 1:0
 *
 */
typedef struct
{
    GT_U8      isIpv4;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      ipv4Fragmented;
    GT_U8      egrTcpUdpPortComparator;
    GT_U8      dipBits[4];
    GT_U8      tosBits;
} PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes common segment of extended egress key formats
 *
 * Fields:
 *     isIpv6     - indication that the packet is IPv6
 *     ipProtocol - IP protocol/Next Header type
 *     dscp       - DSCP field of the IPv4/6 header
 *     isL4Valid  - L4 information is valid
 *     l4Byte0    - L4 information is available
 *     l4Byte1    - L4 information is available
 *     l4Byte2    - L4 information is available
 *     l4Byte3    - L4 information is available
 *     l4Byte13   - L4 information is available
 *     egrTcpUdpPortComparator - TCP/UDP comparator result
 *
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U8            isArp;
    GT_U8            isL2Valid;
    GT_U8            packetCmd;
    GT_U8      ipProtocol;
    GT_U8      dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      egrTcpUdpPortComparator;
} PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard egress not-IP key
 *
 * Fields:
 *     common    - common part for all formats
 *     etherType - ether type
 *     l2Encap   - L2 encapsulation of the packet
 *     macDa     - ethernet Dst MAC address
 *     macSa     - ethernet Src MAC address
 *     isBc      - ethernet broadcast packet
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC  common;
    GT_U16                                  etherType;
    GT_U8                                   l2Encap;
    GT_ETHERADDR                            macDa;
    GT_ETHERADDR                            macSa;
    GT_U8                                   isBc;
} PRV_TGF_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4/V6 L2_QOS key
 *
 * Fields:
 *     common      - common part for all formats
 *     commonStdIp - common part for all standard IP formats
 *     macDa       - Ethernet Destination MAC address
 *     sipBits     - IPV4 full source addr or IPV6 src addr bits 31:0
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC          common;
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC   commonStdIp;
    GT_ETHERADDR                                    macDa;
    GT_U8                                           sipBits[4];
} PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4_L4 key
 *
 * Fields:
 *     common      - common part for all formats
 *     commonStdIp - common part for all standard IP formats
 *     isBc        - ethernet broadcast packet
 *     sip         - IPv4 source IP address field
 *     dip         - IPv4 destination IP address field
 *     l4Byte0     - L4 header byte 0
 *     l4Byte1     - L4 header byte 1
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC          common;
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC   commonStdIp;
    GT_U8                                           isBc;
    GT_IPADDR                                       sip;
    GT_IPADDR                                       dip;
    GT_U8                                           l4Byte0;
    GT_U8                                           l4Byte1;
} PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the egress extended not-IPV6 key.
 *
 * Fields:
 *     common          - common part for all formats
 *     commonExt       - common part for all extended formats
 *     sip             - IPv4 source IP address field.
 *     dip             - IPv4 header destination IP address field
 *     etherType       - ether type
 *     l2Encap         - L2 encapsulation of the packet
 *     macDa           - ethernet Dst MAC address
 *     macSa           - ethernet Src MAC address
 *     ipv4Fragmented  - indicates IPv4 fragment
 *     ipv4Options_Ext - indicates that the IPv4 header has an option field
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC  commonExt;
    GT_IPADDR                                   sip;
    GT_IPADDR                                   dip;
    GT_U16                                      etherType;
    GT_U8                                       l2Encap;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
    GT_U8                                       ipv4Fragmented;
    GT_U8                                       ipv4Options;
} PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L2 key
 *
 * Fields:
 *     common    - common part for all formats
 *     commonExt - common part for all extended formats
 *     sip       - IPv6 source IP address field
 *     dipBits   - IPV6 destination address highest 8 bits
 *     isIpv6Eh  - indicates that an IPv6 extension exists
 *     macDa     - ethernet Dst MAC address
 *     macSa     - ethernet Src MAC address
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC  commonExt;
    GT_IPV6ADDR                                 sip;
    GT_U8                                       dipBits;
    GT_U8                                       isIpv6Eh;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
} PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC;

/*
 * Typedef: struct PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L4 key
 *
 * Fields:
 *     common    - common part for all formats
 *     commonExt - common part for all extended formats
 *     sip       - IPv6 source IP address field
 *     dip       - IPv6 destination IP address field
 *     isIpv6Eh  - indicates that an IPv6 extension exists
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_STC      common;
    PRV_TGF_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC  commonExt;
    GT_IPV6ADDR                                 sip;
    GT_IPV6ADDR                                 dip;
    GT_U8                                       isIpv6Eh;
} PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC;

/*
 * typedef: struct PRV_TGF_PCL_RULE_FORMAT_STD_UDB_STC
 *
 * Description: PCL Rule ingress standard "UDB" Key fields
 *
 * Fields:
 *     commonStd -  fields common for all ingress standard keys
 *     commonUdb -  fields common for ingress "UDB" styled keys
 *     udb       -  user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC          commonStd;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_UDB_STC  commonUdb;
    GT_U8                                       udb[23];
} PRV_TGF_PCL_RULE_FORMAT_STD_UDB_STC;

/*
 * typedef: struct PRV_TGF_PCL_RULE_FORMAT_EXT_UDB_STC
 *
 * Description: PCL Rule ingress extended "UDB" Key fields
 *
 * Fields:
 *     commonIngrExt        - fields common for all ingress extended keys
 *     commonIngrUdb        - fields common for ingress "UDB" styled keys
 *     macSaOrSipBits79_32  - ethernet Src MAC address for not IPV6 packet
 *     macDaOrSipBits127_80 - ethernet Dst MAC address for not IPV6 packet
 *     sipBits31_0          - IPV4 full src addr or IPV6 src addr bits 31:0
 *     dipBits31_0          - IPV4 full dst addr or IPV6 dst addr bits 31:0
 *     dipBits127_112       - bits [127:112] of dst IP address of IPV6 packet.
 *     udb                  - user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STC          commonStd;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC      commonExt;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_UDB_STC  commonIngrUdb;
    GT_U8                                       macSaOrSipBits79_32[6];
    GT_U8                                       macDaOrSipBits127_80[6];
    GT_U8                                       sipBits31_0[4];
    GT_U8                                       dipBits31_0[4];
    GT_U8                                       dipBits127_112[2];
    GT_U8                                       udb[23];
} PRV_TGF_PCL_RULE_FORMAT_EXT_UDB_STC;

/*
 * typedef: struct PRV_TGF_PCL_RULE_FORMAT_EXT_80B_STC
 *
 * Description: PCL Rule ingress external-only 80-bytes Key fields
 *
 * Fields:
 *     commonExt     - fields common for all ingress extended keys
 *     commonIngrUdb - fields common for ingress "UDB" styled keys
 *     ethType       - ether type
 *     macDa         - ethernet DstMAC address
 *     macSa         - ethernet Src MAC address
 *     sip           - src IP address of IPV6 packet
 *     dip           - dst IP address of IPV6 packet
 *     l4Byte0       - L4 header byte 0
 *     l4Byte1       - L4 header byte 1
 *     l4Byte2       - L4 header byte 2
 *     l4Byte3       - L4 header byte 3
 *     l4Byte13      - L4 header byte 13
 *     udb           - user defined bytes
 *
 */
typedef struct
{
    PRV_TGF_PCL_RULE_FORMAT_COMMON_EXT_STC      commonExt;
    PRV_TGF_PCL_RULE_FORMAT_COMMON_STD_UDB_STC  commonIngrUdb;
    GT_U16                                      ethType;
    GT_ETHERADDR                                macDa;
    GT_ETHERADDR                                macSa;
    GT_IPV6ADDR                                 sip;
    GT_IPV6ADDR                                 dip;
    GT_U8                                       l4Byte0;
    GT_U8                                       l4Byte1;
    GT_U8                                       l4Byte2;
    GT_U8                                       l4Byte3;
    GT_U8                                       l4Byte13;
    GT_U8                                       udb[21];
} PRV_TGF_PCL_RULE_FORMAT_EXT_80B_STC;

/*
 * Typedef: union PRV_TGF_PCL_RULE_FORMAT_UNT
 *
 * Description:
 *     This union describes the PCL key.
 *
 * Fields:
 *     ruleStdNotIp      - Standard Not IP packet key
 *     ruleStdIpL2Qos    - Standard IPV4 and IPV6 packets L2 and QOS styled key
 *     ruleStdIpv4L4     - Standard IPV4 packet L4 styled key
 *     ruleStdIpv6Dip    - Standard IPV6 packet DIP styled key
 *     ruleExtNotIpv6    - Extended Not IP and IPV4 packet key
 *     ruleExtIpv6L2     - Extended IPV6 packet L2 styled key
 *     ruleExtIpv6L4     - Extended IPV6 packet L2 styled key
 *     ruleEgrStdNotIp   - Egress Standard Not IP packet key
 *     ruleEgrStdIpL2Qos - Egress Standard IPV4 and IPV6 packets L2 and QOS styled key
 *     ruleEgrStdIpv4L4  - Egress Standard IPV4 packet L4 styled key
 *     ruleEgrExtNotIpv6 - Egress SExtended Not IP and IPV4 packet key
 *     ruleEgrExtIpv6L2  - Egress SExtended IPV6 packet L2 styled key
 *     ruleEgrExtIpv6L4  - Egress SExtended IPV6 packet L2 styled key
 *     ruleStdUdb        - Ingress Standard UDB styled key
 *     ruleExtUdb        - Ingress Extended UDB styled key
 *     ruleExtUdb80B     - Ingress External only 80-Byte UDB styled key
 *
 */
typedef union
{
    PRV_TGF_PCL_RULE_FORMAT_STD_NOT_IP_STC          ruleStdNotIp;
    PRV_TGF_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC       ruleStdIpL2Qos;
    PRV_TGF_PCL_RULE_FORMAT_STD_IPV4_L4_STC         ruleStdIpv4L4;
    PRV_TGF_PCL_RULE_FORMAT_STD_IPV6_DIP_STC        ruleStdIpv6Dip;
    PRV_TGF_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC        ruleExtNotIpv6;
    PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L2_STC         ruleExtIpv6L2;
    PRV_TGF_PCL_RULE_FORMAT_EXT_IPV6_L4_STC         ruleExtIpv6L4;
    PRV_TGF_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC      ruleEgrStdNotIp;
    PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC   ruleEgrStdIpL2Qos;
    PRV_TGF_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC     ruleEgrStdIpv4L4;
    PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC    ruleEgrExtNotIpv6;
    PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC     ruleEgrExtIpv6L2;
    PRV_TGF_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC     ruleEgrExtIpv6L4;
    PRV_TGF_PCL_RULE_FORMAT_STD_UDB_STC             ruleStdUdb;
    PRV_TGF_PCL_RULE_FORMAT_EXT_UDB_STC             ruleExtUdb;
    PRV_TGF_PCL_RULE_FORMAT_EXT_80B_STC             ruleExtUdb80B;
} PRV_TGF_PCL_RULE_FORMAT_UNT;

/*
 * typedef: struct PRV_TGF_PCL_OVERRIDE_UDB_STC
 *
 * Description:
 *      Defines content of some User Defined Bytes in the keys
 *
 * Fields:
 *      vrfIdLsbEnableStdNotIp   - Standard Not Ip Key, VRF-ID's LSB in UDB15
 *      vrfIdMsbEnableStdNotIp   - Standard Not Ip Key, VRF-ID's MSB in UDB16
 *      vrfIdLsbEnableStdIpL2Qos - Standard Ip L2 Qos Key, VRF-ID's LSB in UDB18
 *      vrfIdMsbEnableStdIpL2Qos - Standard Ip L2 Qos Key, VRF-ID's MSB in UDB19
 *      vrfIdLsbEnableStdIpv4L4  - Standard Ipv4 L4 Key, VRF-ID's LSB in UDB20
 *      vrfIdMsbEnableStdIpv4L4  - Standard Ipv4 L4 Key, VRF-ID's MSB in UDB21
 *      vrfIdLsbEnableStdUdb     - Standard UDB Key, VRF-ID's LSB in UDB0
 *      vrfIdMsbEnableStdUdb     - Standard UDB Key, VRF-ID's MSB in UDB1
 *      vrfIdLsbEnableExtNotIpv6 - Extended Not Ipv6 Key, VRF-ID's LSB in UDB5
 *      vrfIdMsbEnableExtNotIpv6 - Extended Not Ipv6 Key, VRF-ID's MSB in UDB1
 *      vrfIdLsbEnableExtIpv6L2  - Extended Ipv6 L2 Key, VRF-ID's LSB in UDB11
 *      vrfIdMsbEnableExtIpv6L2  - Extended Ipv6 L2 Key, VRF-ID's MSB in UDB6
 *      vrfIdLsbEnableExtIpv6L4  - Extended Ipv6 L4 Key, VRF-ID's LSB in UDB14
 *      vrfIdMsbEnableExtIpv6L4  - Extended Ipv6 L4 Key, VRF-ID's MSB in UDB12
 *      vrfIdLsbEnableExtUdb     - Extended UDB Key, VRF-ID's LSB in UDB1
 *      vrfIdMsbEnableExtUdb     - Extended UDB Key, VRF-ID's MSB in UDB2
 *      qosProfileEnableStdUdb   - Standard UDB key, QoS profile in UDB2
 *      qosProfileEnableExtUdb   - Extended UDB key, QoS profile in UDB5
 *
 */
typedef struct
{
    GT_BOOL       vrfIdLsbEnableStdNotIp;
    GT_BOOL       vrfIdMsbEnableStdNotIp;
    GT_BOOL       vrfIdLsbEnableStdIpL2Qos;
    GT_BOOL       vrfIdMsbEnableStdIpL2Qos;
    GT_BOOL       vrfIdLsbEnableStdIpv4L4;
    GT_BOOL       vrfIdMsbEnableStdIpv4L4;
    GT_BOOL       vrfIdLsbEnableStdUdb;
    GT_BOOL       vrfIdMsbEnableStdUdb;
    GT_BOOL       vrfIdLsbEnableExtNotIpv6;
    GT_BOOL       vrfIdMsbEnableExtNotIpv6;
    GT_BOOL       vrfIdLsbEnableExtIpv6L2;
    GT_BOOL       vrfIdMsbEnableExtIpv6L2;
    GT_BOOL       vrfIdLsbEnableExtIpv6L4;
    GT_BOOL       vrfIdMsbEnableExtIpv6L4;
    GT_BOOL       vrfIdLsbEnableExtUdb;
    GT_BOOL       vrfIdMsbEnableExtUdb;
    GT_BOOL       qosProfileEnableStdUdb;
    GT_BOOL       qosProfileEnableExtUdb;
} PRV_TGF_PCL_OVERRIDE_UDB_STC;


/*
 * typedef: struct PRV_TGF_PCL_OVERRIDE_UDB_TRUNK_HASH_STC
 *
 * Description:
 *      Defines content of some User Defined Bytes in the key
 *      to be the packets trunk hash value.
 *      Only for Lion and above devices.
 *
 * Fields:
 *      Each value is BOOL,
 *      GT_TRUE causes overriding some User Defined Bytes in some key
 *          by the packets trunk hash value.
 *      GT_FALSE causes the User Defined Bytes contains data from packet
 *          as configured.
 *
 *      trunkHashEnableStdNotIp   - Standard Not Ip Key,
 *                                  trunk hash in UDB17
 *      trunkHashEnableStdIpv4L4  - Standard Ipv4 L4 Key,
 *                                  trunk hash in UDB22
 *      trunkHashEnableStdUdb     - Standard UDB Key,
 *                                  trunk hash in UDB3
 *      trunkHashEnableExtNotIpv6 - Extended Not Ipv6 Key,
 *                                  trunk hash in UDB3
 *      trunkHashEnableExtIpv6L2  - Extended Ipv6 L2 Key,
 *                                  trunk hash in UDB7
 *      trunkHashEnableExtIpv6L4  - Extended Ipv6 L4 Key,
 *                                  trunk hash in UDB13
 *      trunkHashEnableExtUdb     - Extended UDB Key,
 *                                  trunk hash in UDB3
 *
 * Comment:
 */
typedef struct
{
    GT_BOOL       trunkHashEnableStdNotIp;
    GT_BOOL       trunkHashEnableStdIpv4L4;
    GT_BOOL       trunkHashEnableStdUdb;
    GT_BOOL       trunkHashEnableExtNotIpv6;
    GT_BOOL       trunkHashEnableExtIpv6L2;
    GT_BOOL       trunkHashEnableExtIpv6L4;
    GT_BOOL       trunkHashEnableExtUdb;
} PRV_TGF_PCL_OVERRIDE_UDB_TRUNK_HASH_STC;

/*
 * Typedef: enum PRV_TGF_PCL_EGRESS_KEY_VID_UP_MODE_ENT
 *
 * Description: enumerator for Egress PCL
 *              VID and UP key fields content mode
 *
 * Fields:
 *  PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E   - extract from packet Tag0
 *  PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E   - extract from packet Tag1
 */
typedef enum
{
    PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E,
    PRV_TGF_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E
} PRV_TGF_PCL_EGRESS_KEY_VID_UP_MODE_ENT;

/*
 * typedef: enum PRV_TGF_PCL_EGRESS_PKT_TYPE_ENT
 *
 * Description: packet types that can be enabled/disabled for Egress PCL.
 *              By default on all ports and on all packet types are disabled
 *
 * Enumerators:
 *   PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E - Enable/Disable Egress Policy
 *                   for Control packets FROM CPU.
 *   PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_DATA_E    - Enable/Disable Egress Policy
 *                   for data packets FROM CPU.
 *   PRV_TGF_PCL_EGRESS_PKT_TO_CPU_E           - Enable/Disable Egress Policy
 *                   on TO-CPU packets
 *   PRV_TGF_PCL_EGRESS_PKT_TO_ANALYZER_E      - Enable/Disable Egress Policy
 *                   for TO ANALYZER packets.
 *   PRV_TGF_PCL_EGRESS_PKT_TS_E               - Enable/Disable Egress Policy
 *                   for data packets that are tunneled in this device.
 *   PRV_TGF_PCL_EGRESS_PKT_NON_TS_E           - Enable/Disable Egress Policy
 *                   for data packets that are not tunneled in this device.
 *
 * Comment:
 *    relevant only for DxCh2 and above devices
 */
typedef enum
{
    PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,
    PRV_TGF_PCL_EGRESS_PKT_FROM_CPU_DATA_E,
    PRV_TGF_PCL_EGRESS_PKT_TO_CPU_E,
    PRV_TGF_PCL_EGRESS_PKT_TO_ANALYZER_E,
    PRV_TGF_PCL_EGRESS_PKT_TS_E,
    PRV_TGF_PCL_EGRESS_PKT_NON_TS_E
} PRV_TGF_PCL_EGRESS_PKT_TYPE_ENT;
/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChRuleAction
*
* DESCRIPTION:
*       Convert generic into device specific Policy Engine Action
*
* INPUTS:
*       ruleActionPtr - (pointer to) Policy Engine Action
*
* OUTPUTS:
*       dxChRuleActionPtr - (pointer to) DxCh Policy Engine Action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChRuleAction
(
    IN  PRV_TGF_PCL_ACTION_STC      *ruleActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC    *dxChRuleActionPtr
);
#endif /* CHX_FAMILY */

/*******************************************************************************
* prvTgfPclInit
*
* DESCRIPTION:
*   The function initializes the Policy engine
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS prvTgfPclInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy
*
* INPUTS:
*    devNum - device number
*    enable - enable ingress policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclIngressPolicyEnable
(
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy
*
* INPUTS:
*    devNum - device number
*    enable - enable ingress policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressPolicyEnable
(
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* INPUTS:
*    devNum  - device number
*    portNum - port number
*    enable  - enable/disable Ingress Policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclPortIngressPolicyEnable
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfPclEgressPclPacketTypesSet
*
* DESCRIPTION:
*    Enables/disables egress policy per packet type and port.
*
* INPUTS:
*    devNum    - device number
*    portNum   - port number
*    pktType   - packet type
*    enable    - enable/disable Ingress Policy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             portNum,
    IN PRV_TGF_PCL_EGRESS_PKT_TYPE_ENT   pktType,
    IN GT_BOOL                           enable
);

/*******************************************************************************
* prvTgfPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table perlookup.
*
* INPUTS:
*    devNum    - device number
*    portNum   - port number
*    direction - policy direction
*    lookupNum - lookup number
*    sublookup - sublookup 0 or 1 (relevant for DxCh Xcat only)
*    mode      - PCL Configuration Table access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                        portNum,
    IN CPSS_PCL_DIRECTION_ENT                       direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                   lookupNum,
    IN GT_U32                                       sublookup,
    IN PRV_TGF_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
);

/*******************************************************************************
* prvTgfPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* INPUTS:
*       devNum           - device number
*       interfaceInfoPtr - (pointer to) interface data
*       direction        - Policy direction
*       lookupNum        - Lookup number
*       lookupCfgPtr     - (pointer to) lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - one of the input parameters is not valid
*       GT_TIMEOUT   - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclCfgTblSet
(
    IN CPSS_INTERFACE_INFO_STC       *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT     lookupNum,
    IN PRV_TGF_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* prvTgfPclRuleWithOptionsSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule
*       ruleIndex        - index of the rule in the TCAM
*       ruleOptionsBmp   - Bitmap of rule's options.
*       maskPtr          - (pointer to) rule mask
*       patternPtr       - (pointer to) rule pattern
*       actionPtr        - (pointer to) policy rule action
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleWithOptionsSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN GT_U32                             ruleIndex,
    IN PRV_TGF_PCL_RULE_OPTION_ENT        ruleOptionsBmp,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *maskPtr,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *patternPtr,
    IN PRV_TGF_PCL_ACTION_STC            *actionPtr
);

/*******************************************************************************
* prvTgfPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* INPUTS:
*       devNum     - device number
*       ruleFormat - format of the Rule
*       ruleIndex  - index of the rule in the TCAM
*       maskPtr    - (pointer to) rule mask
*       patternPtr - (pointer to) rule pattern
*       actionPtr  - (pointer to) policy rule action
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN GT_U32                             ruleIndex,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *maskPtr,
    IN PRV_TGF_PCL_RULE_FORMAT_UNT       *patternPtr,
    IN PRV_TGF_PCL_ACTION_STC            *actionPtr
);

/*******************************************************************************
* prvTgfPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* INPUTS:
*       devNum    - device number
*       ruleSize  - size of Rule
*       ruleIndex - index of the rule in the TCAM
*       actionPtr - policy rule action that applied on packet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*       GT_OUT_OF_RANGE - on the parameters is out of range
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleActionUpdate
(
    IN CPSS_PCL_RULE_SIZE_ENT         ruleSize,
    IN GT_U32                         ruleIndex,
    IN PRV_TGF_PCL_ACTION_STC        *actionPtr
);

/*******************************************************************************
* prvTgfPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule
*
* INPUTS:
*       devNum    - device number
*       ruleSize  - size of Rule
*       ruleIndex - index of the rule in the TCAM.
*       valid     - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - on TCAM found rule of different size
*       GT_TIMEOUT   - on max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclRuleValidStatusSet
(
    IN CPSS_PCL_RULE_SIZE_ENT         ruleSize,
    IN GT_U32                         ruleIndex,
    IN GT_BOOL                        valid
);

/*******************************************************************************
* prvTgfPclUdbIndexConvert
*
* DESCRIPTION:
*   The function converts the UDB relative index (in rule format)
*   to UDB absolute index in UDB configuration table entry
*   Conversion needed for XCAT and Puma devices, not needed for CH1-3
*
* INPUTS:
*       ruleFormat - rule format
*       packetType - packet Type
*       udbIndex   - relative index of UDB in the rule
*
* OUTPUTS:
*       udbAbsIndexPtr - (pointer to) UDB absolute index in
*                        UDB configuration table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - on parameter value more then HW bit field
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUdbIndexConvert
(
    IN  PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT     ruleFormat,
    IN  GT_U32                               udbRelIndex,
    OUT GT_U32                               *udbAbsIndexPtr
);

/*******************************************************************************
* prvTgfPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* INPUTS:
*       ruleFormat - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType - packet Type
*                      Relevant for DxChXCat and above devices
*       udbIndex   - index of User Defined Byte to configure
*       offsetType - the type of offset
*       offset     - The offset of the user-defined byte
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameter
*       GT_OUT_OF_RANGE - on parameter value more then HW bit field
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUserDefinedByteSet
(
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT     ruleFormat,
    IN PRV_TGF_PCL_PACKET_TYPE_ENT          packetType,
    IN GT_U32                               udbIndex,
    IN PRV_TGF_PCL_OFFSET_TYPE_ENT          offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* prvTgfPclOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes by predefined key fields
*
* INPUTS:
*       devNum         - device number
*       udbOverridePtr - (pointer to) UDB override structure
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameter
*       GT_BAD_PTR   - on null pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclOverrideUserDefinedBytesSet
(
    IN  PRV_TGF_PCL_OVERRIDE_UDB_STC *udbOverridePtr
);

/*******************************************************************************
* prvTgfPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header depth
*
* INPUTS:
*    udbErrorCmd - command applied to a packet
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on one of the input parameters is not valid
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclInvalidUdbCmdSet
(
    IN PRV_TGF_UDB_ERROR_CMD_ENT    udbErrorCmd
);


/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfPclDefPortInitExt1
*
* DESCRIPTION:
*     Initialize PCL Engine
*
* INPUTS:
*       portNum   - port number
*       direction - PCL_DIRECTION
*       lookupNum - PCL_LOOKUP_NUMBER
*       pclId     - pcl Id
*       nonIpKey  - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv4Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv6Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_FAIL      - on general failure error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclDefPortInitExt1
(
    IN GT_U8                            portNum,
    IN CPSS_PCL_DIRECTION_ENT           direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT       lookupNum,
    IN GT_U32                           pclId,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT nonIpKey,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv4Key,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv6Key
);

/*******************************************************************************
* prvTgfPclDefPortInit
*
* DESCRIPTION:
*     Initialize PCL Engine
*
* INPUTS:
*       portNum   - port number
*       direction - PCL_DIRECTION
*       lookupNum - PCL_LOOKUP_NUMBER
*       nonIpKey  - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv4Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*       ipv6Key   - PCL_RULE_FORMAT_TYPE for PCL_LOOKUP_CFG_TABLE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*       GT_FAIL      - on general failure error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclDefPortInit
(
    IN GT_U8                            portNum,
    IN CPSS_PCL_DIRECTION_ENT           direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT       lookupNum,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT nonIpKey,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv4Key,
    IN PRV_TGF_PCL_RULE_FORMAT_TYPE_ENT ipv6Key
);

/*******************************************************************************
* prvTgfPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       index         - UDE Ethertype index, range 0..5
*       ethType       - Ethertype value (range 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclUdeEtherTypeSet
(
    IN  GT_U32          index,
    IN  GT_U32          ethType
);

/*******************************************************************************
* prvTgfPclOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* INPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  PRV_TGF_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
);

/*******************************************************************************
* prvTgfPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* INPUTS:
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclEgressKeyFieldsVidUpModeSet
(
    IN  PRV_TGF_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
);

/*******************************************************************************
* prvTgfPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* INPUTS:
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfPclLookupCfgPortListEnableSet
(
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* prvTgfPclRestore
*
* DESCRIPTION:
*       Function clears pcl settings.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfPclRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPclPolicerCheck
*
* DESCRIPTION:
*    check if the device supports the policer pointed from the PCL/TTI
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the policer pointed from the PCL/TTI
*       GT_FALSE - the device not supports the policer pointed from the PCL/TTI
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfPclPolicerCheck(
    void
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfPclGenh */

