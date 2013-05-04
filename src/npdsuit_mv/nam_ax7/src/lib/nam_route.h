#ifndef __NAM_ROUTE_H__
#define __NAM_ROUTE_H__

#define NAM_ROUTE_NEXTHOP_ERR_NOTCONSISTENT		6


#define IN
#define OUT
#define INOUT


#define BUFLENGTH (4096)



typedef unsigned long GT_BOOL;
typedef unsigned long GT_STATUS;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;

#ifdef DRV_LIB_CPSS_3_4
/*
 * Typedef: union GT_IPADDR
 *
 * Description: Defines the IP address
 *
 * Fields:
 *      u32Ip - the IP as single WORD
 *      arIP  - the IP as 4 bytes - NETWORK ORDER
 */
typedef union
{
    GT_U32  u32Ip;
    GT_U8   arIP[4];
}GT_IPADDR;
#else
#define GT_IPADDR unsigned long 
#endif


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

#define ATF_COM		0x02		/* completed entry (ha valid)	*/
#define ATF_PERM	0x04		/* permanent entry		*/
#define ATF_PUBL	0x08		/* publish entry		*/
#define ATF_USETRAILERS	0x10		/* has requested trailers	*/
#define ATF_NETMASK     0x20            /* want to use a netmask (only  for proxy entries) */
#define ATF_DONTPUB	0x40	
#define NPD_ARP_SNOOPING_ERR_NONE		0
#define NPD_ARP_SNOOPING_ERR_NULL_PTR	(NPD_ARP_SNOOPING_ERR_NONE + 1)
#define NPD_ARP_SNOOPING_ERR_DUPLICATED	(NPD_ARP_SNOOPING_ERR_NONE + 2)
#define NPD_ARP_SNOOPING_ERR_NOTEXISTS	(NPD_ARP_SNOOPING_ERR_NONE + 3)
#define NPD_ARP_SNOOPING_ERR_NORESOURCE	(NPD_ARP_SNOOPING_ERR_NONE + 4)
#define NPD_ARP_SNOOPING_ERR_ACTION_TRAP2CPU	(NPD_ARP_SNOOPING_ERR_NONE + 5)
#define NPD_ARP_SNOOPING_ERR_NOTCONSISTENT	(NPD_ARP_SNOOPING_ERR_NONE + 6)
#define NPD_ARP_SNOOPING_ERR_TABLE_FULL	(NPD_ARP_SNOOPING_ERR_NONE + 7)
#define NPD_ARP_SNOOPING_ERR_STATIC_EXIST (NPD_ARP_SNOOPING_ERR_NONE + 8)
#define NPD_ARP_SNOOPING_ERR_STATIC_NOTEXIST	(NPD_ARP_SNOOPING_ERR_NONE + 9)
#define NPD_ARP_SNOOPING_ERR_PORT_NOTMATCH      (NPD_ARP_SNOOPING_ERR_NONE + 10)
#define NPD_ARP_SNOOPING_ERR_KERN_CREATE_FAILED  (NPD_ARP_SNOOPING_ERR_NONE + 11)
#define NPD_ARP_SNOOPING_ERR_ACTION_HARD_DROP  (NPD_ARP_SNOOPING_ERR_NONE + 12)
#define NPD_ARP_SNOOPING_ERR_MAX			(NPD_ARP_SNOOPING_ERR_NONE + 255)

#define NPD_ROUTE_NEXTHOP_ERR_NONE		NPD_ARP_SNOOPING_ERR_NONE
#define NPD_ROUTE_NEXTHOP_ERR_NULL_PTR	NPD_ARP_SNOOPING_ERR_NULL_PTR
#define NPD_ROUTE_NEXTHOP_ERR_DUPLICATED	NPD_ARP_SNOOPING_ERR_DUPLICATED
#define NPD_ROUTE_NEXTHOP_ERR_NOTEXISTS		NPD_ARP_SNOOPING_ERR_NOTEXISTS
#define NPD_ROUTE_NEXTHOP_ERR_NORESOURCE	NPD_ARP_SNOOPING_ERR_NORESOURCE
#define NPD_ROUTE_NEXTHOP_ERR_ACTION_TRAP2CPU	NPD_ARP_SNOOPING_ERR_ACTION_TRAP2CPU
#define NPD_ROUTE_NEXTHOP_ERR_ACTION_HARD_DROP	NPD_ARP_SNOOPING_ERR_ACTION_HARD_DROP
#define NPD_ROUTE_NEXTHOP_ERR_NOTCONSISTENT		NPD_ARP_SNOOPING_ERR_NOTCONSISTENT


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

typedef enum
{
    CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E = 0,
    CPSS_DXCH_IP_PORT_CNT_MODE_E                 = 1,
    CPSS_DXCH_IP_TRUNK_CNT_MODE_E                = 2
}CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT;

typedef enum
{
    CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E = 0,
    CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E       = 1
}CPSS_DXCH_IP_VLAN_CNT_MODE_ENT;
typedef enum
{
    CPSS_IP_PROTOCOL_IPV4_E     = 0,
    CPSS_IP_PROTOCOL_IPV6_E     = 1,
    CPSS_IP_PROTOCOL_IPV4V6_E   = 2
}CPSS_IP_PROTOCOL_STACK_ENT;

typedef struct CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STCT
{
    CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;
    CPSS_IP_PROTOCOL_STACK_ENT           ipMode;
    CPSS_DXCH_IP_VLAN_CNT_MODE_ENT       vlanMode;
    GT_U8                                devNum;
    union
    {
        unsigned char                              port;
        unsigned short                     trunk;
    }portTrunk;
    unsigned short                               vlanId;
}CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC;

typedef enum
{
    CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E    = 0,
    CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E      = 1
}CPSS_DXCH_IP_CNT_SET_MODE_ENT;

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
typedef struct
{
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode;
    unsigned long                 mirrorToRxAnalyzerPort;
} CPSS_DXCH_PCL_ACTION_MIRROR_STC;

typedef struct
{
    char   enableMatchCount;
    unsigned long    matchCounterIndex;
} CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC;


typedef enum
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E = 0,
    CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E,
    CPSS_PACKET_ATTRIBUTE_MODIFY_INVALID_E
} CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT;

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E = 0,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT;


typedef struct
{
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyDscp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT      modifyUp;
    union
    {
        struct
        {
            unsigned long                                      profileIndex;
            unsigned long                                     profileAssignIndex;
            CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT profilePrecedence;
        } ingress;
        struct
        {
            unsigned char                              dscp;
            unsigned char                              up;
        } egress;
    } qos;
} CPSS_DXCH_PCL_ACTION_QOS_STC;

typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
} CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT;
typedef enum {
    CPSS_INTERFACE_PORT_E = 0,
    CPSS_INTERFACE_TRUNK_E,
    CPSS_INTERFACE_VIDX_E,
    CPSS_INTERFACE_VID_E
}CPSS_INTERFACE_TYPE_ENT;

typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        unsigned char   devNum;
        unsigned char   portNum;
    }devPort;

    unsigned short  trunkId;
    unsigned short       vidx;
    unsigned short       vlanId;
#ifdef DRV_LIB_CPSS_3_4
	unsigned char		 devNum;
	unsigned short		 fabricVidx;
	unsigned int		 index;
#endif
}CPSS_INTERFACE_INFO_STC;

typedef struct
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            unsigned long                  vntL2Echo;
            unsigned long                  tunnelStart;
            unsigned long                   tunnelPtr;
        } outIf;
        unsigned long                   routerLttIndex;
    } data;

} CPSS_DXCH_PCL_ACTION_REDIRECT_STC;
typedef struct
{
    unsigned long   policerEnable;
    unsigned long   policerId;

} CPSS_DXCH_PCL_ACTION_POLICER_STC;

typedef enum
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E,
    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E
} CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT;

typedef struct
{
    CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT          modifyVlan;
     unsigned long                                nestedVlan;
    unsigned short                                vlanId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   precedence;

} CPSS_DXCH_PCL_ACTION_VLAN_STC;

typedef struct
{
     unsigned long  doIpUcRoute;
     unsigned long   arpDaIndex;
     unsigned long  decrementTTL;
     unsigned long  bypassTTLCheck;
     unsigned long  icmpRedirectCheck;

} CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC;
#ifdef DRV_LIB_CPSS_3_4
/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC
 *
 * Description:
 *     This structure defines future lookups configuration.
 *
 * Fields:
 *      ipclConfigIndex - index of Pcl Configuration table for next lookup.
 *                     0 means no value to update.
 *      pcl0_1OverrideConfigIndex - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup0_1
 *      pcl1OverrideConfigIndex   - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup1
 *
 *  Comments:
 *
 */
typedef struct
{
    unsigned int                                 ipclConfigIndex;
    unsigned int  pcl0_1OverrideConfigIndex;
    unsigned int  pcl1OverrideConfigIndex;
} CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC;
/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC
 *
 * Description:
 *     This structure defines packet Source Id assignment.
 *
 * Fields:
 *     assignSourceId  - assign Source Id enable:
 *                     - GT_TRUE  - Assign Source Id.
 *                     - GT_FALSE - Don't assign Source Id.
 *     sourceIdValue   - the Source Id value to be assigned
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       -  packet command (forward, mirror-to-cpu,
 *                      hard-drop, soft-drop, or trap-to-cpu)
 *      actionStop   - Action Stop
 *                     GT_TRUE  - to skip the following PCL lookups
 *                     GT_FALSE - to continue with following PCL lookups
 *                     Supported by DxCh3 and DxChXCat above devices.
 *                     DxCh1 and DxCh2 ignores the field.
 *                     Relevant to Policy Action Entry only.
 *      bypassBrigge   - the Bridge engine processed or bypassed
 *                     GT_TRUE  - the Bridge engine is bypassed.
 *                     GT_FALSE - the Bridge engine is processed.
 *      bypassIngressPipe - the ingress pipe bypassed or not.
 *                     GT_TRUE  - the ingress pipe is bypassed.
 *                     GT_FALSE - the ingress pipe is not bypassed.
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      lookupConfig - configuration of IPCL lookups.
 *      mirror       -  packet mirroring configuration
 *      matchCounter -  match counter configuration
 *      qos          -  packet QoS attributes modification configuration
 *      redirect     -  packet Policy redirection configuration
 *      policer      -  packet Policing configuration
 *                      Relevant to Policy Action Entry only.
 *      vlan         -  packet VLAN modification configuration
 *      ipUcRoute    -  special DxCh (not relevant for DxCh2 and above) Ip Unicast Route
 *                      action parameters configuration.
 *                      Relevant to Unicast Route Entry only.
 *      sourceId     -  packet source Id assignment
 *                      Relevant to Policy Action Entry only.
 *
 * Comment:
 */
 typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
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
 #else 
typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
    unsigned long                          egressPolicy;
    CPSS_DXCH_PCL_ACTION_MIRROR_STC        mirror;
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    CPSS_DXCH_PCL_ACTION_QOS_STC           qos;
    CPSS_DXCH_PCL_ACTION_REDIRECT_STC      redirect;
    CPSS_DXCH_PCL_ACTION_POLICER_STC       policer;
    CPSS_DXCH_PCL_ACTION_VLAN_STC          vlan;
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
} CPSS_DXCH_PCL_ACTION_STC;
#endif
typedef union CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNTT
{
	CPSS_DXCH_PCL_ACTION_STC pclIpUcAction;
	CPSS_DXCH_IP_LTT_ENTRY_STC ipLttEntry;
}CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT;

typedef struct CPSS_DXCH_IP_COUNTER_SET_STCT
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
}CPSS_DXCH_IP_COUNTER_SET_STC;

typedef enum
{
    CPSS_IP_CNT_SET0_E   = 0,
    CPSS_IP_CNT_SET1_E   = 1,
    CPSS_IP_CNT_SET2_E   = 2,
    CPSS_IP_CNT_SET3_E   = 3,
    CPSS_IP_CNT_NO_SET_E = 4
}CPSS_IP_CNT_SET_ENT;

typedef union
{
    GT_U32  u32Ip;
    GT_U8   arIP[4];
}_GT_IPADDR;



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

#define BCM_EGRESS_IDX_MIN   (0x186A0) /* Egress object start index    */

#define NAM_L3_L2ONLY           (1 << 0)   /* L2 switch only on interface. */
#define NAM_L3_UNTAG            (1 << 1)   /* Packet goes out untagged. */
#define NAM_L3_S_HIT            (1 << 2)   /* Source IP address match. */
#define NAM_L3_D_HIT            (1 << 3)   /* Destination IP address match. */
#define NAM_L3_HIT              (NAM_L3_S_HIT | NAM_L3_D_HIT) 
#define NAM_L3_HIT_CLEAR        (1 << 4)   /* Clear HIT bit. */
#define NAM_L3_ADD_TO_ARL       (1 << 5)   /* Add interface address MAC to ARL. */
#define NAM_L3_WITH_ID          (1 << 6)   /* ID is provided. */
#define NAM_L3_NEGATE           (1 << 7)   /* Negate a set. */
#define NAM_L3_REPLACE          (1 << 8)   /* Replace existing entry. */
#define NAM_L3_TGID             (1 << 9)   /* Port belongs to trunk. */
#define NAM_L3_RPE              (1 << 10)  /* Pick up new priority (COS). */
#define NAM_L3_IPMC             (1 << 11)  /* Set IPMC for real IPMC entry. */
#define NAM_L3_L2TOCPU          (1 << 12)  /* Packet to CPU unrouted, XGS12: Set*/

#define NAM_L3_DEFIP_CPU        (1 << 13)  /* Strata: DEFIP CPU bit set. */
#define NAM_L3_DEFIP_LOCAL      NAM_L3_DEFIP_CPU /* XGS: Local DEFIP route. */
#define NAM_L3_MULTIPATH        (1 << 14)  /* Specify ECMP treatment. */
#define NAM_L3_LPM_DEFROUTE     (1 << 15)  /* Default route in LPM. */
#define NAM_L3_HOST_AS_ROUTE    (1 << 16)  /* Use LPM if host table full. */
#define NAM_L3_IP6              (1 << 17)  /* IPv6. */
#define NAM_L3_RPF              (1 << 18)  /* RPF check. */
#define NAM_L3_SRC_DISCARD      (1 << 19)  /* Source Match Discard. */
#define NAM_L3_DST_DISCARD      (1 << 20)  /* Destination match discard. */
#define NAM_L3_SECONDARY        (1 << 21)  /* Secondary L3 interface. */
#define NAM_L3_ROUTE_LABEL      (1 << 22)  /* Indicates that MPLS label in route
                                              entry is valid. */
#define MACADDR_STR_LEN 18              /* Formatted MAC address */
#define IPADDR_STR_LEN  16              /* Formatted IP address */
#define IP6ADDR_STR_LEN 64              /* Formatted IP address */
/* Reserved VRF values . */
#define NAM_L3_VRF_OVERRIDE     -1 /* Matches before VRF specific
                                              entries. */
#define NAM_L3_VRF_GLOBAL       -2/* Matches after VRF specific
                                              entries. */
#define NAM_L3_VRF_DEFAULT      0 /* Default VRF ID. */

typedef unsigned char ip6_addr[16];
typedef unsigned char eth_mac_t[6];

typedef struct route_drv_info
{
     unsigned int dip;
	 unsigned int nexthop;
	 unsigned int masklen;
     unsigned int baseindex;/*for marvell platform,this is the next hop route index*/
     unsigned int ifindex;/*/tmp value for broadcom platform*/
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

typedef struct {
     host_infos info[8192];
	 unsigned int size;
	 unsigned int count;
}host_datas;

typedef struct {
     drv_infos info[8192];
	 unsigned int size;
	 unsigned int count;
}rt_datas;

#ifdef DRV_LIB_CPSS
extern GT_STATUS 	cpssDxChIpLpmIpv4UcPrefixAdd	
(
    IN unsigned int                                   lpmDBId,
    IN unsigned int                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN unsigned int                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
);

extern GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN unsigned int                                 lpmDBId,
    IN unsigned int                                 vrId
);

extern GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  unsigned int                                  lpmDBId,
    IN  unsigned int                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  unsigned int                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr
);
extern GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch_tmp
(
	IN	unsigned int								  lpmDBId,
	IN	unsigned int								  vrId,
	IN	GT_IPADDR								ipAddr,
	IN	unsigned int								  prefixLen,
	OUT unsigned int									  *internalRuleIdx,
	OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT	*nextHopInfoPtr
);

extern GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    unsigned int                                    lpmDBId,
    IN    unsigned int                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT unsigned int                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr
);

extern GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
	IN unsigned int 						lpmDBId,
	IN unsigned int 						vrId,
	IN GT_IPADDR							ipAddr,
	IN unsigned int 						prefixLen
);

extern unsigned int npd_route_nexthop_tblindex_get
(
	unsigned int ifIndex,
	unsigned int ipAddr,
	unsigned int *tblIndex
);
extern GT_STATUS cpssDxChIpv4PrefixGet
(
	IN	GT_U8		 devNum,
	IN	unsigned int routerTunnelTermTcamEntryIndex,
	OUT GT_BOOL 	 *validPtr,
	OUT GT_IPADDR	 *ipAddrPtr,
	OUT GT_IPADDR	 *maskPtr,
	OUT GT_U32		 *mcGroupIndexPtr
);


extern GT_STATUS cpssDxChIpCntGet
(
    IN  GT_U8                        devNum,
    IN  CPSS_IP_CNT_SET_ENT          cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC *counters
);
extern GT_STATUS cpssDxChIpCntSetModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_IP_CNT_SET_ENT                        cntSet,
    IN  CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode,
    IN  CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceModeCfgPtr
);
#endif

extern unsigned int get_NextHop_TBL_Entry(unsigned int ifIndex,unsigned int nextHopIp);



#endif

