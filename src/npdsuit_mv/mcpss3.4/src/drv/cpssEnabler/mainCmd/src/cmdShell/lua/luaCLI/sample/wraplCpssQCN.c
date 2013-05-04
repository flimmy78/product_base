/*************************************************************************
* wraplCpssQCN.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*       CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC
*       CPSS_PORT_CNM_GENERATION_CONFIG_STC
*       CPSS_PORT_CN_PROFILE_CONFIG_STC
*typedef struct {
*
*    GT_U32   qosProfileId;
*    GT_U8    isRouted;
*    GT_BOOL  overrideUp;
*    GT_U8    cnmUp;
*    GT_U16   defaultVlanId;
*    GT_U32   scaleFactor;
*    GT_U32   version;
*    GT_U64   cpidMsb;
*    GT_BOOL  cnUntaggedEnable;
*    GT_BOOL  forceCnTag;
*    GT_U32   flowIdTag;
*    GT_BOOL  appendPacket;
*} CPSS_PORT_CNM_GENERATION_CONFIG_STC;
*
*typedef struct {
*    GT_BOOL cnAware;
*    GT_U32  threshold;
*} CPSS_PORT_CN_PROFILE_CONFIG_STC;
*
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
#include <cpss/extServices/os/gtOs/gtEnvDep.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

void prv_lua_to_c_GT_U64(lua_State *L, GT_U64 *val);
void prv_c_to_lua_GT_U64(lua_State *L, GT_U64 *val);


int check_wraplCpssQCN()
{
    return 1;
}


/*******************************************/
/* CPSS_PORT_CN_PROFILE_CONFIG_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_PORT_CN_PROFILE_CONFIG_STC 
*
* Description:
*       Convert to CPSS_PORT_CN_PROFILE_CONFIG_STC 
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_PORT_CN_PROFILE_CONFIG_STC (
    lua_State *L,
    CPSS_PORT_CN_PROFILE_CONFIG_STC  *val
)
{
    F_BOOL  (val, -1,cnAware);
    F_NUMBER(val, -1,threshold,   GT_U32);
}

/*************************************************************************
* prv_c_to_lua_CPSS_PORT_CN_PROFILE_CONFIG_STC
*
* Description:
*       Convert CPSS_PORT_CN_PROFILE_CONFIG_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_PORT_CN_PROFILE_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CN_PROFILE_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL  (val,  t, cnAware);
    FO_NUMBER(val,  t, threshold,   GT_U32);
}

add_mgm_type(CPSS_PORT_CN_PROFILE_CONFIG_STC);
/*******************************************/
/* CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC 
*
* Description:
*       Convert to CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC 
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC (
    lua_State *L,
    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC  *val
)
{
    F_NUMBER(val, -1,wExp,        GT_U32);
    F_NUMBER(val, -1,fbLsb,       GT_U32);
    F_BOOL  (val, -1,deltaEnable);
    F_NUMBER(val, -1,fbMin,       GT_U32);
    F_NUMBER(val, -1,fbMax,       GT_U32);
}

/*************************************************************************
* prv_c_to_lua_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC
*
* Description:
*       Convert CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val,  t, wExp,        GT_U32);
    FO_NUMBER(val,  t, fbLsb,       GT_U32);
    FO_BOOL  (val,  t, deltaEnable);
    FO_NUMBER(val,  t, fbMin,       GT_U32);
    FO_NUMBER(val,  t, fbMax,       GT_U32);
}

add_mgm_type(CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC);


/*******************************************/
/* CPSS_PORT_CNM_GENERATION_CONFIG_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_PORT_CNM_GENERATION_CONFIG_STC 
*
* Description:
*       Convert to CPSS_PORT_CNM_GENERATION_CONFIG_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_PORT_CNM_GENERATION_CONFIG_STC (
    lua_State *L,
    CPSS_PORT_CNM_GENERATION_CONFIG_STC  *val
)
{   
    F_NUMBER(val, -1,qosProfileId,        GT_U32);
    F_NUMBER(val, -1,isRouted,            GT_U8);
    F_BOOL  (val, -1,overrideUp);
    F_NUMBER(val, -1,cnmUp,               GT_U8);
    F_NUMBER(val, -1,defaultVlanId,       GT_U16);
    F_NUMBER(val, -1,scaleFactor,         GT_U32);
    F_NUMBER(val, -1,version,             GT_U32);
    F_STRUCT(val, -1,cpidMsb,             GT_U64);
    F_BOOL  (val, -1,cnUntaggedEnable);
    F_BOOL  (val, -1,forceCnTag);
    F_NUMBER(val, -1,flowIdTag,           GT_U32);
    F_BOOL  (val, -1,appendPacket);
}

/*************************************************************************
* prv_c_to_lua_CPSS_PORT_CNM_GENERATION_CONFIG_STC
*
* Description:
*       Convert CPSS_PORT_CNM_GENERATION_CONFIG_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_PORT_CNM_GENERATION_CONFIG_STC(
    lua_State *L,
    CPSS_PORT_CNM_GENERATION_CONFIG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);

    FO_NUMBER(val, t, qosProfileId,        GT_U32);
    FO_NUMBER(val, t, isRouted,            GT_U8);
    FO_BOOL  (val, t, overrideUp);
    FO_NUMBER(val, t, cnmUp,               GT_U8);
    FO_NUMBER(val, t, defaultVlanId,       GT_U16);
    FO_NUMBER(val, t, scaleFactor,         GT_U32);
    FO_NUMBER(val, t, version,             GT_U32);
    FO_STRUCT(val, t, cpidMsb,             GT_U64);
    FO_BOOL  (val, t, cnUntaggedEnable);
    FO_BOOL  (val, t, forceCnTag);
    FO_NUMBER(val, t, flowIdTag,           GT_U32);
    FO_BOOL  (val, t, appendPacket);
}

add_mgm_type(CPSS_PORT_CNM_GENERATION_CONFIG_STC);

