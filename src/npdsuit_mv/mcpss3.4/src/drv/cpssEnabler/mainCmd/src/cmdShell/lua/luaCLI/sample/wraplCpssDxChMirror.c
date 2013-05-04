/*************************************************************************
* wraplCpssDxChMirror.c
*
* DESCRIPTION:
*       A lua wrapper for mirroring functions
*       It implements support for the following types:
*
*           CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

int check_wraplCpssDxChMirror()
{
    return 1;
}

/* externs */
void prv_lua_to_c_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);
void prv_c_to_lua_CPSS_INTERFACE_INFO_STC(lua_State *L, CPSS_INTERFACE_INFO_STC *info);

/*******************************************/
/* CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
*
* Description:
*       Convert to CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
static void prv_lua_to_c_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC *val
)
{
    F_STRUCT(val, -1, interface, CPSS_INTERFACE_INFO_STC);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC
*
* Description:
*       Convert CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
static void prv_c_to_lua_CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC(
    lua_State *L,
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, interface, CPSS_INTERFACE_INFO_STC);
}

add_mgm_type(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC);

