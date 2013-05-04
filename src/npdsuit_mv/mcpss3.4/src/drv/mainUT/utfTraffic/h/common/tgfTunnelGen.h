/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTunnelGen.h
*
* DESCRIPTION:
*       Generic API for Tunnel
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfTunnelGenh
#define __tgfTunnelGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfPclGen.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/tti/cpssDxChTtiTypes.h>
    #include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
    #include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/tti/cpssExMxPmTtiTypes.h>
    #include <cpss/exMxPm/exMxPmGen/tti/cpssExMxPmTti.h>
    #include <cpss/exMxPm/exMxPmGen/tunnelStart/cpssExMxPmTunnelStart.h>
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_TTI_REDIRECT_COMMAND_ENT
 *
 * Description:
 *      TTI action redirect command
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_NO_REDIRECT_E               - do not redirect this packet
 *      PRV_TGF_TTI_REDIRECT_TO_EGRESS_E        - policy switching
 *      PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E - policy routing
 *      PRV_TGF_TTI_VRF_ID_ASSIGN_E             - vrf id assignment
 *      PRV_TGF_TTI_REDIRECT_TO_OUTLIF_E        - redirect to a configured OutLIF
 *      PRV_TGF_TTI_REDIRECT_TO_NEXT_HOP_E      - redirect to an IP Next Hop Entry
 *      PRV_TGF_TTI_TRIGGER_VPLS_LOOKUP_E       - trigger VPLS Lookup
 */
typedef enum
{
    PRV_TGF_TTI_NO_REDIRECT_E,
    PRV_TGF_TTI_REDIRECT_TO_EGRESS_E,
    PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E,
    PRV_TGF_TTI_VRF_ID_ASSIGN_E,
    PRV_TGF_TTI_REDIRECT_TO_OUTLIF_E,
    PRV_TGF_TTI_REDIRECT_TO_NEXT_HOP_E,
    PRV_TGF_TTI_TRIGGER_VPLS_LOOKUP_E
} PRV_TGF_TTI_REDIRECT_COMMAND_ENT;

/*
 * typedef: enum PRV_TGF_TTI_PASSENGER_TYPE_ENT
 *
 * Description:
 *      TTI tunnel passenger protocol types
 *
 * Enumerations:
 *      PRV_TGF_TTI_PASSENGER_IPV4_E            - IPv4 passenger type
 *      PRV_TGF_TTI_PASSENGER_IPV6_E            - IPv6 passenger type
 *      PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E    - ethernet with crc passenger type
 *      PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E - ethernet with no crc passenger type
 *      PRV_TGF_TTI_PASSENGER_IPV4V6_E          - IPv4/IPv6 passenger type
 *      PRV_TGF_TTI_PASSENGER_MPLS_E            - MPLS passenger type
 *
 */
typedef enum
{
    PRV_TGF_TTI_PASSENGER_IPV4_E,
    PRV_TGF_TTI_PASSENGER_IPV6_E,
    PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E,
    PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E,
    PRV_TGF_TTI_PASSENGER_IPV4V6_E,
    PRV_TGF_TTI_PASSENGER_MPLS_E
} PRV_TGF_TTI_PASSENGER_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_PASSANGER_TYPE_ENT
 *
 * Description:
 *      Tunnel passenger protocol types
 *
 * Enumerations:
 *      PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E - ethernet passenger type
 *      PRV_TGF_TUNNEL_PASSENGER_OTHER_E    - other passenger type
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E,
    PRV_TGF_TUNNEL_PASSENGER_OTHER_E
} PRV_TGF_TUNNEL_PASSANGER_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TTI_VLAN_COMMAND_ENT
 *
 * Description:
 *      TTI action VLAN command.
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E   - do not modify vlan
 *      PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E - modify vlan only for untagged packets
 *      PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E   - modify vlan only for tagged packets
 *      PRV_TGF_TTI_VLAN_MODIFY_ALL_E      - modify vlan to all packets
 *
 */
typedef enum
{
    PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E,
    PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E,
    PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E,
    PRV_TGF_TTI_VLAN_MODIFY_ALL_E
} PRV_TGF_TTI_VLAN_COMMAND_ENT;

/*
 * typedef: enum PRV_TGF_TTI_KEY_TYPE_ENT
 *
 * Description:
 *      TTI key type.
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_KEY_IPV4_E - IPv4 TTI key type
 *      PRV_TGF_TTI_KEY_MPLS_E - MPLS TTI key type
 *      PRV_TGF_TTI_KEY_ETH_E  - Ethernet TTI key type
 *      PRV_TGF_TTI_KEY_MIM_E  - Mac in Mac TTI key type
 *
 */
typedef enum
{
    PRV_TGF_TTI_KEY_IPV4_E,
    PRV_TGF_TTI_KEY_MPLS_E,
    PRV_TGF_TTI_KEY_ETH_E,
    PRV_TGF_TTI_KEY_MIM_E
} PRV_TGF_TTI_KEY_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TTI_MAC_MODE_ENT
 *
 * Description:
 *      Determines MAC that will be used to generate lookup TCAM key
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_MAC_MODE_DA_E - use destination MAC
 *      PRV_TGF_TTI_MAC_MODE_SA_E - use source MAC
 *
 */
typedef enum
{
    PRV_TGF_TTI_MAC_MODE_DA_E,
    PRV_TGF_TTI_MAC_MODE_SA_E
} PRV_TGF_TTI_MAC_MODE_ENT;

/*
 * typedef: enum PRV_TGF_TTI_QOS_MODE_TYPE_ENT
 *
 * Description: QoS trust mode type
 *
 * Enumerations:
 *  PRV_TGF_TTI_QOS_KEEP_PREVIOUS_E    - keep prior QoS  profile assignment
 *  PRV_TGF_TTI_QOS_TRUST_PASS_L2_E    - Trust passenger packet L2 QoS
 *  PRV_TGF_TTI_QOS_TRUST_PASS_L3_E    - Trust passenger packet L3 QoS
 *  PRV_TGF_TTI_QOS_TRUST_PASS_L2_L3_E - Trust passenger packet L2 and L3 QoS
 *  PRV_TGF_TTI_QOS_UNTRUST_E          - Untrust packet QoS
 *  PRV_TGF_TTI_QOS_TRUST_MPLS_EXP_E   - Trust outer MPLS label EXP
 *
 */
typedef enum
{
    PRV_TGF_TTI_QOS_KEEP_PREVIOUS_E    = 0,
    PRV_TGF_TTI_QOS_TRUST_PASS_L2_E    = 1,
    PRV_TGF_TTI_QOS_TRUST_PASS_L3_E    = 2,
    PRV_TGF_TTI_QOS_TRUST_PASS_L2_L3_E = 3,
    PRV_TGF_TTI_QOS_UNTRUST_E          = 4,
    PRV_TGF_TTI_QOS_TRUST_MPLS_EXP_E   = 5
} PRV_TGF_TTI_QOS_MODE_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TTI_MODIFY_UP_ENT
 *
 * Description:
 *      TTI modify UP enable
 *
 * Enumerations:
 *      PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E - do not modify the previous UP enable setting
 *      PRV_TGF_TTI_MODIFY_UP_ENABLE_E      - set modify UP enable flag to 1
 *      PRV_TGF_TTI_MODIFY_UP_DISABLE_E     - set modify UP enable flag to 0
 *      PRV_TGF_TTI_MODIFY_UP_RESERVED_E    - reserved
 *
 */
typedef enum
{
    PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E = 0,
    PRV_TGF_TTI_MODIFY_UP_ENABLE_E      = 1,
    PRV_TGF_TTI_MODIFY_UP_DISABLE_E     = 2,
    PRV_TGF_TTI_MODIFY_UP_RESERVED_E    = 3
} PRV_TGF_TTI_MODIFY_UP_ENT;

/*
 * typedef: enum PRV_TGF_TTI_TAG1_UP_COMMAND_ENT
 *
 * Description:
 *      TTI tag1 UP command
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E
 *                - If packet does not contain Tag1 assign according
 *                  to action entry's <UP1>, else use Tag1<UP>
 *      PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E
 *                - If packet contains Tag0 use Tag0<UP0>, else
 *                  use action entry's <UP1> field
 *      PRV_TGF_TTI_TAG1_UP_ASSIGN_ALL_E
 *                - Assign action entry's <UP1> field to all packets
 *
 */
typedef enum
{
    PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E = 0,
    PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E = 1,
    PRV_TGF_TTI_TAG1_UP_ASSIGN_ALL_E            = 2
} PRV_TGF_TTI_TAG1_UP_COMMAND_ENT;

/*
 * typedef: enum PRV_TGF_TTI_MODIFY_DSCP_ENT
 *
 * Description:
 *      TTI modify DSCP enable
 *
 * Enumerations:
 *      PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E - do not modify the previous DSCP enable setting
 *      PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E      - set modify DSCP enable flag to 1
 *      PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E     - set modify DSCP enable flag to 0
 *      PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E    - reserved
 *
 */
typedef enum
{
    PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E = 0,
    PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E      = 1,
    PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E     = 2,
    PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E    = 3
} PRV_TGF_TTI_MODIFY_DSCP_ENT;

/*
 * typedef: enum PRV_TGF_TTI_MPLS_COMMAND_ENT
 *
 * Description:
 *      TTI action MPLS command.
 *
 * Enumerations:
 *      PRV_TGF_TTI_MPLS_NOP_COMMAND_E          - self explainatory
 *      PRV_TGF_TTI_MPLS_SWAP_COMMAND_E         - self explainatory
 *      PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E  - self explainatory
 *      PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E   - self explainatory
 *      PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E  - self explainatory
 *      PRV_TGF_TTI_MPLS_POP3_LABELS_COMMAND_E  - self explainatory
 *      PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E - self explainatory
 *
 */
typedef enum
{
    PRV_TGF_TTI_MPLS_NOP_COMMAND_E          = 0,
    PRV_TGF_TTI_MPLS_SWAP_COMMAND_E         = 1,
    PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E  = 2,
    PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E   = 3,
    PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E  = 4,
    PRV_TGF_TTI_MPLS_POP3_LABELS_COMMAND_E  = 5,
    PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E = 6
} PRV_TGF_TTI_MPLS_COMMAND_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_START_MARK_MODE_ENT
 *
 * Description: Enumeration of tunnel start entry mark modes
 *
 * Enumerations:
 *  PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E - based on Tunnel Start entry
 *  PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E - based on t QoS Profile
 *  PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E - based on ingress pipe
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E              = 0,
    PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E = 1,
    PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E       = 2
} PRV_TGF_TUNNEL_START_MARK_MODE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_ENT
 *
 * Description: Enumeration of tunnel start entry mpls ethertypes
 *
 * Enumerations:
 *  PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_UC_E - MPLS ethertype Unicast
 *  PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_MC_E - MPLS ethertype Multicast
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_UC_E = 0,
    PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_MC_E = 1
} PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TTI_ACTION_TYPE_ENT
 *
 * Description:
 *      TTI action type enum
 *
 * Enumerations:
 *
 *      PRV_TGF_TTI_ACTION_TYPE1_ENT - type1 action
 *      PRV_TGF_TTI_ACTION_TYPE2_ENT - type2 action
 *
 */
typedef enum
{
    PRV_TGF_TTI_ACTION_TYPE1_ENT = 0,
    PRV_TGF_TTI_ACTION_TYPE2_ENT = 1
} PRV_TGF_TTI_ACTION_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_QOS_MODE_ENT
 *
 * Description: Enumeration of QoS modes
 *
 * Enumerations:
 *
 *  PRV_TGF_TUNNEL_QOS_KEEP_PREVIOUS_E      - keep QoS profile assignment from
 *                                            previous mechanisms
 *  PRV_TGF_TUNNEL_QOS_TRUST_PASS_UP_E      - trust passenger protocol 802.1p user
 *                                            priority
 *  PRV_TGF_TUNNEL_QOS_TRUST_PASS_DSCP_E    - trust passenger protocol IP DSCP
 *  PRV_TGF_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E - trust passenger protocol 802.1p user
 *                                            priority and IP DSCP
 *  PRV_TGF_TUNNEL_QOS_TRUST_MPLS_EXP_E     - trust outer label MPLS EXP
 *  PRV_TGF_TUNNEL_QOS_UNTRUST_PKT_E        - untrust packet QoS
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_QOS_KEEP_PREVIOUS_E      = 0,
    PRV_TGF_TUNNEL_QOS_TRUST_PASS_UP_E      = 1,
    PRV_TGF_TUNNEL_QOS_TRUST_PASS_DSCP_E    = 2,
    PRV_TGF_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E = 3,
    PRV_TGF_TUNNEL_QOS_TRUST_MPLS_EXP_E     = 4,
    PRV_TGF_TUNNEL_QOS_UNTRUST_PKT_E        = 5
} PRV_TGF_TUNNEL_QOS_MODE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_START_TTL_MODE_ENT
 *
 * Description: Enumeration of tunnel start MPLS LSR TTL operation.
 *
 * Enumerations:
 *
 *  PRV_TGF_TUNNEL_START_TTL_TO_INCOMING_TTL_E - set TTL to incoming TTL Assignment
 *  PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_E - set TTL to the TTL of the swapped label
 *  PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E - set TTL to the TTL of the swapped label - 1
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_START_TTL_TO_INCOMING_TTL_E         = 0,
    PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_E           = 1,
    PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E = 2
} PRV_TGF_TUNNEL_START_TTL_MODE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT
 *
 * Description:
 *      GRE ether type.
 *
 * Enumerations:
 *      PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E  - GRE type 0 - control ether type.
 *      PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E  - GRE type 1 - data ether type.
 *
 */
typedef enum
{
    PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E = 0,
    PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E = 1

} PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT;

/*
 * Typedef: struct PRV_TGF_TTI_RULE_COMMON_STC
 *
 * Description:
 *          TTI Common fields in TCAM rule
 *
 * Fields:
 *
 *  pclId           - PCL id
 *  srcIsTrunk      - whether source is port or trunk
 *  srcPortTrunk    - Source port or Source trunk-ID
 *  mac             - MAC DA or MAC SA of the tunnel header
 *  vid             - packet VID assignment
 *  isTagged        - whether the external mac is considered tagged on not
 *  dsaSrcIsTrunk   - whether sourse is Trunk or not
 *  dsaSrcPortTrunk - DSA tag source port or trunk
 *  dsaSrcDevice    - DSA tag source device
 *  sourcePortGroupId - Indicates the port group where the packet entered the device.
 *                      Defined only for MPLS lookup.
 *                     (Lion and above for multi port groups devices only)
 *
 */
typedef struct
{
    GT_U32                  pclId;
    GT_BOOL                 srcIsTrunk;
    GT_U32                  srcPortTrunk;
    GT_ETHERADDR            mac;
    GT_U16                  vid;
    GT_BOOL                 isTagged;
    GT_BOOL                 dsaSrcIsTrunk;
    GT_U8                   dsaSrcPortTrunk;
    GT_U8                   dsaSrcDevice;
    GT_U32                  sourcePortGroupId;

} PRV_TGF_TTI_RULE_COMMON_STC;

/*
 * Typedef: struct PRV_TGF_TTI_IPV4_RULE_STC
 *
 * Description:
 *          TTI IPv4 TCAM rule
 *
 * Fields:
 *
 *  tunneltype - tunneling protocol of the packet
 *  srcIp      - tunnel source IP
 *  destIp     - tunnel destination IP
 *  isArp      - whether the packet is ARP or not ARP
 *
 */
typedef struct
{
    PRV_TGF_TTI_RULE_COMMON_STC common;
    GT_U32                      tunneltype;
    GT_IPADDR                   srcIp;
    GT_IPADDR                   destIp;
    GT_BOOL                     isArp;

} PRV_TGF_TTI_IPV4_RULE_STC;

/*
 * Typedef: struct PRV_TGF_TTI_MPLS_RULE_STC
 *
 * Description:
 *          TTI MPLS TCAM rule
 *
 * Fields:
 *
 *  label0            - MPLS label 0
 *  exp0              - EXP of MPLS label 0
 *  label1            - MPLS label 1
 *  exp1              - EXP of MPLS label 1
 *  label2            - MPLS label 2
 *  exp2              - EXP of MPLS label 2
 *  numOfLabels       - The number of MPLS labels in the label stack
 *  protocolAboveMPLS - The protocol above MPLS
 *
 */
typedef struct
{
    PRV_TGF_TTI_RULE_COMMON_STC common;
    GT_U32                      label0;
    GT_U32                      exp0;
    GT_U32                      label1;
    GT_U32                      exp1;
    GT_U32                      label2;
    GT_U32                      exp2;
    GT_U32                      numOfLabels;
    GT_U32                      protocolAboveMPLS;

} PRV_TGF_TTI_MPLS_RULE_STC;

/*
 * Typedef: struct PRV_TGF_TTI_ETH_RULE_STC
 *
 * Description:
 *          TTI Ethernet TCAM rule
 *
 * Fields:
 *
 *  up0           - user priority as found in the outer VLAN tag header
 *  cfi0          - CFI (Canonical Format Indicator)
 *  isVlan1Exists - whether the fields: VLAN-ID1, UP1, and CFI1
 *  vid1          - VLAN ID of vlan1
 *  up1           - user priority of vlan1
 *  cfi1          - CFI (Canonical Format Indicator) of vlan1
 *  etherType     - ether type
 *  macToMe       - if there was a match in MAC2ME table
 *
 */
typedef struct
{
    PRV_TGF_TTI_RULE_COMMON_STC common;
    GT_U32                      up0;
    GT_U32                      cfi0;
    GT_BOOL                     isVlan1Exists;
    GT_U16                      vid1;
    GT_U32                      up1;
    GT_U32                      cfi1;
    GT_U32                      etherType;
    GT_BOOL                     macToMe;

} PRV_TGF_TTI_ETH_RULE_STC;

/*
 * Typedef: struct PRV_TGF_TTI_MIM_RULE_STC
 *
 * Description:
 *          TTI MIM TCAM rule
 *
 * Fields:
 *
 *  bUp   - backbone user priority assigned from vlan header
 *  bDp   - backbone drop precedence assigned from vlan header
 *  iSid  - inner service instance assigned from tag information
 *  iUp   - inner user priority assigned from tag information
 *  iDp   - inner drop precedence assigned from tag information
 *  iRes1 - 2 reserved bits
 *  iRes2 - 2 reserved bits
 */
typedef struct
{
    PRV_TGF_TTI_RULE_COMMON_STC common;
    GT_U32                      bUp;
    GT_U32                      bDp;
    GT_U32                      iSid;
    GT_U32                      iUp;
    GT_U32                      iDp;
    GT_U32                      iRes1;
    GT_U32                      iRes2;

} PRV_TGF_TTI_MIM_RULE_STC;

/*
 * typedef: union PRV_TGF_TTI_RULE_UNT
 *
 * Description:
 *      Union for TTI rule
 *
 * Fields:
 *
 *  ipv4 - ipv4 rule
 *  mpls - MPLS rule
 *  eth  - ethernet rule
 *  mim  - Mac in Mac rule
 *
 */
typedef union
{
    PRV_TGF_TTI_IPV4_RULE_STC   ipv4;
    PRV_TGF_TTI_MPLS_RULE_STC   mpls;
    PRV_TGF_TTI_ETH_RULE_STC    eth;
    PRV_TGF_TTI_MIM_RULE_STC    mim;

} PRV_TGF_TTI_RULE_UNT;

/*
 * Typedef: struct PRV_TGF_TTI_ACTION_STC
 *
 * Description:
 *          TTI TCAM rule action parameters
 *
 * Fields:
 *      tunnelTerminate         - whether the packet's tunnel header is removed;
 *      passengerPacketType     - passenger packet type
 *      copyTtlFromTunnelHeader - whether to copy TTL from tunnel header
 *      command                 - forwarding command
 *      redirectCommand         - where to redirect the packet
 *      egressInterface         - egress interface to redirect packet
 *      tunnelStart             - whether <redirectCommand> is REDIRECT_TO_EGRESS
 *      tunnelStartPtr          - pointer to the Tunnel Start entry
 *      routerLookupPtr         - pointer to the Router Lookup Translation Table entry
 *      vrfId                   - VRF-ID assigned to the packet
 *      VirtualSourceInterface  - virtual source interface
 *      sourceIdSetEnable       - if set, then the <sourceId> is assign to the packet
 *      sourceId                - source ID
 *      vlanCmd                 - vlan command applied to packets matching the TTI
 *      vlanId                  - TTI VLAN-ID assignment
 *      vlanPrecedence          - whether the VID assignment
 *      nestedVlanEnable        - enable/disable nested vlan
 *      bindToPolicer           - If set, the packet is bound to <policerIndex>
 *      policerIndex            - Traffic profile to be used if the <bindToPolicer> is set
 *      qosPrecedence           - whether QoS profile can be overridden by subsequent pipeline
 *      qosTrustMode            - QoS trust mode type
 *      qosProfile              - QoS profile to assign to the packet
 *      modifyUpEnable          - modify UP enable
 *      modifyDscpEnable        - modify DSCP enable
 *      up                      - default UP assignment
 *      remapDSCP               - Whether to remap the DSCP
 *      userDefinedCpuCode      - user defined cpu code
 *      vntl2Echo               - if set, the packet MAC SA/DA are switched on the transmitting
 *      bridgeBypass            - if set, the packet isn't subject to any bridge mechanisms
 *      actionStop              - if set, the packet will not have any searches in the IPCL mechanism
 *      activateCounter         - enables binding of a CNC counter with matching of this TTI action
 *      counterIndex            - the CNC match counter index.
 *
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                         command;
    PRV_TGF_TTI_REDIRECT_COMMAND_ENT            redirectCommand;
    CPSS_NET_RX_CPU_CODE_ENT                    userDefinedCpuCode;
    GT_BOOL                                     tunnelTerminate;
    PRV_TGF_TTI_PASSENGER_TYPE_ENT              passengerPacketType;
    GT_BOOL                                     copyTtlFromTunnelHeader;
    GT_BOOL                                     mirrorToIngressAnalyzerEnable;
    GT_U32                                      policerIndex;
    CPSS_INTERFACE_INFO_STC                     interfaceInfo;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT vlanPrecedence;
    PRV_TGF_TTI_VLAN_COMMAND_ENT                vlanCmd;
    GT_U16                                      vlanId;
    GT_BOOL                                     nestedVlanEnable;

    GT_BOOL                                     tunnelStart;
    GT_U32                                      tunnelStartPtr;
    GT_U32                                      routerLookupPtr;
    GT_U32                                      vrfId;
    GT_BOOL                                     useVidx;
    GT_BOOL                                     sourceIdSetEnable;
    GT_U32                                      sourceId;
    GT_BOOL                                     bindToPolicer;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    PRV_TGF_TTI_QOS_MODE_TYPE_ENT               qosTrustMode;
    GT_U32                                      qosProfile;
    PRV_TGF_TTI_MODIFY_UP_ENT                   modifyUpEnable;
    PRV_TGF_TTI_MODIFY_DSCP_ENT                 modifyDscpEnable;
    GT_U32                                      up;
    GT_BOOL                                     remapDSCP;
    GT_BOOL                                     vntl2Echo;
    GT_BOOL                                     bridgeBypass;
    GT_BOOL                                     actionStop;
    GT_BOOL                                     activateCounter;
    GT_U32                                      counterIndex;

    GT_U32                                      arpPtr;
    GT_U32                                      ipNextHopIndex;
    PRV_TGF_TTI_MPLS_COMMAND_ENT                mplsCmd;
    GT_U32                                      exp;
    GT_BOOL                                     setExp;
    GT_BOOL                                     enableDecTtl;
    GT_U32                                      mplsLabel;
    GT_U32                                      ttl;
    GT_U32                                      pwId;
    GT_BOOL                                     sourceIsPE;
    GT_BOOL                                     enableSourceLocalFiltering;
    GT_U32                                      floodDitPointer;
    GT_BOOL                                     counterEnable;
    GT_BOOL                                     meterEnable;
    GT_U32                                      flowId;
    GT_BOOL                                     sstIdEnable;
    GT_U32                                      sstId;

    PRV_TGF_TUNNEL_PASSANGER_TYPE_ENT           tsPassengerPacketType;
    PRV_TGF_TTI_VLAN_COMMAND_ENT                tag0VlanCmd;
    GT_U16                                      tag0VlanId;
    PRV_TGF_TTI_VLAN_COMMAND_ENT                tag1VlanCmd;
    GT_U16                                      tag1VlanId;
    GT_BOOL                                     keepPreviousQoS;
    GT_BOOL                                     trustUp;
    GT_BOOL                                     trustDscp;
    GT_BOOL                                     trustExp;
    PRV_TGF_TTI_TAG1_UP_COMMAND_ENT             tag1UpCommand;
    GT_U32                                      tag0Up;
    GT_U32                                      tag1Up;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT         pcl0OverrideConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT         pcl0_1OverrideConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT         pcl1OverrideConfigIndex;
    GT_U32                                      iPclConfigIndex;
    GT_BOOL                                     bindToCentralCounter;
    GT_U32                                      centralCounterIndex;
    GT_BOOL                                     ingressPipeBypass;

} PRV_TGF_TTI_ACTION_STC;

/*
 * Typedef: struct PRV_TGF_TTI_ACTION_2_STC
 *
 * Description:
 *          TTI TCAM rule action parameters (standard action)
 *
 * Fields:
 *      tunnelTerminate       - whether the packet's tunnel header is removed
 *      ttPassengerPacketType - passenger packet type
 *      tsPassengerPacketType - type of passenger packet
 *      copyTtlFromTunnelHeader - the TTL is copied from the last popped label
 *      mplsCommand           - MPLS Action applied to the packet
 *      mplsTtl               - TTL is taken from the tunnel or inner MPLS label
 *      enableDecrementTtl    - enable/disable TTL decremented
 *      command               - forwarding command
 *      redirectCommand       - where to redirect the packet
 *      egressInterface       - egress interface to redirect packet
 *      arpPtr                - pointer to the ARP entry for MPLS routed packets
 *      tunnelStart           - redirected to Tunnel/non-Tunnel Egress Interface
 *      tunnelStartPtr        - pointer to the Tunnel Start entry
 *      routerLttPtr          - pointer to the Router Lookup Translation Table entry
 *      vrfId                 - VRF-ID assigned to the packet
 *      sourceIdSetEnable     - if set, then the <sourceId> is assign to the packet
 *      sourceId              - source ID assigned to the packet
 *      tag0VlanCmd           - tag0 vlan command
 *      tag0VlanId            - tag0 VLAN-ID assignment
 *      tag1VlanCmd           - tag1 vlan command
 *      tag1VlanId            - tag1 VLAN-ID assignment
 *      tag0VlanPrecedence    - whether the VID assignment can be overridden by
 *                              subsequent VLAN assignment mechanism
 *      nestedVlanEnable      - enable/disable nested vlan
 *      bindToPolicerMeter    - If set, the packet is bound to the policer meter
 *      bindToPolicer         - If set, the packet is bound to the policer/billing/IPFIX
 *      policerIndex          - Traffic profile to be used if the <bindToPolicer> is set
 *      qosPrecedence         - whether QoS profile can be overridden by subsequent pipeline
 *      keepPreviousQoS       - assign QoS profile based on QoS fields
 *      trustUp               - do not trust packet’s L2 QoS
 *      trustDscp             - do not trust packet’s L3 QoS
 *      trustExp              - do not trust packet’s EXP
 *      qosProfile            - QoS profile to assign to the packet
 *      modifyTag0Up          - modify tag0 UP
 *      tag1UpCommand         - tag1 UP command
 *      modifyDscp            - modify DSCP mode
 *      tag0Up                - tag0 UP assignment
 *      tag1Up                - tag1 UP assignment
 *      remapDSCP             - packet’s DSCP is not remapped
 *      pcl0OverrideConfigIndex  - controls the index used for IPCL0 lookup
 *      pcl0O1verrideConfigIndex - controls the index used for IPCL0-1 lookup
 *      pcl1OverrideConfigIndex  - controls the index used for IPCL1 lookup
 *      iPclConfigIndex          - pointer to IPCL configuration entry to be used
 *      mirrorToIngressAnalyzerEnable - packet is mirrored to the Ingress Analyzer port
 *      userDefinedCpuCode    - user defined cpu code
 *      bindToCentralCounter  - enable/disable central counter binding
 *      centralCounterIndex   - central counter bound to this entry
 *      vntl2Echo             - if set, the packet MAC SA/DA are switched on the transmitting
 *      bridgeBypass          - if set, the packet isn't subject to any bridge mechanisms
 *      ingressPipeBypass     - if set, the ingress engines are all bypassed for this packet
 *      actionStop            - if set, the packet will not have any searches in the IPCL mechanism
 *      hashMaskIndex         - Determines which mask is used in the CRC based hash.
 *                              0: do not override hash mask index.
 *                              Other values: override the hash mask index value.
 *                             (Lion and above devices only)
 *      modifyMacSa           - If set, the packet is marked for MAC SA modification
 *                              Relevant only when <redirectCommand> == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E
 *                             (Lion and above devices only)
 *      modifyMacDa           - If set, the packet is marked for MAC DA modification,
 *                              and the <ARP Index> specifies the new MAC DA.
 *                              Relevant only when <redirectCommand> == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E
 *                             (Lion and above devices only)
 *      ResetSrcPortGroupId   - If set, the source port group ID of the packet is set to the current port group.
 *                              Relevant only when <redirectCommand> != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E
 *                             (Lion and above for multi port groups devices only)
 *      multiPortGroupTtiEnable    - If set, the packet is forwarded to a ring port for another TTI lookup.
 *                                   Relevant only when <redirectCommand> == CPSS_DXCH_TTI_NO_REDIRECT_E
 *                                  (Lion and above for multi port groups devices only)
 *
 */
typedef struct
{
    GT_BOOL                                 tunnelTerminate;
    PRV_TGF_TTI_PASSENGER_TYPE_ENT          ttPassengerPacketType;
    PRV_TGF_TUNNEL_PASSANGER_TYPE_ENT       tsPassengerPacketType;
    GT_BOOL                                 copyTtlFromTunnelHeader;
    PRV_TGF_TTI_MPLS_COMMAND_ENT            mplsCommand;
    GT_U32                                  mplsTtl;
    GT_BOOL                                 enableDecrementTtl;
    CPSS_PACKET_CMD_ENT                     command;
    PRV_TGF_TTI_REDIRECT_COMMAND_ENT        redirectCommand;
    CPSS_INTERFACE_INFO_STC                 egressInterface;
    GT_U32                                  arpPtr;
    GT_BOOL                                 tunnelStart;
    GT_U32                                  tunnelStartPtr;
    GT_U32                                  routerLttPtr;
    GT_U32                                  vrfId;
    GT_BOOL                                 sourceIdSetEnable;
    GT_U32                                  sourceId;
    PRV_TGF_TTI_VLAN_COMMAND_ENT            tag0VlanCmd;
    GT_U16                                  tag0VlanId;
    PRV_TGF_TTI_VLAN_COMMAND_ENT            tag1VlanCmd;
    GT_U16                                  tag1VlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT tag0VlanPrecedence;
    GT_BOOL                                 nestedVlanEnable;
    GT_BOOL                                 bindToPolicerMeter;
    GT_BOOL                                 bindToPolicer;
    GT_U32                                  policerIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    GT_BOOL                                 keepPreviousQoS;
    GT_BOOL                                 trustUp;
    GT_BOOL                                 trustDscp;
    GT_BOOL                                 trustExp;
    GT_U32                                  qosProfile;
    PRV_TGF_TTI_MODIFY_UP_ENT               modifyTag0Up;
    PRV_TGF_TTI_TAG1_UP_COMMAND_ENT         tag1UpCommand;
    PRV_TGF_TTI_MODIFY_DSCP_ENT             modifyDscp;
    GT_U32                                  tag0Up;
    GT_U32                                  tag1Up;
    GT_BOOL                                 remapDSCP;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT     pcl0OverrideConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT     pcl0_1OverrideConfigIndex;
    PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_ENT     pcl1OverrideConfigIndex;
    GT_U32                                  iPclConfigIndex;
    GT_BOOL                                 mirrorToIngressAnalyzerEnable;
    CPSS_NET_RX_CPU_CODE_ENT                userDefinedCpuCode;
    GT_BOOL                                 bindToCentralCounter;
    GT_U32                                  centralCounterIndex;
    GT_BOOL                                 vntl2Echo;
    GT_BOOL                                 bridgeBypass;
    GT_BOOL                                 ingressPipeBypass;
    GT_BOOL                                 actionStop;
    GT_U32                                  hashMaskIndex;
    GT_BOOL                                 modifyMacSa;
    GT_BOOL                                 modifyMacDa;
    GT_BOOL                                 ResetSrcPortGroupId;
    GT_BOOL                                 multiPortGroupTtiEnable;

} PRV_TGF_TTI_ACTION_2_STC;

/*
 * typedef: struct PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC
 *
 * Description: Struct of configuration for X-over-MPLS tunnel start
 *
 * Fields:
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - VID in the Tunnel VLAN tag
 *  upMarkMode        - user priority mode
 *  up                - user priority
 *  macDa             - Tunnel next hop MAC DA
 *  numLabels         - number of MPLS labels
 *  ttl               - tunnel IPv4 header TTL
 *  ttlMode           - MPLS LSR TTL operations
 *  label1            - MPLS label 1
 *  exp1MarkMode      - EXP1 mode
 *  exp1              - The EXP1 value
 *  label2            - MPLS label 2
 *  exp2MarkMode      - EXP2 mode
 *  exp2              - The EXP2 value
 *  exp2MarkMode      - EXP3 mode
 *  exp2              - The EXP3 value
 *  retainCRC         - retain the passenger packet CRC
 *  setSBit           - S-bit
 *  cfi               - CFI bit assigned for TS packets
 */
typedef struct
{
    GT_BOOL                                     tagEnable;
    GT_U16                                      vlanId;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT          upMarkMode;
    GT_U32                                      up;
    GT_ETHERADDR                                macDa;
    GT_U32                                      numLabels;
    GT_U32                                      ttl;
    PRV_TGF_TUNNEL_START_TTL_MODE_ENT           ttlMode;
    PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_ENT    ethType;
    GT_U32                                      label1;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT          exp1MarkMode;
    GT_U32                                      exp1;
    GT_U32                                      label2;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT          exp2MarkMode;
    GT_U32                                      exp2;
    GT_U32                                      label3;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT          exp3MarkMode;
    GT_U32                                      exp3;
    GT_BOOL                                     retainCRC;
    GT_BOOL                                     setSBit;
    GT_U32                                      cfi;
} PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC;

/*
 * typedef: struct PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC
 *
 * Description: Struct of configuration for X-over-IPv4 tunnel start
 *
 * Fields:
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - VID in the Tunnel VLAN tag
 *  upMarkMode        - user priority
 *  up                - The tag 802.1p user priority
 *  dscpMarkMode      - DSCP mode
 *  dscp              - The DSCP in the IPv4 tunnel header
 *  macDa             - Tunnel next hop MAC DA
 *  dontFragmentFlag  - Don't fragment flag in the tunnel IPv4 header
 *  ttl               - tunnel IPv4 header TTL
 *  autoTunnel        - Relevant for IPv6-over-IPv4 and IPv6-over-GRE-IPv4 only,
 *                      if GT_TRUE, the IPv4 header DIP is derived from IPv6
 *                      passenger packet and destIp field is ignored
 *  autoTunnelOffset  - Relevant for IPv6-over-IPv4 or IPv6-over-GRE-IPv4 only (0..15)
 *                      If <autoTunnel> is GT_TRUE, this field is the offset
 *                      of IPv4 destination address inside IPv6 destination
 *                      address
 *  ethType           - Ethertype to put in GRE protocol
 *  destIp            - Tunnel destination IP
 *  srcIp             - Tunnel source IP
 *  cfi               - CFI bit assigned for TS packets
 *  retainCrc         - retain passenger packet CRC
 *
 */
typedef struct
{
    GT_BOOL                             tagEnable;
    GT_U16                              vlanId;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  upMarkMode;
    GT_U32                              up;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  dscpMarkMode;
    GT_U32                              dscp;
    GT_ETHERADDR                        macDa;
    GT_BOOL                             dontFragmentFlag;
    GT_U32                              ttl;
    GT_BOOL                             autoTunnel;
    GT_U32                              autoTunnelOffset;
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT      ethType;
    GT_IPADDR                           destIp;
    GT_IPADDR                           srcIp;
    GT_U32                              cfi;
    GT_BOOL                             retainCrc;
} PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC;

/*
 * typedef: struct PRV_TGF_TUNNEL_START_MIM_ENTRY_STC
 *
 * Description: Struct of configuration for MacInMac tunnel start entry
 *
 * Fields:
 *  tagEnable      - the packet is sent with VLAN tag
 *  vlanId         - VID in the Tunnel VLAN tag
 *  upMarkMode     - user priority mode
 *  up             - user priority
 *  macDa          - Tunnel header MAC DA
 *  ttl            - tunnel IPv4 header TTL
 *  retainCrc      - retain passenger packet CRC
 *  iSid           - MacInMac inner service identifier
 *  iSidAssignMode - iSid Assignment mode
 *  iUp            - inner user priority field
 *  iUpMarkMode    - Marking mode to determine assignment of iUp
 *  iDp            - inner drop precedence in the iTag
 *  iDpMarkMode    - DP mode
 *  iTagReserved   - The reserved bits to be set in the iTag
 *
 */
typedef struct
{
    GT_BOOL                             tagEnable;
    GT_U16                              vlanId;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  upMarkMode;
    GT_U32                              up;
    GT_U32                              ttl;
    GT_ETHERADDR                        macDa;
    GT_BOOL                             retainCrc;
    GT_U32                              iSid;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  iSidAssignMode;
    GT_U32                              iUp;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  iUpMarkMode;
    GT_U32                              iDp;
    PRV_TGF_TUNNEL_START_MARK_MODE_ENT  iDpMarkMode;
    GT_U32                              iTagReserved;

} PRV_TGF_TUNNEL_START_MIM_ENTRY_STC;

/*
 * typedef: union PRV_TGF_TUNNEL_START_ENTRY_UNT
 *
 * Description: Union for configuration for tunnel start
 *
 * Fields:
 *  ipv4Cfg - configuration for X-over-IPv4 tunnel start entry
 *  mplsCfg - configuration for X-over-MPLS tunnel start entry
 *  mimCfg  - configuration for MacInMac tunnel start entry
 *
 */
typedef union
{
    PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC  ipv4Cfg;
    PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC  mplsCfg;
    PRV_TGF_TUNNEL_START_MIM_ENTRY_STC   mimCfg;
} PRV_TGF_TUNNEL_START_ENTRY_UNT;


/*
 * Typedef: struct PRV_TGF_TTI_MAC_VLAN_STC
 *
 * Description:
 *          Mac To Me and Vlan To Me.
 *
 * Fields:
 *
 *  mac            - Mac to me address
 *  vlanId         - vlan-to-me ID
 *
 */
typedef struct
{
    GT_ETHERADDR        mac;
    GT_U16              vlanId;

} PRV_TGF_TTI_MAC_VLAN_STC;
/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTtiRuleActionSet
*
* DESCRIPTION:
*       This function sets rule action
*
* INPUTS:
*       devNum    - device number
*       ruleIndex - index of the rule in the TCAM
*       actionPtr - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleActionSet
(
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
);

/*******************************************************************************
* prvTgfTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum     - device number
*       keyType    - TTI key type
*       ruleIndex  - index of the tunnel termination entry
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleSet
(
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_RULE_UNT         *patternPtr,
    IN  PRV_TGF_TTI_RULE_UNT         *maskPtr,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
);

/*******************************************************************************
* prvTgfTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum    - device number
*       keyType   - TTI key type
*       ruleIndex - index of the tunnel termination entry
*
* OUTPUTS:
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_RULE_UNT         *patternPtr,
    OUT PRV_TGF_TTI_RULE_UNT         *maskPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
);

/*******************************************************************************
* prvTgfTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleValidStatusSet
(
    IN  GT_U32                        routerTtiTcamRow,
    IN  GT_BOOL                       valid
);

/*******************************************************************************
* prvTgfTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry
*
* INPUTS:
*       devNum     - device number
*       index      - line index for the tunnel start entry
*       tunnelType - type of the tunnel
*       configPtr  - (pointer to) tunnel start configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - on failure
*       GT_OUT_OF_RANGE - parameter not in valid range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartEntrySet
(
    IN  GT_U32                          index,
    IN  CPSS_TUNNEL_TYPE_ENT            tunnelType,
    IN  PRV_TGF_TUNNEL_START_ENTRY_UNT *configPtr
);

/*******************************************************************************
* prvTgfTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry
*
* INPUTS:
*       devNum - device number
*       index  - line index for the tunnel start entry
*
* OUTPUTS:
*       tunnelTypePtr - (pointer to) the type of the tunnel
*       configPtr     - (pointer to) tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range.
*       GT_BAD_STATE    - on invalid tunnel type
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TUNNEL_TYPE_ENT           *tunnelTypePtr,
    OUT PRV_TGF_TUNNEL_START_ENTRY_UNT *configPtr
);

/*******************************************************************************
* prvTgfTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       keyType - TTI key type
*       enable  - enable\disable TTI lookup
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortLookupEnableSet
(
    IN  GT_U8                         portNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* prvTgfTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       keyType - TTI key type
*       macMode - MAC mode to use
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacModeSet
(
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_MAC_MODE_ENT      macMode
);

/*******************************************************************************
* prvTgfTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       keyType - TTI key type
*
* OUTPUTS:
*       macModePtr - (poiner to) MAC mode to use
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacModeGet
(
    IN  GT_U8                         devNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_MAC_MODE_ENT     *macModePtr
);

/*******************************************************************************
* prvTgfTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - enable\disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                         portNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* prvTgfTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type
*
* INPUTS:
*       devNum  - device number
*       ethType - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMimEthTypeSet
(
    IN  GT_U32                        ethType
);

/*******************************************************************************
* prvTgfTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
*
* INPUTS:
*       entryIndex        - Index of mac and vlan in MacToMe table
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacToMeSet
(
    IN  GT_U32                            entryIndex,
    IN  PRV_TGF_TTI_MAC_VLAN_STC         *valuePtr,
    IN  PRV_TGF_TTI_MAC_VLAN_STC         *maskPtr
);

/*******************************************************************************
* prvTgfTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum     - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType    - TTI key type
*       ruleIndex  - index of the tunnel termination entry
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleSet
(
    IN  GT_U32                        ruleIndex,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_RULE_UNT         *patternPtr,
    IN  PRV_TGF_TTI_RULE_UNT         *maskPtr,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
);

/*******************************************************************************
* prvTgfTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum    - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType   - TTI key type
*       ruleIndex - index of the tunnel termination entry
*
* OUTPUTS:
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_RULE_UNT         *patternPtr,
    OUT PRV_TGF_TTI_RULE_UNT         *maskPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
);

/*******************************************************************************
* prvTgfTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType - TTI key type
*       macMode - MAC mode to use
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupMacModeSet
(
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_MAC_MODE_ENT      macMode
);

/*******************************************************************************
* prvTgfTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType - TTI key type
*
* OUTPUTS:
*       macModePtr - (pointer to) MAC mode to use
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupMacModeGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_MAC_MODE_ENT     *macModePtr
);

/*******************************************************************************
* prvTgfTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleValidStatusSet
(
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        routerTtiTcamRow,
    IN  GT_BOOL                       valid
);

/*******************************************************************************
* prvTgfBrgTtiCheck
*
* DESCRIPTION:
*    check if the device supports the tti
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the tti
*       GT_FALSE - the device not supports the tti
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfBrgTtiCheck(
    void
);

/*******************************************************************************
* prvTgfTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT   greTunnelType,
    IN  GT_U32                              ethType
);

/*******************************************************************************
* prvTgfTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*
* OUTPUTS:
*       ethTypePtr - (pointer to) Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT   greTunnelType,
    OUT GT_U32                              *ethTypePtr
);

/*******************************************************************************
* prvTgfTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       enable - GT_TRUE: Add offset to tunnel total length
*                GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvTgfTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* INPUTS:
*       devNum           - device number
*       additionToLength - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong value in any of the parameters
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 additionToLength
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfTunnelGenh */

