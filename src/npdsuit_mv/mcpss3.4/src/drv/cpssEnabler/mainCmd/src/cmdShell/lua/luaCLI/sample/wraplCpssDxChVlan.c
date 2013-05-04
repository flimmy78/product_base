#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>

#include <lua.h>

/* DxCh max number of entries to be read from tables\classes */
#define  CPSS_DXCH_VLAN_MAX_ENTRY_CNS 4096

int check_wraplCpssDxChVlan()
{
    printf("File wraplCpssDxChVlan.c inside");
    return 1;
}


int wrlCpssDxChVlanExists(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_STATUS                       rc;

    rc = cpssDxChBrgVlanEntryRead(
            (GT_U8)lua_tonumber(L, 1) /*devNum*/,
            (GT_U16)lua_tonumber(L, 2) /*vlanId*/,
            &portsMembersPtr,
            &portsTaggingPtr,
            &vlanInfoPtr,
            &isValidPtr,
            &portsTaggingCmd);

    lua_pushboolean(L, (rc == GT_OK && isValidPtr != GT_FALSE) ? 1 : 0);
    return 1;
}

int wrlCpssDxChVlanList(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    int                             cnt = 0;
    GT_U16                          vlanId; 
    GT_STATUS                       rc;

    lua_settop(L, 1);
    lua_newtable(L);
    for (vlanId = 0; vlanId < CPSS_DXCH_VLAN_MAX_ENTRY_CNS; vlanId++)
    {
        rc = cpssDxChBrgVlanEntryRead(
                (GT_U8)lua_tonumber(L, 1) /*devNum*/,
                vlanId,
                &portsMembersPtr,
                &portsTaggingPtr,
                &vlanInfoPtr,
                &isValidPtr,
                &portsTaggingCmd);
        if (rc == GT_OK && isValidPtr != GT_FALSE)
        {
            lua_pushnumber(L, vlanId);
            lua_rawseti(L, 2, ++cnt);
        }
    }
    return 1;
}

int wrlCpssDxChGetVlanInfo(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_STATUS                       rc;
    GT_U8                           devNum = (GT_U8)lua_tonumber(L, 1);
    GT_U16                          vlanId = (GT_U16)lua_tonumber(L, 2);
    int                             port;

    lua_settop(L, 2);
    rc = cpssDxChBrgVlanEntryRead(
            devNum,
            vlanId,
            &portsMembersPtr,
            &portsTaggingPtr,
            &vlanInfoPtr,
            &isValidPtr,
            &portsTaggingCmd);
    if (rc != GT_OK || isValidPtr == GT_FALSE)
    {
        return 0;
    }

    lua_newtable(L); /* 3 */

    lua_newtable(L); /* 4: portsMembers */
    for (port = 0; port < 64; port++)
    {
        if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsMembersPtr, port))
        {
            lua_pushnumber(L, port);
            lua_rawseti(L, 4, lua_objlen(L, 4)+1);
        }
    }
    lua_setfield(L, 3, "portsMembers");

    lua_newtable(L); /* 4: portsTagging */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported == GT_TRUE)
    {
        lua_newtable(L); /* 5: portsTaggingCmd */
        for (port = 0; port < 64; port++)
        {
            if (portsTaggingCmd.portsCmd[port] != CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E)
            {
                lua_pushnumber(L, port);
                lua_rawseti(L, 4, lua_objlen(L, 4)+1);
                lua_pushnumber(L, portsTaggingCmd.portsCmd[port]);
                lua_rawseti(L, 5, port);
            }
        }
        lua_setfield(L, 3, "portsTaggingCmd");
    }
    else
    {
        for (port = 0; port < 64; port++)
        {
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsTaggingPtr, port))
            {
                lua_pushnumber(L, port);
                lua_rawseti(L, 4, lua_objlen(L, 4)+1);
            }
        }
    }
    lua_setfield(L, 3, "portsTagging");

#define F_BOOL(field) \
    lua_pushboolean(L, vlanInfoPtr.field); \
    lua_setfield(L, 3, #field);

/* fieldtype in F_NUMBER is ignored,
 * fields will be shared with wrlCpssDxChGetVlanEntrySet */
#define F_NUMBER(field, fieldtype) \
    lua_pushnumber(L, vlanInfoPtr.field); \
    lua_setfield(L, 3, #field);

    F_BOOL(unkSrcAddrSecBreach);
    F_NUMBER(unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unkUcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(ipv4IgmpToCpuEn);
    F_BOOL(mirrToRxAnalyzerEn);
    F_BOOL(ipv6IcmpToCpuEn);
    F_NUMBER(ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    F_NUMBER(ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_NUMBER(ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_BOOL(ipv4IpmBrgEn);
    F_BOOL(ipv6IpmBrgEn);
    F_NUMBER(ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    F_BOOL(ipv4UcastRouteEn);
    F_BOOL(ipv4McastRouteEn);
    F_BOOL(ipv6UcastRouteEn);
    F_BOOL(ipv6McastRouteEn);
    F_NUMBER(stgId, GT_U32);
    F_BOOL(autoLearnDisable);
    F_BOOL(naMsgToCpuEn);
    F_NUMBER(mruIdx, GT_U32);
    F_BOOL(bcastUdpTrapMirrEn);
    F_NUMBER(vrfId, GT_U32);
    F_NUMBER(floodVidx, GT_U16);
    F_NUMBER(floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    F_NUMBER(portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    F_BOOL(ucastLocalSwitchingEn);
    F_BOOL(mcastLocalSwitchingEn);

#undef  F_BOOL
#undef  F_NUMBER

    return 1;
}

int wrlCpssDxChVlanEntrySet(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC               portsMembersPtr;
    CPSS_PORTS_BMP_STC               portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC      vlanInfoPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                           i; /* loop iterator */
    GT_STATUS                        result;
    GT_U8                            devNum = (GT_U8)lua_tonumber(L, 1);
    GT_U16                           vlanId = (GT_U16)lua_tonumber(L, 2);

    lua_settop(L, 3);

    /* portsmembers */
    portsMembersPtr.ports[0] = 0;
    portsMembersPtr.ports[1] = 0;
    lua_getfield(L, 3, "portsMembers");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        for (i = 1; i <= lua_objlen(L, -1); i++)
        {
            lua_rawgeti(L, -1, i);
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                int portNum = lua_tointeger(L, -1);
                if (portNum < 64)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembersPtr, portNum);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    /* portstagging */
    portsTaggingPtr.ports[0] = 0;
    portsTaggingPtr.ports[1] = 0;
    lua_getfield(L, 3, "portsTaggingPtr");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        for (i = 1; i <= lua_objlen(L, -1); i++)
        {
            lua_rawgeti(L, -1, i);
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                int portNum = lua_tointeger(L, -1);
                if (portNum < 64)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsTaggingPtr, portNum);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    for (i = 0; i < 64; i++)
    {
        portsTaggingCmd.portsCmd[i] = CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsTaggingPtr, i)
                                    ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
                                    : CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

#define F_BOOL(field) \
    lua_getfield(L, 3, #field); \
    if (lua_type(L, -1) == LUA_TBOOLEAN) \
    { \
        vlanInfoPtr.field = lua_toboolean(L, -1) ? GT_TRUE : GT_FALSE; \
    } \
    else \
    { \
        vlanInfoPtr.field = GT_FALSE; \
    } \
    lua_pop(L, 1);

#define F_NUMBER(field, fieldtype) \
    lua_getfield(L, 3, #field); \
    if (lua_type(L, -1) == LUA_TNUMBER) \
    { \
        vlanInfoPtr.field = (fieldtype)lua_tointeger(L, -1); \
    } \
    else \
    { \
        vlanInfoPtr.field = (fieldtype)0; \
    } \
    lua_pop(L, 1);

    F_BOOL(unkSrcAddrSecBreach);
    F_NUMBER(unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unkUcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(ipv4IgmpToCpuEn);
    F_BOOL(mirrToRxAnalyzerEn);
    F_BOOL(ipv6IcmpToCpuEn);
    F_NUMBER(ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    F_NUMBER(ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_NUMBER(ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_BOOL(ipv4IpmBrgEn);
    F_BOOL(ipv6IpmBrgEn);
    F_NUMBER(ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    F_BOOL(ipv4UcastRouteEn);
    F_BOOL(ipv4McastRouteEn);
    F_BOOL(ipv6UcastRouteEn);
    F_BOOL(ipv6McastRouteEn);
    F_NUMBER(stgId, GT_U32);
    F_BOOL(autoLearnDisable);
    F_BOOL(naMsgToCpuEn);
    F_NUMBER(mruIdx, GT_U32);
    F_BOOL(bcastUdpTrapMirrEn);
    F_NUMBER(vrfId, GT_U32);
    F_NUMBER(floodVidx, GT_U16);
    F_NUMBER(floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    F_NUMBER(portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    F_BOOL(ucastLocalSwitchingEn);
    F_BOOL(mcastLocalSwitchingEn);

#undef  F_BOOL
#undef  F_NUMBER
    lua_getfield(L, 3, "vrfId");
    if (lua_type(L, -1) != LUA_TNUMBER)
    {
        vlanInfoPtr.vrfId = 0;
        vlanInfoPtr.floodVidx = 0xFFF;
        vlanInfoPtr.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
        vlanInfoPtr.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
        vlanInfoPtr.ucastLocalSwitchingEn = GT_FALSE;
        vlanInfoPtr.mcastLocalSwitchingEn = GT_FALSE;
    }
    lua_pop(L, 1);

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryWrite(devNum,
                                       vlanId,
                                       &portsMembersPtr,
                                       &portsTaggingPtr,
                                       &vlanInfoPtr,
                                       &portsTaggingCmd);


    lua_pushnumber(L, result);
    return 1;
}

int wrlCpssDxChVlanEntryInvalidate(
    lua_State *L
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum = (GT_U8)lua_tonumber(L, 1);
    GT_U16                          vlanId = (GT_U16)lua_tonumber(L, 2);

    rc = cpssDxChBrgVlanEntryInvalidate(
            devNum,
            vlanId);

    lua_pushnumber(L, rc);
    return 1;
}
