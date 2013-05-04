/*************************************************************************
* wraplCpssDxChNet.c
*
* DESCRIPTION:
*       A lua wrapper for netIf functions
*           CPSS_DXCH_NET_TX_PARAMS_STC
*            CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
*            CPSS_DXCH_NET_DSA_PARAMS_STC
*             CPSS_DXCH_NET_DSA_COMMON_STC
*             CPSS_DXCH_NET_DSA_TO_CPU_STC
*             CPSS_DXCH_NET_DSA_FROM_CPU_STC
*             CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
*             CPSS_DXCH_NET_DSA_FORWARD_STC
*           
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

int check_wraplCpssDxChNet()
{
    return 1;
}

/* externs */
void prv_lua_to_c_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);
void prv_c_to_lua_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);


enumDescr enumDescr_CPSS_DXCH_NET_DSA_TYPE_ENT[] = {
    { CPSS_DXCH_NET_DSA_TYPE_REGULAR_E, "regular" }, 
    { CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E, "extended" },
    { 0, NULL }
};

enumDescr enumDescr_CPSS_NET_RX_CPU_CODE_ENT[] = {
    /* L2 Bridge */
    ENUM_ENTRY(CPSS_NET_CONTROL_E),
    ENUM_ENTRY(CPSS_NET_UN_KNOWN_UC_E),
    ENUM_ENTRY(CPSS_NET_UN_REGISTERD_MC_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_BPDU_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_DEST_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_SRC_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CONTROL_MAC_RANGE_TRAP_E),
    /* Pre-egress engine */
    ENUM_ENTRY(CPSS_NET_RX_SNIFFER_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_ARP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_IGMP_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_SA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_DA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_SA_DA_E),
    ENUM_ENTRY(CPSS_NET_INTERVENTION_PORT_LOCK_E),
    ENUM_ENTRY(CPSS_NET_EXP_TNL_BAD_IPV4_HDR_E),
    ENUM_ENTRY(CPSS_NET_EXP_TNL_BAD_VLAN_E),
    /* MLL */
    ENUM_ENTRY(CPSS_NET_MLL_RPF_TRAP_E),
    /* Pre-router */
    ENUM_ENTRY(CPSS_NET_RESERVED_SIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERNAL_SIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_SIP_SPOOF_TRAP_E),
    /* PCL */
    ENUM_ENTRY(CPSS_NET_DEF_KEY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IP_CLASS_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CLASS_KEY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_TCP_RST_FIN_TRAP_E),
    ENUM_ENTRY(CPSS_NET_CLASS_KEY_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_TRAP_BY_DEFAULT_ENTRY0_E),
    ENUM_ENTRY(CPSS_NET_TRAP_BY_DEFAULT_ENTRY1_E),
    ENUM_ENTRY(CPSS_NET_TRAP_UD_INVALID_E),
    /* IPv4 */
    ENUM_ENTRY(CPSS_NET_RESERVED_DIP_TRAP_E),
    ENUM_ENTRY(CPSS_NET_MC_BOUNDARY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_INTERNAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_IP_ZERO_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_BAD_IP_HDR_CHECKSUM_E),
    ENUM_ENTRY(CPSS_NET_RPF_CHECK_FAILED_E),
    ENUM_ENTRY(CPSS_NET_OPTIONS_IN_IP_HDR_E),
    ENUM_ENTRY(CPSS_NET_END_OF_IP_TUNNEL_E),
    ENUM_ENTRY(CPSS_NET_BAD_TUNNEL_HDR_E),
    ENUM_ENTRY(CPSS_NET_IP_HDR_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ROUTE_ENTRY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_DIP_CHECK_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ILLEGAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_POLICY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_DEFAULT_ROUTE_TRAP_E),
    /*MTU Exceeded */
    ENUM_ENTRY(CPSS_NET_IP_MTU_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_MPLS_MTU_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_CLASS_MTU_EXCEED_E),
    /*Mpls */
    ENUM_ENTRY(CPSS_NET_MPLS_ZERO_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_NHLFE_ENTRY_TRAP_E),
    ENUM_ENTRY(CPSS_NET_ILLEGAL_POP_E),
    ENUM_ENTRY(CPSS_NET_INVALID_MPLS_IF_E),

    /* Start of additional Values for Tiger use only */
    /* Tiger IPv6 codes */
    ENUM_ENTRY(CPSS_NET_IPV6_ICMP_MLD_E),
    ENUM_ENTRY(CPSS_NET_IPV6_IGMP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_PCL_E),
    ENUM_ENTRY(CPSS_NET_IPV6_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_EXT_HEADER_E),
    ENUM_ENTRY(CPSS_NET_IPV6_BAD_HEADER_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ILLIGAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_DIP_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTE_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTER_RPF_E),
    ENUM_ENTRY(CPSS_NET_IPV6_SCOP_FAIL_E),
    ENUM_ENTRY(CPSS_NET_IPV6_TTL_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_DEFAULT_ROUTE_E),
    /* Tiger Mirror codes */
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_RST_FIN_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_PCE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_BRIDGE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MLD_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IGMP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_RIPV1_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_TTL_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_RPF_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_OPTIONS_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ROUTE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ICMP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_HOP_BY_HOP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_EXT_HEADER_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_ILLIGAL_IP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_SPECIAL_DIP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_SCOP_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IP_MTU_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_INLIF_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_PCL_MTU_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_DEFAULT_ROUTE_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE1_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE2_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_MC_ROUTE3_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_BC_E),
    /*End of additional Values for Tiger use only */

    /*Egress pipe */
    ENUM_ENTRY(CPSS_NET_ETH_BRIDGED_LLT_E),
    ENUM_ENTRY(CPSS_NET_IPV4_ROUTED_LLT_E),
    ENUM_ENTRY(CPSS_NET_UC_MPLS_LLT_E),
    ENUM_ENTRY(CPSS_NET_MC_MPLS_LLT_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ROUTED_LLT_E),
    ENUM_ENTRY(CPSS_NET_L2CE_LLT_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_MIRROR_TO_CPU_E),

    /* addition values for DXSal devices use only */
    ENUM_ENTRY(CPSS_NET_UNKNOWN_UC_E),
    ENUM_ENTRY(CPSS_NET_UNREG_MC_E),

    /* additional values for DXSal,DXCH and EXMXPM devices */
    ENUM_ENTRY(CPSS_NET_LOCK_PORT_MIRROR_E),

    /* addition values for DXCH and EXMXPM devices */
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E),
    ENUM_ENTRY(CPSS_NET_IPV6_ICMP_PACKET_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E),
    ENUM_ENTRY(CPSS_NET_IPV4_BROADCAST_PACKET_E),
    ENUM_ENTRY(CPSS_NET_NON_IPV4_BROADCAST_PACKET_E),
    ENUM_ENTRY(CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E),
    ENUM_ENTRY(CPSS_NET_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E),
    ENUM_ENTRY(CPSS_NET_UNKNOWN_UNICAST_E),

    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP0_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP1_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP2_E),
    ENUM_ENTRY(CPSS_NET_UDP_BC_MIRROR_TRAP3_E),

    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E),
    ENUM_ENTRY(CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E),

    ENUM_ENTRY(CPSS_NET_BRIDGED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E),
    ENUM_ENTRY(CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E),
    ENUM_ENTRY(CPSS_NET_CPU_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_EGRESS_SAMPLED_E),
    ENUM_ENTRY(CPSS_NET_INGRESS_SAMPLED_E),
    ENUM_ENTRY(CPSS_NET_INVALID_PCL_KEY_TRAP_E),

    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E),
    ENUM_ENTRY(CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E),
    ENUM_ENTRY(CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E),
    ENUM_ENTRY(CPSS_NET_ROUTED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_IP_DIP_DA_MISMATCH_E),
    ENUM_ENTRY(CPSS_NET_IP_UC_SIP_SA_MISMATCH_E),

    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_UC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE0_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_MC_ROUTE3_TRAP_E),

    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_UC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE0_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE1_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE2_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV6_MC_ROUTE3_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IP_UC_RPF_FAIL_E),
    ENUM_ENTRY(CPSS_NET_ARP_BC_TO_ME_E),

    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E),
    ENUM_ENTRY(CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E),
    ENUM_ENTRY(CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_HEADER_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E),
    ENUM_ENTRY(CPSS_NET_ARP_REPLY_TO_ME_E),
    ENUM_ENTRY(CPSS_NET_CPU_TO_ALL_CPUS_E),
    ENUM_ENTRY(CPSS_NET_TCP_SYN_TO_CPU_E),

    ENUM_ENTRY(CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_MC_ROUTED_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_L2VPN_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_MPLS_PACKET_FORWARD_E),
    ENUM_ENTRY(CPSS_NET_ARP_BC_EGRESS_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_BC_EGRESS_FILTER_E),
    ENUM_ENTRY(CPSS_NET_MC_CPU_TO_CPU_E),
    ENUM_ENTRY(CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E),
    ENUM_ENTRY(CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E),
    ENUM_ENTRY(CPSS_NET_TT_MPLS_HEADER_CHECK_E),
    ENUM_ENTRY(CPSS_NET_TT_MPLS_TTL_EXCEED_E),
    ENUM_ENTRY(CPSS_NET_TTI_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_MPLS_SRC_FILTERING_E),
    ENUM_ENTRY(CPSS_NET_IPV4_TTL1_EXCEEDED_E),
    ENUM_ENTRY(CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E),

    ENUM_ENTRY(CPSS_NET_OAM_PDU_TRAP_E),
    ENUM_ENTRY(CPSS_NET_IPCL_MIRROR_E),
    ENUM_ENTRY(CPSS_NET_MPLS_TTL1_EXCEEDED_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_1_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_2_E),
    ENUM_ENTRY(CPSS_NET_MPLS_ROUTE_ENTRY_3_E),


    ENUM_ENTRY(CPSS_NET_FIRST_USER_DEFINED_E),
    ENUM_ENTRY(CPSS_NET_LAST_USER_DEFINED_E),

    ENUM_ENTRY(CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E),
    ENUM_ENTRY(CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E),

    ENUM_ENTRY(CPSS_NET_ALL_CPU_OPCODES_E),
    { 0, NULL }
};
    
enumDescr enumDescr_CPSS_DP_LEVEL_ENT[] = {
    ENUM_ENTRY(CPSS_DP_GREEN_E),
    ENUM_ENTRY(CPSS_DP_YELLOW_E),
    ENUM_ENTRY(CPSS_DP_RED_E),
    { 0, NULL }
};

enumDescr enumDescr_CPSS_DXCH_NET_DSA_CMD_ENT[] = {
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_TO_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E),
    ENUM_ENTRY(CPSS_DXCH_NET_DSA_CMD_FORWARD_E),
    { 0, NULL }
};

/*******************************************/
/* CPSS_DXCH_NET_SDMA_TX_PARAMS_STC        */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC *params
)
{
    F_BOOL(params, -1, recalcCrc);
    F_NUMBER(params, -1, txQueue, GT_U8);
    F_NUMBER(params, -1, evReqHndl, GT_U32);
    F_BOOL(params, -1, invokeTxBufferQueueEvent);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC
*
* Description:
*       Convert CPSS_DXCH_NET_SDMA_TX_PARAMS_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_SDMA_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC *params
)
{
    lua_newtable(L);
    FO_BOOL(params, -1, recalcCrc);
    FO_NUMBER(params, -1, txQueue, GT_U8);
    FO_NUMBER(params, -1, evReqHndl, GT_U32);
    FO_BOOL(params, -1, invokeTxBufferQueueEvent);
}

add_mgm_type(CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_COMMON_STC            */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_COMMON_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_COMMON_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_COMMON_STC *val
)
{
    F_ENUM(val, -1, dsaTagType, CPSS_DXCH_NET_DSA_TYPE_ENT);
    F_NUMBER(val, -1, vpt, GT_U8);
    F_NUMBER(val, -1, cfiBit, GT_U8);
    F_NUMBER(val, -1, vid, GT_U16);
    F_BOOL(val, -1, dropOnSource);
    F_BOOL(val, -1, packetIsLooped);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_COMMON_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_COMMON_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_COMMON_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_COMMON_STC *val
)
{
    lua_newtable(L);
    FO_ENUM(val, -1, dsaTagType, CPSS_DXCH_NET_DSA_TYPE_ENT);
    FO_NUMBER(val, -1, vpt, GT_U8);
    FO_NUMBER(val, -1, cfiBit, GT_U8);
    FO_NUMBER(val, -1, vid, GT_U16);
    FO_BOOL(val, -1, dropOnSource);
    FO_BOOL(val, -1, packetIsLooped);
}

add_mgm_type(CPSS_DXCH_NET_DSA_COMMON_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_TO_CPU_STC            */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_TO_CPU_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_TO_CPU_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_TO_CPU_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_TO_CPU_STC *val
)
{
    F_BOOL(val, -1, isEgressPipe);
    F_BOOL(val, -1, isTagged);
    F_NUMBER(val, -1, devNum, GT_U8);
    F_BOOL(val, -1, srcIsTrunk);
    lua_getfield(L, -1, "interface");
    if (lua_istable(L, -1))
    {
        if (val->srcIsTrunk == GT_TRUE)
        {
            F_NUMBER(&(val->interface), -1, srcTrunkId, GT_TRUNK_ID);
        }
        else
        {
            F_NUMBER(&(val->interface), -1, portNum, GT_U8);
        }
    }
    lua_pop(L, -1);
    F_ENUM(val, -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    F_BOOL(val, -1, wasTruncated);
    F_NUMBER(val, -1, originByteCount, GT_U32);
    F_NUMBER(val, -1, timestamp, GT_U32);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_TO_CPU_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_TO_CPU_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_TO_CPU_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_TO_CPU_STC *val
)
{
    lua_newtable(L);
    FO_BOOL(val, -1, isEgressPipe);
    FO_BOOL(val, -1, isTagged);
    FO_NUMBER(val, -1, devNum, GT_U8);
    FO_BOOL(val, -1, srcIsTrunk);
    lua_newtable(L);
    if (val->srcIsTrunk == GT_TRUE)
    {
        FO_NUMBER(&(val->interface), -1, srcTrunkId, GT_TRUNK_ID);
    }
    else
    {
        FO_NUMBER(&(val->interface), -1, portNum, GT_U8);
    }
    lua_setfield(L, -2, "interface");
    FO_ENUM(val, -1, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    FO_BOOL(val, -1, wasTruncated);
    FO_NUMBER(val, -1, originByteCount, GT_U32);
    FO_NUMBER(val, -1, timestamp, GT_U32);
}

add_mgm_type(CPSS_DXCH_NET_DSA_TO_CPU_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_FROM_CPU_STC          */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_FROM_CPU_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_FROM_CPU_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_FROM_CPU_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_FROM_CPU_STC *val
)
{
    F_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
    F_NUMBER(val, -1, tc, GT_U8);
    F_ENUM(val, -1, dp, CPSS_DP_LEVEL_ENT);
    F_BOOL(val, -1, egrFilterEn);
    F_BOOL(val, -1, cascadeControl);
    F_BOOL(val, -1, egrFilterRegistered);
    F_NUMBER(val, -1, srcId, GT_U32);
    F_NUMBER(val, -1, srcDev, GT_U8);
    lua_getfield(L, -1, "extDestInfo");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        if (val->dstInterface.type == CPSS_INTERFACE_VIDX_E
            || val->dstInterface.type == CPSS_INTERFACE_VID_E)
        {
            /* multiDest */
            lua_getfield(L, -1, "multiDest");
            if (lua_type(L, -1) == LUA_TTABLE)
            {
                F_BOOL(&(val->extDestInfo.multiDest), -1, excludeInterface);
                F_STRUCT(&(val->extDestInfo.multiDest), -1, excludedInterface, CPSS_INTERFACE_INFO_STC);
                F_BOOL(&(val->extDestInfo.multiDest), -1, mirrorToAllCPUs);
            }
            lua_pop(L, 1);
        }
        if (val->dstInterface.type == CPSS_INTERFACE_PORT_E)
        {
            /* devPort */
            lua_getfield(L, -1, "devPort");
            if (lua_type(L, -1) == LUA_TTABLE)
            {
                F_BOOL(&(val->extDestInfo.devPort), -1, dstIsTagged);
                F_BOOL(&(val->extDestInfo.devPort), -1, mailBoxToNeighborCPU);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_FROM_CPU_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_FROM_CPU_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_FROM_CPU_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_FROM_CPU_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, dstInterface, CPSS_INTERFACE_INFO_STC);
    FO_NUMBER(val, t, tc, GT_U8);
    FO_ENUM(val, t, dp, CPSS_DP_LEVEL_ENT);
    FO_BOOL(val, t, egrFilterEn);
    FO_BOOL(val, t, cascadeControl);
    FO_BOOL(val, t, egrFilterRegistered);
    FO_NUMBER(val, t, srcId, GT_U32);
    FO_NUMBER(val, t, srcDev, GT_U8);
    lua_newtable(L);
        if (val->dstInterface.type == CPSS_INTERFACE_VIDX_E
            || val->dstInterface.type == CPSS_INTERFACE_VID_E)
        {
            /* multiDest */
            lua_newtable(L);
                FO_BOOL(&(val->extDestInfo.multiDest), t+2, excludeInterface);
                FO_STRUCT(&(val->extDestInfo.multiDest), t+2, excludedInterface, CPSS_INTERFACE_INFO_STC);
                FO_BOOL(&(val->extDestInfo.multiDest), t+2, mirrorToAllCPUs);
            lua_setfield(L, t+1, "multiDest");
        }
        if (val->dstInterface.type == CPSS_INTERFACE_PORT_E)
        {
            /* devPort */
            lua_newtable(L);
                FO_BOOL(&(val->extDestInfo.devPort), t+2, dstIsTagged);
                FO_BOOL(&(val->extDestInfo.devPort), t+2, mailBoxToNeighborCPU);
            lua_setfield(L, t+1, "devPort");
        }
    lua_setfield(L, t, "extDestInfo");
}

add_mgm_type(CPSS_DXCH_NET_DSA_FROM_CPU_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_TO_ANALYZER_STC       */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC *val
)
{
    F_BOOL(val, -1, rxSniffer);
    F_BOOL(val, -1, isTagged);
    lua_getfield(L, -1, "devPort");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        F_NUMBER(&(val->devPort), -1, devNum, GT_U8);
        F_NUMBER(&(val->devPort), -1, portNum, GT_U8);
    }
    lua_pop(L, 1);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_TO_ANALYZER_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, rxSniffer);
    FO_BOOL(val, t, isTagged);
    lua_newtable(L);
        FO_NUMBER(&(val->devPort), t+1, devNum, GT_U8);
        FO_NUMBER(&(val->devPort), t+1, portNum, GT_U8);
    lua_setfield(L, t, "devPort");
}

add_mgm_type(CPSS_DXCH_NET_DSA_TO_ANALYZER_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_FORWARD_STC           */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_FORWARD_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_FORWARD_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_FORWARD_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_FORWARD_STC *val
)
{
    F_BOOL(val, -1, srcIsTagged);
    F_NUMBER(val, -1, srcDev, GT_U8);
    F_BOOL(val, -1, srcIsTrunk);
    lua_getfield(L, -1, "source");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        if (val->srcIsTrunk == GT_TRUE)
        {
            F_NUMBER(&(val->source), -1, trunkId, GT_TRUNK_ID);
        }
        else
        {
            F_NUMBER(&(val->source), -1, portNum, GT_U8);
        }
    }
    lua_pop(L, 1);
    F_NUMBER(val, -1, srcId, GT_U32);
    F_BOOL(val, -1, egrFilterRegistered);
    F_BOOL(val, -1, wasRouted);
    F_NUMBER(val, -1, qosProfileIndex, GT_U32);
    F_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_FORWARD_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_FORWARD_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_FORWARD_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_FORWARD_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);

    FO_BOOL(val, t, srcIsTagged);
    FO_NUMBER(val, t, srcDev, GT_U8);
    FO_BOOL(val, t, srcIsTrunk);
    lua_newtable(L);
        if (val->srcIsTrunk == GT_TRUE)
        {
            FO_NUMBER(&(val->source), t+1, trunkId, GT_TRUNK_ID);
        }
        else
        {
            FO_NUMBER(&(val->source), t+1, portNum, GT_U8);
        }
    lua_setfield(L, t, "source");
    FO_NUMBER(val, t, srcId, GT_U32);
    FO_BOOL(val, t, egrFilterRegistered);
    FO_BOOL(val, t, wasRouted);
    FO_NUMBER(val, t, qosProfileIndex, GT_U32);
    FO_STRUCT(val, t, dstInterface, CPSS_INTERFACE_INFO_STC);
}

add_mgm_type(CPSS_DXCH_NET_DSA_FORWARD_STC);


/*******************************************/
/* CPSS_DXCH_NET_DSA_PARAMS_STC            */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_DSA_PARAMS_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_DSA_PARAMS_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_DSA_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_PARAMS_STC *val
)
{
    F_STRUCT(val, -1, commonParams, CPSS_DXCH_NET_DSA_COMMON_STC);
    F_ENUM(val, -1, dsaType, CPSS_DXCH_NET_DSA_CMD_ENT);
    lua_getfield(L, -1, "dsaInfo");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
        {
            F_STRUCT(&(val->dsaInfo), -1, toCpu, CPSS_DXCH_NET_DSA_TO_CPU_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E)
        {
            F_STRUCT(&(val->dsaInfo), -1, fromCpu, CPSS_DXCH_NET_DSA_FROM_CPU_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E)
        {
            F_STRUCT(&(val->dsaInfo), -1, toAnalyzer, CPSS_DXCH_NET_DSA_TO_ANALYZER_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_FORWARD_E)
        {
            F_STRUCT(&(val->dsaInfo), -1, forward, CPSS_DXCH_NET_DSA_FORWARD_STC);
        }
    }
    lua_pop(L, 1);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_DSA_PARAMS_STC
*
* Description:
*       Convert CPSS_DXCH_NET_DSA_PARAMS_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_DSA_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_DSA_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, commonParams, CPSS_DXCH_NET_DSA_COMMON_STC);
    FO_ENUM(val, t, dsaType, CPSS_DXCH_NET_DSA_CMD_ENT);
    lua_newtable(L);
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
        {
            FO_STRUCT(&(val->dsaInfo), t+1, toCpu, CPSS_DXCH_NET_DSA_TO_CPU_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E)
        {
            FO_STRUCT(&(val->dsaInfo), t+1, fromCpu, CPSS_DXCH_NET_DSA_FROM_CPU_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E)
        {
            FO_STRUCT(&(val->dsaInfo), t+1, toAnalyzer, CPSS_DXCH_NET_DSA_TO_ANALYZER_STC);
        }
        if (val->dsaType == CPSS_DXCH_NET_DSA_CMD_FORWARD_E)
        {
            FO_STRUCT(&(val->dsaInfo), t+1, forward, CPSS_DXCH_NET_DSA_FORWARD_STC);
        }
    lua_setfield(L, t, "dsaInfo");
}

add_mgm_type(CPSS_DXCH_NET_DSA_PARAMS_STC);


/*******************************************/
/* CPSS_DXCH_NET_TX_PARAMS_STC             */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_NET_TX_PARAMS_STC
*
* Description:
*       Convert to CPSS_DXCH_NET_TX_PARAMS_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_NET_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_TX_PARAMS_STC *val
)
{
    F_BOOL(val, -1, packetIsTagged);
    F_PTR(val, -1, cookie, GT_PTR);
    F_STRUCT(val, -1, sdmaInfo, CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);
    F_STRUCT(val, -1, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_NET_TX_PARAMS_STC
*
* Description:
*       Convert CPSS_DXCH_NET_TX_PARAMS_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_NET_TX_PARAMS_STC(
    lua_State *L,
    CPSS_DXCH_NET_TX_PARAMS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, packetIsTagged);
    FO_PTR(val, t, cookie, GT_PTR);
    FO_STRUCT(val, t, sdmaInfo, CPSS_DXCH_NET_SDMA_TX_PARAMS_STC);
    FO_STRUCT(val, t, dsaParam, CPSS_DXCH_NET_DSA_PARAMS_STC);
}

add_mgm_type(CPSS_DXCH_NET_TX_PARAMS_STC);
