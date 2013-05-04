/*************************************************************************
* wraplCpssPFC.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*
*           typedef struct {
*				GT_U32 xonThreshold;
*				GT_U32 xoffThreshold;
*			} CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC;
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
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

int check_wraplCpssPFC()
{
    return 1;
}


/*******************************************/
/* CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
*
* Description:
*       Convert to CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC *val
)
{
    F_NUMBER(val, -1,xonThreshold, GT_U32);
    F_NUMBER(val, -1,xoffThreshold, GT_U32);
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC
*
* Description:
*       Convert CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, xonThreshold, GT_U32);
    FO_NUMBER(val, t, xoffThreshold, GT_U32);
}

add_mgm_type(CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC);

