/*************************************************************************
* wraplCpssExMxPmIpTypes.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*
*           CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT
*           CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC
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
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>


enumDescr enumDescr_CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT[] = {
    ENUM_ENTRY(CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E),
    ENUM_ENTRY(CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E),
    ENUM_ENTRY(CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E),
    { 0, NULL }
};

/*******************************************/
/* CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC
*
* Description:
*       Convert to CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC(
    lua_State *L,
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC *entry
)
{
    F_NUMBER(entry, -1, routeEntryBaseMemAddr, GT_U32);
    F_NUMBER(entry, -1, blockSize, GT_U32);
    F_ENUM(entry, -1, routeEntryMethod, CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT);
}

/*************************************************************************
* prv_c_to_lua_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC
*
* Description:
*       Convert CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC(
    lua_State *L,
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC *entry
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(entry, t, routeEntryBaseMemAddr, GT_U32);
    FO_NUMBER(entry, t, blockSize, GT_U32);
    FO_ENUM(entry, t, routeEntryMethod, CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT);
    lua_settop(L, t);
}
add_mgm_type(CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC)
