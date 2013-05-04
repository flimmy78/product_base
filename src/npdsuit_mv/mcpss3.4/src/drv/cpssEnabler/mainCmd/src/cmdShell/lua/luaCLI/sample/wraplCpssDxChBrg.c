/*************************************************************************
* wraplCpssDxChBrg.c
*
* DESCRIPTION:
*       A lua wrapper for bridging functions
*       It implements support for the following types:
*           CPSS_MAC_ENTRY_EXT_KEY_STC (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E only)
*           CPSS_MAC_ENTRY_EXT_STC
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

/* externs */
void prv_lua_to_c_GT_ETHERADDR(lua_State *L, GT_ETHERADDR *e);
void prv_c_to_lua_GT_ETHERADDR(lua_State *L, GT_ETHERADDR *e);
void prv_lua_to_c_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);
void prv_c_to_lua_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);



int check_wraplCpssDxChBrg()
{
    return 1;
}


enumDescr enumDescr_CPSS_MAC_TABLE_CMD_ENT[] = {
    { CPSS_MAC_TABLE_FRWRD_E, "forward" }, 
    { CPSS_MAC_TABLE_DROP_E, "drop" },
    { CPSS_MAC_TABLE_INTERV_E, "interv" },
    { CPSS_MAC_TABLE_CNTL_E, "cntl" },
    { CPSS_MAC_TABLE_MIRROR_TO_CPU_E, "mirror-to-cpu" },
    { CPSS_MAC_TABLE_SOFT_DROP_E, "soft-drop" },
    { 0, NULL }
};

enumDescr enumDescr_CPSS_MAC_ENTRY_EXT_TYPE_ENT[] = {
    { CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E, "mac-addr" },
    { CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E, "ipv4-mcast" },
    { CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E, "ipv6-mcast" },
    { 0, NULL }
};

/*************************************************************************
* prv_lua_to_c_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Convert to CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_MAC_ENTRY_EXT_KEY_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_KEY_STC *key
)
{
    F_ENUM(key, -1, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    switch (key->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            lua_getfield(L, 1, "macAddr");
            prv_lua_to_c_GT_ETHERADDR(L, &(key->key.macVlan.macAddr));
            lua_pop(L, 1);
            F_NUMBER(&(key->key.macVlan), -1, vlanId, GT_U16);
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            /* TBD */
            break;
    }
}


/*************************************************************************
* prv_c_to_lua_CPSS_MAC_ENTRY_EXT_KEY_STC
*
* Description:
*       Convert CPSS_MAC_ENTRY_EXT_KEY_STC to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*       
*
*************************************************************************/
static void prv_c_to_lua_CPSS_MAC_ENTRY_EXT_KEY_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_KEY_STC *key
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(key, t, entryType, CPSS_MAC_ENTRY_EXT_TYPE_ENT);
    switch (key->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            prv_c_to_lua_GT_ETHERADDR(L, &(key->key.macVlan.macAddr));
            lua_setfield(L, t, "macAddr");
            FO_NUMBER(&(key->key.macVlan), t, vlanId, GT_U16);
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            /* TBD */
            break;
    }

    lua_settop(L, t);
}
add_mgm_type(CPSS_MAC_ENTRY_EXT_KEY_STC);



/*************************************************************************
* prv_lua_to_c_CPSS_MAC_ENTRY_EXT_STC
*
* Description:
*       Convert to CPSS_MAC_ENTRY_EXT_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*
*************************************************************************/
static void prv_lua_to_c_CPSS_MAC_ENTRY_EXT_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_STC *val
)
{
    F_STRUCT(val, -1, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    F_STRUCT(val, -1, dstInterface, CPSS_INTERFACE_INFO_STC);
    F_BOOL(val, -1, isStatic);
    F_ENUM(val, -1, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_ENUM(val, -1, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    F_BOOL(val, -1, daRoute);
    F_BOOL(val, -1, mirrorToRxAnalyzerPortEn);
    F_NUMBER(val, -1, sourceID, GT_U32);
    F_NUMBER(val, -1, userDefined, GT_U32);
    F_NUMBER(val, -1, daQosIndex, GT_U32);
    F_NUMBER(val, -1, saQosIndex, GT_U32);
    F_NUMBER(val, -1, daSecurityLevel, GT_U32);
    F_NUMBER(val, -1, saSecurityLevel, GT_U32);
    F_BOOL(val, -1, appSpecificCpuCode);
    F_BOOL(val, -1, age);
}

/*************************************************************************
* prv_c_to_lua_CPSS_MAC_ENTRY_EXT_STC
*
* Description:
*       Convert CPSS_MAC_ENTRY_EXT_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_MAC_ENTRY_EXT_STC(
    lua_State *L,
    CPSS_MAC_ENTRY_EXT_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);

    FO_STRUCT(val, t, key, CPSS_MAC_ENTRY_EXT_KEY_STC);
    FO_STRUCT(val, t, dstInterface, CPSS_INTERFACE_INFO_STC);
    FO_BOOL(val, t, isStatic);
    FO_ENUM(val, t, daCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_ENUM(val, t, saCommand, CPSS_MAC_TABLE_CMD_ENT);
    FO_BOOL(val, t, daRoute);
    FO_BOOL(val, t, mirrorToRxAnalyzerPortEn);
    FO_NUMBER(val, t, sourceID, GT_U32);
    FO_NUMBER(val, t, userDefined, GT_U32);
    FO_NUMBER(val, t, daQosIndex, GT_U32);
    FO_NUMBER(val, t, saQosIndex, GT_U32);
    FO_NUMBER(val, t, daSecurityLevel, GT_U32);
    FO_NUMBER(val, t, saSecurityLevel, GT_U32);
    FO_BOOL(val, t, appSpecificCpuCode);
    FO_BOOL(val, t, age);
}

add_mgm_type(CPSS_MAC_ENTRY_EXT_STC);

