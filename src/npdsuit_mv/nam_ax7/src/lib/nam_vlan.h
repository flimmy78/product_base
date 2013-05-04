#ifndef __NAM_VLAN_H__
#define __NAM_VLAN_H__

#define NAM_VLAN_ERR_NONE		0
#define NAM_VLAN_ERR_PARAM	(NAM_VLAN_ERR_NONE + 1)
#define NAM_VLAN_ERR		(NAM_VLAN_ERR_NONE + 2)
#define NAM_VLAN_SUCCESS	0
#define NAM_VLAN_FAIL		0xFFFF
#define	NAM_PP_DEV_NUM		0x2
#define NAM_PP_LOGIC_PORT_NUM_MAX	0x40
#define INVALID_ETH_PORT_INDEX	0xff
#define TRUNK_MEMBER_NUM_MAX    0x8

#define TRUE 1
#define FALSE 0
enum vlan_port_bmp_op_e {
	VLAN_PORT_BMP_ADD = 0,
	VLAN_PORT_BMP_DEL,
	VLAN_PORT_BMP_OP_MAX
};

typedef enum{
    VLAN_DIRECTION_INGRESS_E,
    VLAN_DIRECTION_EGRESS_E,
    VLAN_DIRECTION_BOTH_E
}VLAN_DIRECTION_ENT;

typedef enum
{
    CPSS_VLAN_ETHERTYPE0_E = 1,
    CPSS_VLAN_ETHERTYPE1_E
}CPSS_ETHER_MODE_ENT;
struct DRV_VLAN_PORT_BMP_S {
    unsigned int   ports[2];
};
typedef struct {
	unsigned char portNum;
	unsigned char devNum;
}CPSS_TRUNK_MEMBER_STC;

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

typedef enum {
    PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E = 0,
    PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E, /*3*/
    PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
    PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
    PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,/*6*/
    PRV_CPSS_DXCH_TABLE_REMARKING_E,
    PRV_CPSS_DXCH_TABLE_STG_E,
    PRV_CPSS_DXCH_TABLE_VLAN_E,/*9*/
    PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_CPSS_DXCH_TABLE_FDB_E,
    PRV_CPSS_DXCH_TABLE_POLICER_E,
    PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
    PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
    PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
    PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
    PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,

    /* must be last */
    PRV_CPSS_DXCH_TABLE_LAST_E
}PRV_CPSS_DXCH_TABLE_ENT;

typedef enum
{
    CPSS_DXCH_BRG_IP_CTRL_NONE_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV4_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV6_E,
    CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
}CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT;


typedef enum
{
    CPSS_BRG_IPM_SGV_E,
    CPSS_BRG_IPM_GV_E
}CPSS_BRG_IPM_MODE_ENT;

#ifdef DRV_LIB_CPSS_3_4

typedef unsigned long GT_BOOL;
typedef unsigned int   GT_U32;
typedef unsigned short GT_U16;
typedef unsigned char  GT_U8;

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
    CPSS_PACKET_CMD_NONE_E               /* 09 */
} CPSS_PACKET_CMD_ENT;

typedef enum
{
   CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E,
   CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E
} CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT;

typedef enum
{
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E
} CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT;

typedef enum
{
    CPSS_IP_SITE_ID_INTERNAL_E,
    CPSS_IP_SITE_ID_EXTERNAL_E
} CPSS_IP_SITE_ID_ENT;

/*copy from mcpss3.4*/
struct	brg_vlan_drv_info_s
{
    GT_BOOL                                     unkSrcAddrSecBreach;
    CPSS_PACKET_CMD_ENT                         unregNonIpMcastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv4McastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv6McastCmd;
    CPSS_PACKET_CMD_ENT                         unkUcastCmd;
    CPSS_PACKET_CMD_ENT                         unregIpv4BcastCmd;
    CPSS_PACKET_CMD_ENT                         unregNonIpv4BcastCmd;
    GT_BOOL                                     ipv4IgmpToCpuEn;
    GT_BOOL                                     mirrToRxAnalyzerEn;
    GT_BOOL                                     ipv6IcmpToCpuEn;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT              ipCtrlToCpuEn;
    CPSS_BRG_IPM_MODE_ENT                       ipv4IpmBrgMode;
    CPSS_BRG_IPM_MODE_ENT                       ipv6IpmBrgMode;
    GT_BOOL                                     ipv4IpmBrgEn;
    GT_BOOL                                     ipv6IpmBrgEn;
    CPSS_IP_SITE_ID_ENT                         ipv6SiteIdMode;
    GT_BOOL                                     ipv4UcastRouteEn;
    GT_BOOL                                     ipv4McastRouteEn;
    GT_BOOL                                     ipv6UcastRouteEn;
    GT_BOOL                                     ipv6McastRouteEn;
    GT_U32                                      stgId;
    GT_BOOL                                     autoLearnDisable;
    GT_BOOL                                     naMsgToCpuEn;
    GT_U32                                      mruIdx;
    GT_BOOL                                     bcastUdpTrapMirrEn;
    GT_U32                                      vrfId;
    GT_U16                                      floodVidx;
    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT      floodVidxMode;
    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT   portIsolationMode;
    GT_BOOL                                     ucastLocalSwitchingEn;
    GT_BOOL                                     mcastLocalSwitchingEn;
};
#else

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

struct	brg_vlan_drv_info_s {
    unsigned int	unkSrcAddrSecBreach;
    enum PACKET_CMD_ENT	unregNonIpMcastCmd;
    enum PACKET_CMD_ENT	unregIpv4McastCmd;
    enum PACKET_CMD_ENT	unregIpv6McastCmd;
    enum PACKET_CMD_ENT	unkUcastCmd;
    enum PACKET_CMD_ENT	unregIpv4BcastCmd;
    enum PACKET_CMD_ENT	unregNonIpv4BcastCmd;
    unsigned int	ipv4IgmpToCpuEn;
    unsigned int	mirrToRxAnalyzerEn;
    unsigned int	ipv6IcmpToCpuEn;
    enum BRG_IP_CTRL_TYPE_ENT	ipCtrlToCpuEn;
    enum BRG_IPM_MODE_ENT		ipv4IpmBrgMode;
    enum BRG_IPM_MODE_ENT		ipv6IpmBrgMode;
    unsigned int			ipv4IpmBrgEn;
    unsigned int			ipv6IpmBrgEn;
    enum IP_SITE_ID_ENT	ipv6SiteIdMode;
    unsigned int		ipv4UcastRouteEn;
    unsigned int    	ipv4McastRouteEn;
    unsigned int       	ipv6UcastRouteEn;
    unsigned int     	ipv6McastRouteEn;
    unsigned int     	stgId;
    unsigned int      	autoLearnDisable;
    unsigned int      	naMsgToCpuEn;
    unsigned int    	mruIdx;
    unsigned int      	bcastUdpTrapMirrEn;
};

#endif

typedef enum
{
    CPSS_IP_UNICAST_E    = 0,
    CPSS_IP_MULTICAST_E  = 1
}CPSS_IP_UNICAST_MULTICAST_ENT;

/*
 * typedef: enum CPSS_IP_PROTOCOL_STACK_ENT
 *
 * Description: type of IP stack used
 *
 *
 * Enumerations:
 *  CPSS_IP_PROTOCOL_IPV4_E   - IPv4 stack
 *  CPSS_IP_PROTOCOL_IPV6_E   - IPv6 stack
 *  CPSS_IP_PROTOCOL_IPV4V6_E - dual stack
 */
typedef enum
{
    CPSS_IP_PROTOCOL_IPV4_E     = 0,
    CPSS_IP_PROTOCOL_IPV6_E     = 1,
    CPSS_IP_PROTOCOL_IPV4V6_E   = 2
}CPSS_IP_PROTOCOL_STACK_ENT;


typedef enum
{
    CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E,
    CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E
}CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT;

typedef enum
{
    VLAN_FILTER_UNKOWN_UC = 0,
    VLAN_FILTER_UNREG_NONIP_MC ,
    VLAN_FILTER_UNREG_IPV4_MC ,
    VLAN_FILTER_UNREG_IPV6_MC ,
    VLAN_FILTER_UNREG_NONIPV4_BC ,
    VLAN_FILTER_UNREG_IPV4_BC ,
    VLAN_FILTER_TYPE_MAX,
} VLAN_FLITER_ENT;



/*zhengcaisheng added 20090304*/
typedef enum
{
    CPSS_LOCK_FRWRD_E = 1,
    CPSS_LOCK_DROP_E,
    CPSS_LOCK_TRAP_E,
    CPSS_LOCK_MIRROR_E,
    CPSS_LOCK_SOFT_DROP_E
} CPSS_PORT_LOCK_CMD_ENT;

typedef struct {
	unsigned int portMbr[2];
}PORT_MEMBER_BMP;

/*
 * typedef: enum CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT
 *
 * Description: Enumeration of tagging egress commands.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E              - if Tag0 and/or Tag1
 *                                                          were classified in
 *                                                          the incoming packet,
 *                                                          they are removed
 *                                                          from the packet.
 *    CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E                  - packet egress with
 *                                                          Tag0 (Tag0 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E                  - packet egress with
 *                                                          Tag1 (Tag1 as
 *                                                          defined in ingress
 *                                                          pipe).
 *    CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E - Outer Tag0, Inner
 *                                                          Tag1 (tag swap).
 *    CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E - Outer Tag1, Inner
 *                                                          Tag0 (tag swap).
 *    CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E             - TAG0 is added to
 *                                                          the packet
 *                                                          regardless of
 *                                                          whether Tag0 and
 *                                                          TAG1 were
 *                                                          classified in the
 *                                                          incoming packet.
 *    CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E         - the incoming packet
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
 *      Relevant for devices with TR101 feature support.
 */
typedef enum
{
   CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E,
   CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E
}CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT;


/*
 * Typedef: structure CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
 *
 * Description: structure that hold an array of ports' Tagging commands
 *
 * Fields:
 *      portsCmd - array of ports' tagging commands
 *
 * Note:
 *      Array index is port number.
 */

#define CPSS_MAX_PORTS_NUM_CNS          64

typedef struct{
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portsCmd[CPSS_MAX_PORTS_NUM_CNS];
}CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC;
/* max ports in device in PPs of Prestera family */

/*********************************************************
 *
 *			====extern CPSS MODULE API=====
 *
 *
 ********************************************************/
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
	unsigned int* eth_g_index
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
	unsigned int* eth_g_index
);
#endif
extern unsigned int nam_asic_trunk_get_port_member
(
	unsigned short	trunkId,
	unsigned int* 	memberCont,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
extern unsigned int nam_asic_trunk_get_port_member_bmp
(
	unsigned short	trunkId,
	unsigned int*	dev0MbrBmp,
	unsigned int*	dev1MbrBmp
);
#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDrvPpHwRegisterRead
(
    unsigned char 	devNum,
    unsigned long	portGroupId,
    unsigned long	regAddr,
    unsigned long	*data
);
#else
extern unsigned long cpssDrvPpHwRegisterRead
(
    unsigned char 	devNum,
    unsigned long	regAddr,
    unsigned long	*data
);
#endif
extern unsigned long prvCpssDrvHwPpSetRegField
(
    unsigned char 	devNum,
    unsigned long 	regAddr,
    unsigned long 	fieldOffset,
    unsigned long 	fieldLength,
    unsigned int 	fieldData
);
extern unsigned long cpssDxChBrgVlanPortVidGet
(
    unsigned char 	devNum,
    unsigned char 	port,
    unsigned short 	*vidPtr
);
#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDxChBrgVlanEntryWrite
(
    unsigned char	devNum,
    unsigned short	vlanId,
    struct DRV_VLAN_PORT_BMP_S	*portsMembers,
    struct DRV_VLAN_PORT_BMP_S	*portsTagged,
    struct brg_vlan_drv_info_s	*vlanInfoPtr,
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
);
#else
extern unsigned long cpssDxChBrgVlanEntryWrite
(
    unsigned char	devNum,
    unsigned short	vlanId,
    struct DRV_VLAN_PORT_BMP_S	*portsMembers,
    struct DRV_VLAN_PORT_BMP_S	*portsTagged,
    struct brg_vlan_drv_info_s	*vlanInfoPtr
);
#endif
#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDxChBrgVlanMemberAdd
(
	unsigned char	devNum,
	unsigned short	vlanId,
	unsigned char	portNum,
	unsigned long	isTagged,
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
);
#else
extern unsigned long cpssDxChBrgVlanMemberAdd
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned char   portNum,
    unsigned long	isTagged
);
#endif
extern unsigned long cpssDxChBrgVlanPortIngFltEnable
(
    unsigned char   devNum,
    unsigned char   port,
    unsigned long enable
);
extern unsigned long cpssDxChBrgVlanPortVidSet
(
    unsigned char   devNum,
    unsigned char   port,
    unsigned short  vlanId
);
extern unsigned long cpssDxChBrgVlanPortDelete
(
    unsigned char	devNum,
    unsigned short	vlanId,
    unsigned char	port
);
extern unsigned long prvCpssDxChReadTableEntryField
(
    unsigned char				devNum,
    PRV_CPSS_DXCH_TABLE_ENT 	tableType,
    unsigned int				entryIndex,
    unsigned int                fieldWordNum,
    unsigned int                fieldOffset,
    unsigned int                fieldLength,
    unsigned int				*fieldValuePtr
);
extern unsigned long cpssDxChBrgVlanEntryInvalidate
(
    unsigned char    devNum,
    unsigned short   vlanId
);
#ifdef DRV_LIB_CPSS_3_4
extern unsigned long cpssDxChBrgVlanEntryRead
(
    unsigned char	devNum,
    unsigned short	vlanId,
    struct DRV_VLAN_PORT_BMP_S	*portsMembersPtr,
    struct DRV_VLAN_PORT_BMP_S	*portsTaggingPtr,
    struct brg_vlan_drv_info_s *vlanInfoPtr,
    unsigned long	*isValidPtr,
	CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
);
#else
extern unsigned long cpssDxChBrgVlanEntryRead
(
    unsigned char	devNum,
    unsigned short	vlanId,
    struct DRV_VLAN_PORT_BMP_S	*portsMembersPtr,
    struct DRV_VLAN_PORT_BMP_S	*portsTaggingPtr,
    struct brg_vlan_drv_info_s *vlanInfoPtr,
    unsigned long	*isValidPtr
);
#endif
extern unsigned long cpssDxChBrgVlanEntryValidate
(
    unsigned char    devNum,
    unsigned short   vlanId
);

extern unsigned long prvCpssDxChWriteTableEntryField
(
    unsigned char	devNum,
    PRV_CPSS_DXCH_TABLE_ENT tableType,
    unsigned int	entryIndex,
    unsigned int	fieldWordNum,
    unsigned int	fieldOffset,
    unsigned int	fieldLength,
    unsigned int	fieldValuePtr
);
extern unsigned long cpssDxChTrunkTableEntryGet
(
    unsigned char	devNum,
    unsigned short	trunkId,
    unsigned int	*numMembersPtr,
    CPSS_TRUNK_MEMBER_STC	membersArray[TRUNK_MEMBER_NUM_MAX]
);
extern unsigned long cpssDxChIpPortRoutingEnable
(
    unsigned char	devNum,
    unsigned char	portNum,
    CPSS_IP_UNICAST_MULTICAST_ENT    ucMcEnable,
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    unsigned long	enableRouting
);

extern unsigned long cpssDxChBrgVlanNASecurEnable
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned long 	 enable
);
extern unsigned long cpssDxChBrgVlanEtherTypeSet
(
    unsigned char                   devNum,
    unsigned int     etherTypeMode,
    unsigned short                  etherType,
   	unsigned short                 vidRange
);

extern unsigned long cpssDxChBrgVlanLearningStateSet
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned long  	status
);	
extern unsigned long cpssDxChBrgVlanNaToCpuEnable
(
    unsigned char    devNum,
    unsigned short   vlanId,
    unsigned long   enable
);
extern unsigned long cpssDxChBrgVlanUnkUnregFilterSet
(
    unsigned char	devNum,
    unsigned short	vlanId,
    CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    CPSS_PACKET_CMD_ENT                  cmd
);
extern unsigned long cpssDxChBrgVlanNASecurEnable
(
    unsigned char	devNum,
    unsigned short	vlanId,
    unsigned long	enable
);
extern unsigned long cpssDxChBrgVlanIgmpSnoopingEnable
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned long enable
);
extern unsigned long cpssDxChBrgVlanIpCntlToCpuSet
(
    unsigned char                               devNum,
    unsigned short                              vlanId,
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType
);

extern unsigned long cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    unsigned char      devNum,
    unsigned short     vlanId,
    unsigned long    enable
);
extern unsigned long cpssDxChBrgVlanIngressMirrorEnable
(
     unsigned char    devNum,
     unsigned short   vlanId,
     unsigned long  enable
);

extern unsigned long cpssDxChBrgVlanMruProfileValueSet
(
    unsigned char     devNum,
    unsigned int    mruIndex,
    unsigned int mruValue
);
extern unsigned int cpssDxChBrgFdbPortLearnStatusSet
(
    unsigned char                     devNum,
    unsigned char                     port,
    unsigned int                      status,
    unsigned int                      cmd
);

/*******************************************************************************
* cpssDxChIpMtuProfileSet
*
* DESCRIPTION:
*      Sets the global MTU profile limit values.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mtuProfileIndex - the mtu profile index out of the possible 8 (0..7).
*       mtu             - the maximum transmission unit
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChIpMtuProfileSet
(
    unsigned char  devNum,
    unsigned long  mtuProfileIndex,
    unsigned int mtu
);
#endif
/****************************************************
 *
 *		====NAM ASIC Part Function Declearations=====
 *
 *
 *
 ***************************************************/
unsigned int nam_asic_vlan_entry_active
(
	unsigned int product_id,
	unsigned short vlanId
);

unsigned int nam_asic_vlan_entry_ports_add
(
	unsigned char 	devNum,
	unsigned short 	vlanId,
	unsigned char 	portNum,
	unsigned char 	isTagged
); 

unsigned  int nam_asic_vlan_entry_ports_del
(
	unsigned short vlanId,
	unsigned char  devNum,
	unsigned char  portNum,
	unsigned char  needJoin
);
;
unsigned int nam_asic_vlan_entry_trunk_mbr_add
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int	trunkMbrBmp0,
	unsigned int	trunkMbrBmp1,
	unsigned char isTagged
) ;
unsigned int nam_asic_vlan_entry_trunk_mbr_del
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned int	trunkMbrBmp0,
	unsigned int	trunkMbrBmp1,
	unsigned char isTagged
); 
unsigned int nam_asic_set_port_vlan_ingresflt
(	
	unsigned char devNum,
	unsigned char portNum,
	unsigned char enDis
);
unsigned int nam_asic_set_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short pvid
);
unsigned int nam_asic_get_port_pvid
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short* pvid
);
unsigned int nam_asic_vlan_entry_delete
(
	unsigned short vlanId
);
unsigned int nam_vlan_update_vid
(
	unsigned int product_id,
	unsigned short vid_old,
	unsigned short vid_new
);
unsigned int nam_asic_vlan_port_route_en
(
	unsigned char devNum,
	unsigned char portNum
);
unsigned int nam_asic_vlan_port_route_dis
(
	unsigned char devNum,
	unsigned char portNum
);
unsigned int nam_asic_port_l3intf_vlan_entry_set
(
	unsigned int product_id,
	unsigned short vlanId
);
unsigned int nam_asic_vlan_get_port_members_bmp
(
	unsigned short	vlanId,
	unsigned int	*untagBmp,
	unsigned int	*tagBmp
);
unsigned int nam_asic_vlan_get_port_members
(
	unsigned short	vlanId,
	/*
	unsigned int	*npdMbrBmpWord,
	unsigned int	*npdTagBmpWord
	*/
	unsigned int	*untagmbrIndxArr,
	unsigned int	*tagmbrIndxArr
	
);

unsigned int nam_vlan_na_msg_enable
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  enable
);

/*************************************
 *	====extern CPSS Multicast API==== 
 *
 *************************************/
typedef struct{
    unsigned int   ports[2];
}CPSS_PORTS_BMP_STC;
#ifdef DRV_LIB_CPSS

extern unsigned long cpssDxChBrgVlanIgmpSnoopingEnable
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned long enable
);
extern unsigned long cpssDxChBrgGenIgmpSnoopEnable
(
    unsigned char   devNum,
    unsigned char  portNum,
    unsigned long enable
);

extern unsigned long cpssDxChBrgMcMemberAdd
(
    unsigned char   devNum,
    unsigned short  vidx,
    unsigned char   portNum
);

extern unsigned long cpssDxChBrgMcMemberDelete
(
    unsigned char   devNum,
    unsigned short  vidx,
    unsigned char   portNum
);

extern unsigned long cpssDxChBrgMcGroupDelete
(
    unsigned char   devNum,
    unsigned short  vidx
);

extern unsigned long cpssDxChBrgMcEntryWrite
(
    unsigned char   devNum,
    unsigned short  vidx,
    CPSS_PORTS_BMP_STC   *portBitmapPtr
);

extern unsigned long cpssDxChBrgVlanIpmBridgingEnable
(
    unsigned char   devNum,
    unsigned short  vlanId,
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    unsigned long   enable
);

extern unsigned long cpssDxChBrgVlanIpmBridgingModeSet
(
    unsigned char   devNum,
    unsigned short  vlanId,
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    CPSS_BRG_IPM_MODE_ENT       ipmMode
);


extern unsigned long  cpssDxChIpExceptionCommandSet
(
    unsigned char                            devNum,
    unsigned int								 exceptionType,
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    CPSS_PACKET_CMD_ENT              command
);
#endif
/****************************************************
 *
 *===NAM ASIC Multicast Part Function Declearation===
 *
 ****************************************************/
unsigned int nam_asic_igmp_trap_set_by_vid
(
    unsigned char   devNum,
    unsigned short  vlanId,
    unsigned long 	enable
);

unsigned int nam_asic_igmp_trap_set_by_devport
(
	unsigned short	vid,
    unsigned char   devNum,
    unsigned char  portNum,
    unsigned long 	enable
);
unsigned int nam_asic_vlan_ipv4_mc_enalbe
(
	unsigned char devNum,	
	unsigned short vlanId,	
	unsigned long enable
);

unsigned int nam_asic_vlan_igmp_enable_drop_unmc
(
	unsigned char devNum,
	unsigned short vlanId
);

unsigned int nam_asic_vlan_igmp_enable_forword_unmc
( 
	unsigned char devNum,
	unsigned short vlanId
);

unsigned int nam_asic_vlan_ipv4_mcmode_set
(
	unsigned char devNum,
	unsigned short vlanId
);
unsigned int nam_asic_l2mc_member_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId,
	unsigned char *hasMbr
);
unsigned int nam_asic_l2mc_member_add
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned char portNum,
	unsigned int group_ip,
	unsigned short vlanId
);
unsigned int nam_asic_l2mc_group_del
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   group_ip,
	unsigned short vlanId
);
unsigned int nam_asic_group_mbr_bmp
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int *groupMbrBmp
);
unsigned int nam_asic_group_mbr_bmp_v1
(
	unsigned char devNum,
	unsigned short vidx,
	unsigned int   groupAddr,
	unsigned short vlanId,
	PORT_MEMBER_BMP* groupMbrBmp
);

unsigned int nam_asic_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn
);

int nam_config_vlan_mtu
(
	unsigned char devNum,
	unsigned int mtuIndex,
	unsigned int mtuValue
);

int nam_config_vlan_egress_filter
(
	unsigned char devNum,
	unsigned int isable
);

int nam_config_vlan_filter
(
	unsigned short vlanId,
	unsigned int filterType,
	unsigned int enDis
);

/**************************************************
*
*		Broadcast To CPU rate limiter 
*
****************************************************/

typedef enum
{
    CPSS_DP_GREEN_E = 0,
    CPSS_DP_YELLOW_E,
    CPSS_DP_RED_E
}CPSS_DP_LEVEL_ENT;

typedef enum
{
    CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E,
    CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E
} CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT;


typedef struct
{
    unsigned char		tc;
    CPSS_DP_LEVEL_ENT	dp;
    unsigned int		truncate;
    CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT  cpuRateLimitMode;
    unsigned int				cpuCodeRateLimiterIndex;
    unsigned int				cpuCodeStatRateLimitIndex;
    unsigned int				designatedDevNumIndex;
} CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC;
#ifdef DRV_LIB_CPSS

extern unsigned long cpssDxChNetIfCpuCodeTableSet
(
    unsigned char				devNum,
    CPSS_NET_RX_CPU_CODE_ENT	cpuCode,
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
);

extern unsigned long cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    unsigned char  devNum,
    unsigned int rateLimiterIndex,
    unsigned int windowSize,
    unsigned int pktLimit
);
extern unsigned int cpssDxChBrgVlanEgressFilteringEnable
(
    unsigned char    dev,
    unsigned int  enable
);

extern unsigned int cpssDxChBrgMcEntryRead
(
   unsigned char              devNum,
   unsigned short             vidx,
   CPSS_PORTS_BMP_STC   *portBitmapPtr
);

#endif
int nam_asic_bcast_rate_limiter_config(	unsigned int enDis );
#endif /* _NAM_VLAN_H*/
