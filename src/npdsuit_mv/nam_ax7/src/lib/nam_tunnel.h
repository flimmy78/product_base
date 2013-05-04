#ifndef __NAM_TUNNEL_H__
#define __NAM_TUNNEL_H__

/*#include "nam_arp.h"*/
/*#include "nam_route.h"*/
#if 0

#define IN
#define OUT
#define INOUT
#define GT_IPADDR unsigned long 

typedef unsigned long GT_BOOL;
typedef unsigned long GT_STATUS;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;
typedef unsigned short  GT_TRUNK_ID;

typedef struct
{
	unsigned char       arEther[6];
}GT_ETHERADDR;


typedef enum {
	CPSS_TUNNEL_IPV4_OVER_IPV4_E        = 0,
	CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E    = 1,
	CPSS_TUNNEL_IPV6_OVER_IPV4_E        = 2,
	CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E    = 3,
	CPSS_TUNNEL_IP_OVER_MPLS_E          = 4,
	CPSS_TUNNEL_ETHERNET_OVER_MPLS_E    = 5,
	CPSS_TUNNEL_X_OVER_IPV4_E           = 6,
	CPSS_TUNNEL_X_OVER_GRE_IPV4_E       = 7,
	CPSS_TUNNEL_X_OVER_MPLS_E           = 8,
	CPSS_TUNNEL_IP_OVER_X_E             = 9,
	CPSS_TUNNEL_INVALID_E               = 10

} CPSS_TUNNEL_TYPE_ENT;

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
	device surport here :
	case CPSS_PACKET_CMD_ROUTE_E:
		pktCommand = 0;
		break;
	case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
		pktCommand = 1;
		break;
	case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
		pktCommand = 2;
		break;
	case CPSS_PACKET_CMD_DROP_HARD_E:
		pktCommand = 3;
		break;
	case CPSS_PACKET_CMD_DROP_SOFT_E:
		pktCommand = 4;
		break;
*/

typedef enum 
{
    CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E            = 0,
    CPSS_TUNNEL_PASSENGER_PACKET_IPV6_E            = 1,
    CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_NO_CRC_E = 2,
    CPSS_TUNNEL_PASSENGER_PACKET_ETHERNET_CRC_E    = 3

} CPSS_TUNNEL_PASSENGER_PACKET_ENT;

/*
 * typedef: enum CPSS_DXCH_TUNNEL_QOS_MODE_ENT
 *
 * Description: Enumeration of QoS modes.
 *
 * Enumerations:
 *     
 *  CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E  - keep QoS profile assignment from 
 *                                          previous mechanisms
 *  CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E  - trust passenger protocol 802.1p 
user 
 *                                          priority; relevant to Ethernet-
over-MPLS
 *                                          packets only
 *  CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E    - trust passenger protocol IP 
DSCP
 *  CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E - trust passenger protocol 802.
1p user 
 *                                              priority and IP DSCP
 *  CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E - trust outer label MPLS EXP in the 
 *                                          tunneling header
 *  CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E    - untrust packet QoS, assign QoS 
profile 
 *                                          according to tunnel termination 
entry 
 *                                          QoS profile assignment
 *  CPSS_DXCH_TUNNEL_QOS_INVALID_E        - invalid tunnel QoS mode 
 *  
 */
typedef enum 
{
	CPSS_DXCH_TUNNEL_QOS_KEEP_PREVIOUS_E        = 0,
	CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_E        = 1,
	CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_DSCP_E      = 2,
	CPSS_DXCH_TUNNEL_QOS_TRUST_PASS_UP_DSCP_E   = 3,
	CPSS_DXCH_TUNNEL_QOS_TRUST_MPLS_EXP_E       = 4,
	CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E          = 5,
	CPSS_DXCH_TUNNEL_QOS_INVALID_E              = 6

} CPSS_DXCH_TUNNEL_QOS_MODE_ENT;

typedef enum
{
    /* L2 Bridge */
    CPSS_NET_CONTROL_E                          = 1,
    CPSS_NET_UN_KNOWN_UC_E                      = 10,
    CPSS_NET_UN_REGISTERD_MC_E                  = 11,
    CPSS_NET_CONTROL_BPDU_E                     = 16,
    CPSS_NET_CONTROL_DEST_MAC_TRAP_E            = 17,
    CPSS_NET_CONTROL_SRC_MAC_TRAP_E             = 18,
    CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E         = 19,
    CPSS_NET_CONTROL_MAC_RANGE_TRAP_E           = 20,
    /* Pre-egress engine */
    CPSS_NET_RX_SNIFFER_TRAP_E                  = 21,
    CPSS_NET_INTERVENTION_ARP_E                 = 32,
    CPSS_NET_INTERVENTION_IGMP_E                = 33,
    CPSS_NET_INTERVENTION_SA_E                  = 34,
    CPSS_NET_INTERVENTION_DA_E                  = 35,
    CPSS_NET_INTERVENTION_SA_DA_E               = 36,
    CPSS_NET_INTERVENTION_PORT_LOCK_E           = 37,
    CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E             = 38,
    CPSS_NET_EXP_TNL_BAD_VLAN_E                 = 39,
    /* MLL */
    CPSS_NET_MLL_RPF_TRAP_E                     = 120,
    /* Pre-router */
    CPSS_NET_RESERVED_SIP_TRAP_E                = 128,
    CPSS_NET_INTERNAL_SIP_TRAP_E                = 129,
    CPSS_NET_SIP_SPOOF_TRAP_E                   = 130,
    /* PCL */
    CPSS_NET_DEF_KEY_TRAP_E                     = 132,
    CPSS_NET_IP_CLASS_TRAP_E                    = 133,
    CPSS_NET_CLASS_KEY_TRAP_E                   = 134,
    CPSS_NET_TCP_RST_FIN_TRAP_E                 = 135,
    CPSS_NET_CLASS_KEY_MIRROR_E                 = 136,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E           = 137,
    CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E           = 138,
    CPSS_NET_TRAP_UD_INVALID_E                  = 139,
    /* IPv4 */
    CPSS_NET_RESERVED_DIP_TRAP_E                = 144,
    CPSS_NET_MC_BOUNDARY_TRAP_E                 = 145,
    CPSS_NET_INTERNAL_DIP_E                     = 146,
    CPSS_NET_IP_ZERO_TTL_TRAP_E                 = 147,
    CPSS_NET_BAD_IP_HDR_CHECKSUM_E              = 148,
    CPSS_NET_RPF_CHECK_FAILED_E                 = 149,
    CPSS_NET_OPTIONS_IN_IP_HDR_E                = 150,
    CPSS_NET_END_OF_IP_TUNNEL_E                 = 151,
    CPSS_NET_BAD_TUNNEL_HDR_E                   = 152,
    CPSS_NET_IP_HDR_ERROR_E                     = 153,
    CPSS_NET_ROUTE_ENTRY_TRAP_E                 = 154,
    CPSS_NET_DIP_CHECK_ERROR_E                  = 155,
    CPSS_NET_ILLEGAL_DIP_E                      = 156,
    CPSS_NET_POLICY_TRAP_E                      = 157,
    CPSS_NET_DEFAULT_ROUTE_TRAP_E               = 158,
    /*MTU Exceeded */
    CPSS_NET_IP_MTU_EXCEED_E                    = 161,
    CPSS_NET_MPLS_MTU_EXCEED_E                  = 162,
    CPSS_NET_CLASS_MTU_EXCEED_E                 = 163,
    /*Mpls */
    CPSS_NET_MPLS_ZERO_TTL_TRAP_E               = 171,
    CPSS_NET_NHLFE_ENTRY_TRAP_E                 = 172,
    CPSS_NET_ILLEGAL_POP_E                      = 173,
    CPSS_NET_INVALID_MPLS_IF_E                  = 174,

    /* Start of additional Values for Tiger use only */
    /* Tiger IPv6 codes */
    CPSS_NET_IPV6_ICMP_MLD_E                    = 175,
    CPSS_NET_IPV6_IGMP_E                        = 176,
    CPSS_NET_IPV6_MC_PCL_E                      = 186,
    CPSS_NET_IPV6_HOP_BY_HOP_E                  = 187,
    CPSS_NET_IPV6_EXT_HEADER_E                  = 188,
    CPSS_NET_IPV6_BAD_HEADER_E                 = 189,
    CPSS_NET_IPV6_ILLIGAL_DIP_E                 = 190,
    CPSS_NET_IPV6_DIP_ERROR_E                  = 191,
    CPSS_NET_IPV6_ROUTE_TRAP_E                  = 192,
    CPSS_NET_IPV6_ROUTER_RPF_E                  = 193,
    CPSS_NET_IPV6_SCOP_FAIL_E                   = 194,
    CPSS_NET_IPV6_TTL_TRAP_E                    = 195,
    CPSS_NET_IPV6_DEFAULT_ROUTE_E               = 196,
    /* Tiger Mirror codes */
    CPSS_NET_MIRROR_PCL_RST_FIN_E               = 201,
    CPSS_NET_MIRROR_PCL_PCE_E                   = 202,
    CPSS_NET_MIRROR_BRIDGE_E                    = 203,
    CPSS_NET_MIRROR_MLD_E                       = 204,
    CPSS_NET_MIRROR_IGMP_E                      = 205,
    CPSS_NET_MIRROR_RIPV1_E                     = 206,
    CPSS_NET_MIRROR_TTL_E                       = 207,
    CPSS_NET_MIRROR_RPF_E                       = 208,
    CPSS_NET_MIRROR_IP_OPTIONS_E                = 209,
    CPSS_NET_MIRROR_ROUTE_E                     = 210,
    CPSS_NET_MIRROR_ICMP_E                      = 211,
    CPSS_NET_MIRROR_HOP_BY_HOP_E                = 212,
    CPSS_NET_MIRROR_EXT_HEADER_E                = 213,
    CPSS_NET_MIRROR_HEADER_ERROR_E              = 214,
    CPSS_NET_MIRROR_ILLIGAL_IP_E                = 215,
    CPSS_NET_MIRROR_SPECIAL_DIP_E               = 216,
    CPSS_NET_MIRROR_IP_SCOP_E                   = 217,
    CPSS_NET_MIRROR_IP_MTU_E                    = 218,
    CPSS_NET_MIRROR_INLIF_E                     = 219,
    CPSS_NET_MIRROR_PCL_MTU_E                   = 220,
    CPSS_NET_MIRROR_DEFAULT_ROUTE_E             = 221,
    CPSS_NET_MIRROR_MC_ROUTE1_E                 = 222,
    CPSS_NET_MIRROR_MC_ROUTE2_E                 = 223,
    CPSS_NET_MIRROR_MC_ROUTE3_E                 = 224,
    CPSS_NET_MIRROR_IPV4_BC_E                   = 225,
    /*End of additional Values for Tiger use only */

    /*Egress pipe */
    CPSS_NET_ETH_BRIDGED_LLT_E                  = 248,
    CPSS_NET_IPV4_ROUTED_LLT_E                  = 249,
    CPSS_NET_UC_MPLS_LLT_E                      = 250,
    CPSS_NET_MC_MPLS_LLT_E                      = 251,
    CPSS_NET_IPV6_ROUTED_LLT_E                  = 252,
    CPSS_NET_L2CE_LLT_E                         = 253,
    CPSS_NET_EGRESS_MIRROR_TO_CPU_E             = 254,
    
    /* addition values for DXSal devices use only */
    CPSS_NET_UNKNOWN_UC_E                       = 261,
    CPSS_NET_UNREG_MC_E                         = 262,

    /* additional values for DXSal,DXCH and EXMXPM devices */
    CPSS_NET_LOCK_PORT_MIRROR_E                 = 271,

    /* addition values for DXCH and EXMXPM devices */
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E        = 300,
    CPSS_NET_IPV6_ICMP_PACKET_E                 = 301,
    CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E         = 302,
    CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E       = 303,
    CPSS_NET_IPV4_BROADCAST_PACKET_E            = 304,
    CPSS_NET_NON_IPV4_BROADCAST_PACKET_E        = 305,
    CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E        = 306,
    CPSS_NET_UNREGISTERED_MULTICAST_E           = 307,
    CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E      = 308,
    CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E      = 309,

    CPSS_NET_UDP_BC_MIRROR_TRAP0_E              = 322,
    CPSS_NET_UDP_BC_MIRROR_TRAP1_E              = 323,
    CPSS_NET_UDP_BC_MIRROR_TRAP2_E              = 324,
    CPSS_NET_UDP_BC_MIRROR_TRAP3_E              = 325,

    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E      = 326,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E      = 327,
    CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E      = 328,

    CPSS_NET_BRIDGED_PACKET_FORWARD_E           = 400,
    CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E      = 401,
    CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E       = 402,
    CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E           = 403,
    CPSS_NET_CPU_TO_CPU_E                       = 404,
    CPSS_NET_EGRESS_SAMPLED_E                   = 405,
    CPSS_NET_INGRESS_SAMPLED_E                  = 406,
    CPSS_NET_INVALID_PCL_KEY_TRAP_E             = 409,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E = 410,
    CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E       = 411,
    CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E        = 412,
    CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E        = 413,
    CPSS_NET_ROUTED_PACKET_FORWARD_E               = 414,
    CPSS_NET_IP_DIP_DA_MISMATCH_E                  = 415,
    CPSS_NET_IP_UC_SIP_SA_MISMATCH_E               = 416,

    CPSS_NET_IPV4_UC_ROUTE1_TRAP_E                 = 417,
    CPSS_NET_IPV4_UC_ROUTE2_TRAP_E                 = 418,
    CPSS_NET_IPV4_UC_ROUTE3_TRAP_E                 = 419,
    CPSS_NET_IPV4_MC_ROUTE0_TRAP_E                 = 420,
    CPSS_NET_IPV4_MC_ROUTE1_TRAP_E                 = 421,
    CPSS_NET_IPV4_MC_ROUTE2_TRAP_E                 = 422,
    CPSS_NET_IPV4_MC_ROUTE3_TRAP_E                 = 423,

    CPSS_NET_IPV6_UC_ROUTE1_TRAP_E                 = 424,
    CPSS_NET_IPV6_UC_ROUTE2_TRAP_E                 = 425,
    CPSS_NET_IPV6_UC_ROUTE3_TRAP_E                 = 426,
    CPSS_NET_IPV6_MC_ROUTE0_TRAP_E                 = 427,
    CPSS_NET_IPV6_MC_ROUTE1_TRAP_E                 = 428,
    CPSS_NET_IPV6_MC_ROUTE2_TRAP_E                 = 430,
    CPSS_NET_IPV6_MC_ROUTE3_TRAP_E                 = 431,
    CPSS_NET_IP_UC_RPF_FAIL_E                      = 432,
    CPSS_NET_ARP_BC_TO_ME_E                        = 433,

    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E = 434,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E = 435,
    CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E = 436,
    CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E           = 437,
    CPSS_NET_IPV4_TT_HEADER_ERROR_E                  = 438,
    CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E             = 439,
    CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E               = 440,
    CPSS_NET_ARP_REPLY_TO_ME_E                       = 441,
    CPSS_NET_CPU_TO_ALL_CPUS_E                       = 442,
    CPSS_NET_TCP_SYN_TO_CPU_E                        = 443,

    CPSS_NET_INLIF_MIRROR_E                          = 444,
    CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E             = 445,
    CPSS_NET_MC_ROUTED_PACKET_FORWARD_E              = 446,
    CPSS_NET_L2VPN_PACKET_FORWARD_E                  = 447,
    CPSS_NET_MPLS_PACKET_FORWARD_E                   = 448,
    CPSS_NET_UNREGISTERED_MC_EGRESS_FILTER_E         = 449,
    CPSS_NET_UNKNOWN_UC_EGRESS_FILTER_E              = 450,
    CPSS_NET_ARP_BC_EGRESS_MIRROR_E                  = 451,
    CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E    = 452,
    CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E         = 453,
    CPSS_NET_VPLS_BC_EGRESS_FILTER_E                 = 454,
    CPSS_NET_MC_CPU_TO_CPU_E                         = 455,
    CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E              = 456,
    CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E              = 457,
    CPSS_NET_TT_MPLS_HEADER_CHECK_E                  = 458,
    CPSS_NET_TT_MPLS_TTL_EXCEED_E                    = 459,
    CPSS_NET_TTI_MIRROR_E                            = 460,
    CPSS_NET_MPLS_SRC_FILTERING_E                    = 461,
    CPSS_NET_IPV4_TTL1_EXCEEDED_E                    = 462,
    CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E                 = 463,


    CPSS_NET_FIRST_USER_DEFINED_E               = 500,
    CPSS_NET_LAST_USER_DEFINED_E = (CPSS_NET_FIRST_USER_DEFINED_E + 63),

    CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E            = 1024,
    CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E             = (CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + 255),

    CPSS_NET_ALL_CPU_OPCODES_E                  = 0x7FFFFFFF
}CPSS_NET_RX_CPU_CODE_ENT;


/*
 * typedef: enum CPSS_DXCH_TUNNEL_TERM_EXCEPTION_UNT
 *
 * Description: Enumeration for tunnel termination exceptions.
 *
 * Enumerations:
 *     
 *  CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E -
 *      IPv4 tunnel termination header error. 
 *      IPv4 Tunnel Termination Header Error exception is detected if ANY of 
 *      the following criteria are NOT met:
 *        - IPv4 header <Checksum> is correct
 *        - IPv4 header <Version> = 4
 *        - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
 *        - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length
> / 4
 *        - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= 
MAC layer packet byte count
 *        - IPv4 header <SIP> != IPv4 header <DIP>
 *
 *  CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E - 
 *      IPv4 tunnel termination option/fragment error.
 *      An IPv4 Tunnel Termination Option/Fragment error occurs if the packet 
 *      has either IPv4 Options or is fragmented.
 *        - IPv4 Options are detected if the IPv4 header <IP Header Len> > 5.
 *        - The packet is considered fragmented if either the IPv4 header flag 
 *          <More Fragments> is set, or if the IPv4 header <Fragment offset> 
> 0. 
 *          A tunnel-terminated fragmented packet must be reassembled prior 
 *          to further processing of the passenger packet.
 *
 *  CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E - 
 *      IPv4 tunnel termination unsupported GRE header error.
 *      An IPv4 Tunnel Termination GRE Header error occurs if the IPv4 header 
 *      <IP Protocol> = 47 (GRE) and the 16 most significant bits of the GRE 
 *      header are not all 0.
 *  
 */
typedef enum 
{
	CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       = 0,
	CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  = 1,
	CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    = 2

} CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT;


/*
 * typedef: enum CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT
 *
 * Description: Enumeration of tunnel start entry QoS mark modes.
 *
 * Enumerations:
 *     
 *  CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E - 
 *      Mark based on the explicit QoS fields in Tunnel Start entry. 
 *
 *  CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E - 
 *      Mark based on the packet QoS Profile assignment. The QoS Profile table 
 *      entry provides the tunnel header QoS fields.
 *  
 */
typedef enum 
{
	CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E                = 0,
	CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E   = 1

} CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT;

typedef enum {
    CPSS_INTERFACE_PORT_E = 0,
    CPSS_INTERFACE_TRUNK_E,
    CPSS_INTERFACE_VIDX_E,
    CPSS_INTERFACE_VID_E
}CPSS_INTERFACE_TYPE_ENT;

typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        GT_U8   devNum;
        GT_U8   portNum;
    }devPort;

    GT_TRUNK_ID  trunkId;
    GT_U16       vidx;
    GT_U16       vlanId;
#ifdef DRV_LIB_CPSS_3_4
	unsigned char		 devNum;
	unsigned short		 fabricVidx;
	unsigned int		 index;
#endif
}CPSS_INTERFACE_INFO_STC;

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;

/*
 * typedef: struct CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC
 *
 * Description: Struct of configuration for X-over-IPv4 tunnel start. 
 *
 * Fields:
 *     
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - Relevant only if entry <tagEnable> is set. This 
 *                      field is the VID in the Tunnel VLAN tag (0..4095)
 *  upMarkMode        - Relevant only if entry <tagEnable> is set.
 *                      Options:
 *                      - Set user priority according to entry <802.1p UP>.
 *                      - Set user priority according to packet assigned QoS 
attributes
 *  up                - The tag 802.1p user priority (0..7)
 *                      Relevant only if entry <tagEnable> is set and entry
 *                      <upMarkMode> is set according to entry <802.1p UP>.
 *  dscpMarkMode      - Options:
 *                      - Set DSCP according to entry <DSCP>.
 *                      - Set DSCP according to packet QoS Profile assignment.
 *  dscp              - The DSCP in the IPv4 tunnel header (0..63)
 *                      Relevant only if entry <dscpMarkMode> is according 
 *                      to entry <DSCP>.
 *  macDa             - Tunnel next hop MAC DA
 *  dontFragmentFlag  - Don't fragment flag in the tunnel IPv4 header
 *  ttl               - tunnel IPv4 header TTL (0..255)
 *                          0:     use passenger packet TTL
 *                          1-255: use this field for header TTL 
 *  autoTunnel        - Relevant for IPv6-over-IPv4 and IPv6-over-GRE-IPv4 
only,
 *                      if GT_TRUE, the IPv4 header DIP is derived from IPv6 
 *                      passenger packet and destIp field is ignored
 *  autoTunnelOffset  - Relevant for IPv6-over-IPv4 or IPv6-over-GRE-IPv4 
only (0..15)
 *                      If <autoTunnel> is GT_TRUE, this field is the offset 
 *                      of IPv4 destination address inside IPv6 destination 
 *                      address. Offset is measured in bytes between LSBs of 
 *                      the addresses:
 *                          1. IPv4 compatible & ISATAP = 0
 *                          2. 6to4 = 10 (decimal)
 *  destIp            - Tunnel destination IP
 *  srcIp             - Tunnel source IP
 *  
 */
typedef struct
{
	GT_BOOL         tagEnable;
	GT_U16          vlanId;
	CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    upMarkMode;
	unsigned int    up;
	CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    dscpMarkMode;
	unsigned int    dscp;
	GT_ETHERADDR    macDa;
	GT_BOOL         dontFragmentFlag;
	unsigned int    ttl;
	GT_BOOL         autoTunnel;
	unsigned int    autoTunnelOffset;
	GT_IPADDR       destIp;
	GT_IPADDR       srcIp;

} CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC;


/*
 * typedef: struct CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC
 *
 * Description: Struct of configuration for X-over-MPLS tunnel start 
 *
 * Fields:
 *     
 *  tagEnable         - If set, the packet is sent with VLAN tag
 *  vlanId            - Relevant only if entry <tagEnable> is set. This 
 *                      field is the VID in the Tunnel VLAN tag (0..4095)
 *  upMarkMode        - Relevant only if entry <tagEnable> is set.
 *                      Options:
 *                      - Set user priority according to entry <802.1p UP>.
 *                      - Set user priority according to packet assigned QoS 
attributes
 *  up                - The tag 802.1p user priority (0..7)
 *                      Relevant only if entry <tagEnable> is set and entry
 *                      <upMarkMode> is set according to entry <802.1p UP>.
 *  macDa             - Tunnel next hop MAC DA
 *  numLabels         - number of MPLS labels (1 or 2)
 *  ttl               - tunnel IPv4 header TTL (0..255)
 *                          0:     use passenger packet TTL
 *                          1-255: use this field for header TTL 
 *  label1            - Relevant only when number of labels is 2; (0..1048575)
 *                      MPLS label 1; if <numLabels> = 2 then this is the 
 *                      inner label
 *  exp1MarkMode      - Relevant only when number of labels is 2;
 *                      Options:
 *                      - Set EXP1 according to entry <exp1>.
 *                      - Set EXP1 according to packet QoS Profile assignment 
 *                        to EXP mapping table.
 *  exp1              - The EXP1 value (0..7)
 *                      Relevant only when number of labels is 2 and entry 
 *                      <exp1MarkMode> is set according to entry <EXP1>.
 *  label2            - MPLS label 2; (0..1048575) if <numLabels> = 2 then 
 *                      this is the outer label, immediately following the 
MPLS 
 *                      Ether Type
 *  exp2MarkMode      - Options:
 *                      - Set EXP2 according to entry <exp2>.
 *                      - Set EXP2 according to packet QoS Profile assignment 
 *                        to EXP mapping table.
 *  exp2              - The EXP2 value (0..7)
 *                      Relevant if entry <exp2MarkMode> is set to according 
 *                      to entry <EXP2>.
 *  retainCRC         - Relevant for Ethernet-over-MPLS. If GT_TRUE, retain 
 *                      the passenger packet CRC and add additional 4-byte CRC 
 *                      based on the tunneling header packet. 
 *  
 */
typedef struct 
{
	GT_BOOL         tagEnable;
	GT_U16          vlanId;
	CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    upMarkMode;
	unsigned int    up;
	GT_ETHERADDR    macDa;
	unsigned int    numLabels;
	unsigned int    ttl;
	unsigned int    label1;
	CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    exp1MarkMode;
	unsigned int    exp1;
	unsigned int    label2;
	CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT    exp2MarkMode;
	unsigned int    exp2;
	GT_BOOL         retainCRC;

} CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC;


/*
 * typedef: union CPSS_DXCH_TUNNEL_START_CONFIG_UNT
 *
 * Description: Union for configuration for tunnel start (X-over-MPLS tunnel 
 *              start or X-over-IPv4 tunnel start). 
 *
 * Fields:
 *     
 *  ipv4Cfg           - configuration for X-over-IPv4 tunnel start
 *  mplsCfg           - configuration for X-over-MPLS tunnel start
 *  
 */
typedef union 
{
	CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC  ipv4Cfg;
	CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC  mplsCfg;

} CPSS_DXCH_TUNNEL_START_CONFIG_UNT;


/*
 * typedef: struct CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC
 *
 * Description: Struct for configuration for ipv4 tunnel termination. 
 *
 * Fields:
 *
 *  srcPortTrunk  - Source port or Source trunk-ID
 *                  if port, range (0..63)
 *                  if trunk, range (0..127)
 *  srcIsTrunk    - whether source is port or trunk (0..1)
 *                  0: Source is not a trunk (i.e. it is port)
 *                  1: Source is a trunk 
 *  srcDev        - Source device (0..31)
 *  vid           - packet VID assignment (0..4095)
 *  macDa         - MAC DA of the tunnel header 
 *  srcIp         - tunnel source IP 
 *  destIp        - tunnel destination IP 
 *  
 */
typedef struct
{
	unsigned int            srcPortTrunk;
	unsigned int            srcIsTrunk;
	GT_U8                   srcDev;
	GT_U16                  vid;
	GT_ETHERADDR            macDa;
	GT_IPADDR               srcIp;
	GT_IPADDR               destIp;

} CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC;


/*
 * typedef: struct CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC
 *
 * Description: Struct for configuration for MPLS tunnel termination.
 *
 * Fields:
 *     
 *  srcPortTrunk  - Source port or Source trunk-ID
 *                  if port, range (0..63)
 *                  if trunk, range (0..127)
 *  srcIsTrunk    - whether source is port or trunk (0..1)
 *                  0: Source is not a trunk (i.e. it is port)
 *                  1: Source is a trunk 
 *  srcDev        - Source device (0..31)
 *  vid           - packet VID assignment (0.4095)
 *  macDa         - MAC DA of the tunnel header
 *  label1        - MPLS label 1; in case packet arrives with 2 MPLS 
 *                  labels, this is the inner label (0..1048575)
 *  sBit1         - stop bit for MPLS label 1 (0..1)
 *  exp1          - EXP of MPLS label 1 (0..7)
 *  label2        - MPLS label 2; this is the outer label, immediately 
 *                  following the MPLS Ether Type (0..1048575)
 *  sBit2         - stop bit for MPLS label 2 (0..1)
 *  exp2          - EXP of MPLS label 2 (0..7)
 *  
 * Comments:
 *      MPLS S-Bit1 and S-Bit2 are derived from the number of labels. 
 *
 */
typedef struct 
{
	unsigned int                  srcPortTrunk;
	unsigned int                  srcIsTrunk;
	GT_U8                   srcDev;
	GT_U16                  vid;
	GT_ETHERADDR            macDa;
	unsigned int                  label1;
	unsigned int                  sBit1;
	unsigned int                  exp1;
	unsigned int                  label2;
	unsigned int                  sBit2;
	unsigned int                  exp2;

} CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC;


/*
 * typedef: union CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT
 *
 * Description: Union for configuration for tunnel termination (X-over-MPLS 
 *              tunnel termination or X-over-IPv4 tunnel termination).
 *
 * Fields:
 *     
 *  ipv4Cfg           - configuration for X-over-IPv4 tunnel termination
 *  mplsCfg           - configuration for X-over-MPLS tunnel termination
 *  
 */
typedef union 
{
	CPSS_DXCH_TUNNEL_TERM_IPV4_CONFIG_STC   ipv4Cfg;
	CPSS_DXCH_TUNNEL_TERM_MPLS_CONFIG_STC   mplsCfg;

} CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT;


/*
 * typedef: struct CPSS_DXCH_TUNNEL_TERM_ACTION_STC
 *
 * Description: Struct for tunnel termination action. 
 *
 * Fields:
 *     
 *  command           - tunnel termination entry forwarding command
 *  userDefinedCpuCode    - user defined cpu code. relavant only if <cmd> is 
 *                          trap or mirror (CPSS_NET_FIRST_USER_DEFINED_E .. 
 *                          CPSS_NET_LAST_USER_DEFINED_E)
 *  passengerPacketType   - type of passenger packet
 *  egressInterface   - egress interface to redirect packet. Relevant only 
 *                      to Ethernet-over-MPLS tunnel ternimation. IPv4/6-over-
X 
 *                      are processed by the ingree pipeline. The egress 
 *                      interface can be {Device, Port}, Trunk-ID or VLAN.
 *  isTunnelStart     - whether <outInterface> is tunnel start. Relevant only 
 *                      to Ethernet-over-MPLS tunnel termination.
 *  tunnelStartIdx    - tunnel start entry index in case <isTunnelStart> 
 *                      is GT_TRUE (0..1023)
 *  vlanId            - vlan Id assigned to the tunnel termination packet. Note
 *                      that the VID is set by the TT action regardless to the
 *                      incoming packet <VID Precedence> (0..4095)
 *  vlanPrecedence    - whether the VID assignment can be overridden by 
 *                      subsequent VLAN assignment mechanism (the Policy 
engine)
 *  nestedVlanEnable  - Relevant only for Ethernet-over-MPLS tunnel 
termination.
 *                      If GT_TRUE then, in terms of egress tagging, the 
 *                      passenger packet is always considered to received as 
 *                      untagged. 
 *  copyTtlFromTunnelHeader   - Relevant only for IPv4/6-over-X tunnel 
termination.
 *                              GT_TRUE:  the Router engine uses the IPv4 
tunnel 
 *                              header TTL as the incoming TTL.
 *                              GT_FALSE: the Router engine ignores the IPv4 
 *                              tunnel header TTL and uses the passenger 
 *                              packet TTL as the incoming TTL.
 *  qosMode           - QoS mode type (refer to GT_TUNNEL_QOS_MODE_TYPE)
 *  qosPrecedence     - whether QoS profile can be overridden by subsequent 
 *                      pipeline engines
 *  qosProfile        - QoS profile to assign to packet in case <qosMode> 
 *                      is GT_TUNNEL_QOS_UNTRUST_PKT (0..127)
 *  defaultUP         - default user priority (0..7)
 *  modifyUP          - Relevant only if the packet is Ethernet-over-MPLS.
 *                      Whether to modify user priority and how to modify it.
 *                      NOTE: The Tunnel Termination Action Assigns may 
 *                      override the packet <modifyUP> assignment regardless 
 *                      of its QoS Precedence.
 *  remapDSCP         - Whether to remap the DSCP. Relevant only if the 
 *                      passenger packet is IPv4/6 and the entry <qosMode> is 
 *                      configured to trust the passenger DSCP.
 *  modifyDSCP        - Whether to modify DSCP and how to modify it. 
 *                      Relevant for IPv4/6-over-X, or, if the packet is 
 *                      Ethernet-over-MPLS and the Ethernet passenger packet 
 *                      is IPv4/6.
 *                      NOTE: The Tunnel Termination Action Assigns may 
 *                      override the packet <modifyDSCP> assignment regardless 
 *                      of its QoS Precedence.
 *  mirrorToIngressAnalyzerEnable - If set, the packet is mirrored to the 
Ingress 
 *                                  Analyzer port.
 *  policerEnable     - If set, the packet is policed according to the traffic 
 *                      profile defined in the specified <policerIndex>.
 *  policerIndex      - the policer index to use in case <enPolicer> is GT_TRUE
 *                      (0..255)
 *  matchCounterEnable    - Whether to increment counter upon matching tunnel 
 *                          termination key.
 *                          Options:
 *                          - Do not increment a Policy engine match counter 
for 
 *                            this packet.
 *                          - Increment the Policy engine match counter 
defined 
 *                            by the entry <matchCounterIndex>.
 *  matchCounterIndex - For packets matching this Tunnel Termination rule, 
 *                      the respective Policy engine counter is incremented.
 *  
 */
typedef struct
{
	CPSS_PACKET_CMD_ENT                         command;
	CPSS_NET_RX_CPU_CODE_ENT                    userDefinedCpuCode;
	CPSS_TUNNEL_PASSENGER_PACKET_ENT            passengerPacketType;
	CPSS_INTERFACE_INFO_STC                     egressInterface;
	GT_BOOL                                     isTunnelStart;
	unsigned int                                tunnelStartIdx;
	GT_U16                                      vlanId;
	CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT vlanPrecedence;
	GT_BOOL                                     nestedVlanEnable;
	GT_BOOL                                     copyTtlFromTunnelHeader;
	CPSS_DXCH_TUNNEL_QOS_MODE_ENT               qosMode;
	CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence; 
	unsigned int                                qosProfile   ;
	unsigned int                                defaultUP;
	CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUP;
	GT_BOOL                                     remapDSCP;
	CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDSCP;
	GT_BOOL                                     mirrorToIngressAnalyzerEnable;
	GT_BOOL                                     policerEnable;
	unsigned int                                policerIndex;
	GT_BOOL                                     matchCounterEnable;
	unsigned int                                matchCounterIndex;

} CPSS_DXCH_TUNNEL_TERM_ACTION_STC;

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

typedef enum
{
    CPSS_IP_CNT_SET0_E   = 0,
    CPSS_IP_CNT_SET1_E   = 1,
    CPSS_IP_CNT_SET2_E   = 2,
    CPSS_IP_CNT_SET3_E   = 3,
    CPSS_IP_CNT_NO_SET_E = 4
}CPSS_IP_CNT_SET_ENT;

typedef enum
{
    CPSS_IP_SITE_ID_INTERNAL_E,
    CPSS_IP_SITE_ID_EXTERNAL_E
} CPSS_IP_SITE_ID_ENT;

typedef struct CPSS_DXCH_IP_UC_ROUTE_ENTRY_STCT
{
    CPSS_PACKET_CMD_ENT                 cmd;
    CPSS_DXCH_IP_CPU_CODE_INDEX_ENT     cpuCodeIdx;
    GT_BOOL                             appSpecificCpuCodeEnable;
    GT_BOOL                             unicastPacketSipFilterEnable;
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_BOOL                             qosProfileMarkingEnable;
    unsigned int                              qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_IP_CNT_SET_ENT                 countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    unsigned int                              sipAccessLevel;
    unsigned int                              dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_BOOL                             scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                 siteId;
    unsigned int                              mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    GT_U16                              nextHopVlanId;
    CPSS_INTERFACE_INFO_STC             nextHopInterface;
    unsigned int                              nextHopARPPointer;
    unsigned int                              nextHopTunnelPointer;

}CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC;

typedef enum
{
    CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E = 0,
    CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E = 1
}CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT;

typedef enum
{
    CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E   = 0,
    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E   = 1,
    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E = 2,
    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E       = 3 
}CPSS_IPV6_PREFIX_SCOPE_ENT;

typedef struct CPSS_DXCH_IP_LTT_ENTRY_STCT
{
    CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT routeType;
    unsigned int                              numOfPaths;
    unsigned int                              routeEntryBaseIndex;
    GT_BOOL                             ucRPFCheckEnable;
    GT_BOOL                             sipSaCheckMismatchEnable;
    CPSS_IPV6_PREFIX_SCOPE_ENT          ipv6MCGroupScopeLevel;
}CPSS_DXCH_IP_LTT_ENTRY_STC;

typedef union CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNTT
{
	/*CPSS_DXCH_PCL_ACTION_STC pclIpUcAction;*/
	CPSS_DXCH_IP_LTT_ENTRY_STC ipLttEntry;
}CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT;

/*
 * typedef: enum CPSS_DXCH_TTI_KEY_TYPE_ENT
 *
 * Description:
 *      TTI key type.
 *
 * Enumerations:
 *
 *      CPSS_DXCH_TTI_KEY_IPV4_E    - IPv4 TTI key type
 *      CPSS_DXCH_TTI_KEY_MPLS_E    - MPLS TTI key type
 *      CPSS_DXCH_TTI_KEY_ETH_E     - Ethernet TTI key type
 *      CPSS_DXCH_TTI_KEY_MIM_E     - Mac in Mac TTI key type
 *
 */
typedef enum
{
    CPSS_DXCH_TTI_KEY_IPV4_E  = 0,
    CPSS_DXCH_TTI_KEY_MPLS_E  = 1,
    CPSS_DXCH_TTI_KEY_ETH_E   = 2,
    CPSS_DXCH_TTI_KEY_MIM_E   = 3

} CPSS_DXCH_TTI_KEY_TYPE_ENT;

extern GT_STATUS cpssDxChTunnelStartEntrySet
(
	IN  GT_U8                               devNum,
	IN  unsigned int                        routerArpTunnelStartLineIndex,
	IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
	IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
);

extern GT_STATUS cpssDxChTunnelStartEntryGet
(
	IN   GT_U8                              devNum,
	IN   unsigned int                       routerArpTunnelStartLineIndex,
	OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
	OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
);	

extern GT_STATUS cpssDxChIpv4TunnelTermPortSet
(
	IN  GT_U8       devNum,
	IN  GT_U8       port,
	IN  GT_BOOL     enable
);

extern GT_STATUS cpssDxChIpv4TunnelTermPortGet
(
	IN  GT_U8       devNum,
	IN  GT_U8       port,
	OUT GT_BOOL     *enablePtr
);

extern GT_STATUS cpssDxChTunnelTermEntrySet
(
	IN  GT_U8                               devNum,
	IN  unsigned int                        routerTunnelTermTcamIndex,
	IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
	IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
	IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
	IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
);

extern GT_STATUS cpssDxChTunnelTermEntryGet
(
	IN  GT_U8                               devNum,
	IN  unsigned int                              routerTunnelTermTcamIndex,
	OUT GT_BOOL                             *validPtr,
	OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
	OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
	OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
	OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
);

extern GT_STATUS cpssDxChTunnelTermEntryInvalidate
(
	IN  GT_U8       devNum,
	IN  unsigned int  routerTunnelTermTcamIndex
);

extern GT_STATUS cpssDxChTunnelStartEgressFilteringSet
(
	IN  GT_U8   devNum,
	IN  GT_BOOL enable
);

extern GT_STATUS cpssDxChTunnelStartEgressFilteringGet
(
	IN  GT_U8   devNum,
	OUT GT_BOOL *enablePtr
);

extern GT_STATUS cpssDxChTunnelTermExceptionCmdSet
(
	IN  GT_U8                               devNum,
	IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
	IN  CPSS_PACKET_CMD_ENT                 command
);

extern GT_STATUS cpssDxChTunnelCtrlTtExceptionCmdGet
(
	IN  GT_U8                               devNum,
	IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
	OUT CPSS_PACKET_CMD_ENT                 *commandPtr
);

extern unsigned long cpssDxChIpUcRouteEntriesWrite
(
    IN unsigned char	devNum,
    IN unsigned int     baseRouteEntryIndex,
    IN CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN unsigned long    numOfRouteEntries
);

#endif

extern int nam_arp_free_mactbl_index
(
	unsigned int	 val
);

extern int nam_arp_mactbl_index_get
(
   unsigned int val
);

extern int nam_arp_free_nexthop_tbl_index
(
	unsigned int	 val
);

extern int nam_arp_get_mactbl_index
(
	unsigned int	 *val
);

int nam_tunnel_init
(
	void
);

int nam_tunnel_start_set_entry
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex,
	unsigned int tunnelType,
	struct tunnel_item_s	*configPtr
);

int nam_tunnel_start_del_entry
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex,
	unsigned int tunnelType
);

int nam_tunnel_nh_set
(
	unsigned char devNum,
	struct tunnel_item_s	*configPtr
);

int nam_tunnel_nh_del
(
	unsigned char devNum,
	unsigned int  nexthopindex
);

int nam_tunnel_set_tcam_ipltt
(
	unsigned char devnum,
	unsigned int hostdip,
	unsigned int hdiplen,
	struct tunnel_item_s	*configPtr
);

int  nam_tunnel_del_tcam_ipltt
(
	unsigned int dstip,
	unsigned int masklen
);

int nam_tunnel_start_set_check
(
	unsigned char devNum,
	unsigned int routerArpTunnelStartLineIndex
);

int nam_tunnel_ipv4_tunnel_term_port_set
(
	unsigned char       devNum,
	unsigned char       port,
	unsigned long		enable
);

int nam_tunnel_term_entry_set
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex,
	unsigned int  tunnelType,
	struct tunnel_item_s    *configPtr
);

int nam_tunnel_term_entry_del
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex,
	unsigned int  tunnelType
);

int nam_tunnel_term_entry_invalidate
(
	unsigned char devNum,
	unsigned int routerTunnelTermTcamIndex
);

#endif

