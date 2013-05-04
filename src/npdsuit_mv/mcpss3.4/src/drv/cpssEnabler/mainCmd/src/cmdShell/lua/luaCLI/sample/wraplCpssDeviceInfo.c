/*************************************************************************
* wraplCpssDeviceInfo.c
*
* DESCRIPTION:
*       A lua wrapper for 
*   
*   typedef struct
*   {
*        CPSS_PP_DEVICE_TYPE             devType;
*        GT_U8                           revision;
*        CPSS_PP_FAMILY_TYPE_ENT         devFamily;
*        GT_U8                           maxPortNum;
*        GT_U8                           numOfVirtPorts;
*        CPSS_PORTS_BMP_STC              existingPorts;
*   }CPSS_GEN_CFG_DEV_INFO_STC;
*   and
*   
*  
*
*   typedef struct{
*        GT_U32      ports[2];
*   }CPSS_PORTS_BMP_STC;
*   and
*
*   typedef struct
*   {
*        CPSS_GEN_CFG_DEV_INFO_STC    genDevInfo;
*    }CPSS_DXCH_CFG_DEV_INFO_STC;
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
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

/* externs */
void prv_lua_to_c_CPSS_PORTS_BMP_STC(lua_State *L, CPSS_PORTS_BMP_STC *val);
void prv_c_to_lua_CPSS_PORTS_BMP_STC(lua_State *L, CPSS_PORTS_BMP_STC *val);


int check_wraplCpssDeviceConfig()
{
    return 1;
}


/*******************************************/
/* CPSS_GEN_CFG_DEV_INFO_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_GEN_CFG_DEV_INFO_STC
*
* Description:
*       Convert to "c" CPSS_GEN_CFG_DEV_INFO_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_GEN_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_GEN_CFG_DEV_INFO_STC *val
)
{
    F_NUMBER(val, -1, devType, GT_U32);
    F_NUMBER(val, -1, revision, GT_U8);
    F_NUMBER(val, -1, devFamily, GT_U32);
    F_NUMBER(val, -1, maxPortNum, GT_U8);
    F_NUMBER(val, -1, numOfVirtPorts, GT_U8);
    F_STRUCT(val, -1, existingPorts, CPSS_PORTS_BMP_STC);    
}

/*************************************************************************
* prv_c_to_lua_CPSS_GEN_CFG_DEV_INFO_STC
*
* Description:
*       Convert CPSS_GEN_CFG_DEV_INFO_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_GEN_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_GEN_CFG_DEV_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, devType, GT_U32);
    FO_NUMBER(val, t, revision, GT_U8);
    FO_NUMBER(val, t, devFamily, GT_U32);
    FO_NUMBER(val, t, maxPortNum, GT_U8);
    FO_NUMBER(val, t, numOfVirtPorts, GT_U8);
    FO_STRUCT(val, t, existingPorts, CPSS_PORTS_BMP_STC)   
}

add_mgm_type(CPSS_GEN_CFG_DEV_INFO_STC);

/*******************************************/
/* CPSS_DXCH_CFG_DEV_INFO_STC   */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_DXCH_CFG_DEV_INFO_STC
*
* Description:
*       Convert to "c" CPSS_DXCH_CFG_DEV_INFO_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_DXCH_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_DXCH_CFG_DEV_INFO_STC *val
)
{
    F_STRUCT(val, -1, genDevInfo, CPSS_GEN_CFG_DEV_INFO_STC);    
}

/*************************************************************************
* prv_c_to_lua_CPSS_DXCH_CFG_DEV_INFO_STC
*
* Description:
*       Convert CPSS_DXCH_CFG_DEV_INFO_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_DXCH_CFG_DEV_INFO_STC(
    lua_State *L,
    CPSS_DXCH_CFG_DEV_INFO_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_STRUCT(val, t, genDevInfo, CPSS_GEN_CFG_DEV_INFO_STC)   
}

add_mgm_type(CPSS_DXCH_CFG_DEV_INFO_STC);

