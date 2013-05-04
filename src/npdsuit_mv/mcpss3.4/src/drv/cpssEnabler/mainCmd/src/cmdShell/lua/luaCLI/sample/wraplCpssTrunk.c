/*************************************************************************
* wraplCpssTrunk.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*
*           CPSS_TRUNK_MEMBER_STC
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
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

int check_wraplCpssTrunk()
{
    return 1;
}


/*******************************************/
/* CPSS_TRUNK_MEMBER_STC                   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_TRUNK_MEMBER_STC
*
* Description:
*       Convert to CPSS_CPSS_TRUNK_MEMBER_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_TRUNK_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_MEMBER_STC *val
)
{
    F_NUMBER(val, -1, port, GT_U8);
    F_NUMBER(val, -1, device, GT_U8);
}

/*************************************************************************
* prv_c_to_lua_CPSS_TRUNK_MEMBER_STC
*
* Description:
*       Convert CPSS_TRUNK_MEMBER_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_TRUNK_MEMBER_STC(
    lua_State *L,
    CPSS_TRUNK_MEMBER_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, port, GT_U8);
    FO_NUMBER(val, t, device, GT_U8);
}

add_mgm_type(CPSS_TRUNK_MEMBER_STC);

