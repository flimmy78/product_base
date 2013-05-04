/*************************************************************************
* wraplCpssPortCntr.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*
*       CPSS_PORT_INTERFACE_MODE_ENT
*       CPSS_PORT_SPEED_ENT
*       CPSS_INTERFACE_INFO_STC    
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
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <lua.h>
#include <cmdShell/lua/genwrapper_API.h>

enumDescr enumDescr_CPSS_PORT_INTERFACE_MODE_ENT[] = {
    {CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E,   "REDUCED_10Bits"},
    {CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E,    "RGMII"}, 
    {CPSS_PORT_INTERFACE_MODE_MII_E,             "MII"},
    {CPSS_PORT_INTERFACE_MODE_SGMII_E,           "SGMII"},
    {CPSS_PORT_INTERFACE_MODE_XGMII_E,           "XGMII"},
    {CPSS_PORT_INTERFACE_MODE_MGMII_E,           "MGMII"},
    {CPSS_PORT_INTERFACE_MODE_1000BASE_X_E,      "1000BASE_X"},
    {CPSS_PORT_INTERFACE_MODE_GMII_E,            "GMII"},
    {CPSS_PORT_INTERFACE_MODE_MII_PHY_E,         "MII_PHY"},
    {CPSS_PORT_INTERFACE_MODE_QX_E,              "QX"},
    {CPSS_PORT_INTERFACE_MODE_HX_E,              "HX"},
    {CPSS_PORT_INTERFACE_MODE_RXAUI_E,           "RXAUI"},
    {CPSS_PORT_INTERFACE_MODE_100BASE_FX_E,      "100BASE_FX"},
    {CPSS_PORT_INTERFACE_MODE_QSGMII_E,          "QSGMII"},
    {CPSS_PORT_INTERFACE_MODE_XLG_E,             "XLG"},
    {CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,     "XGMII"},
    { 0, NULL }
};

enumDescr enumDescr_CPSS_PORT_SPEED_ENT[] = {    
    {CPSS_PORT_SPEED_10_E, "speed_10"},
    {CPSS_PORT_SPEED_100_E, "speed_100"},
    {CPSS_PORT_SPEED_1000_E, "speed_1G"},
    {CPSS_PORT_SPEED_10000_E, "speed_10G"},
    {CPSS_PORT_SPEED_12000_E, "speed_12G"},
    {CPSS_PORT_SPEED_2500_E,  "speed_2.5G"},
    {CPSS_PORT_SPEED_5000_E,  "speed_5G"},
    {CPSS_PORT_SPEED_13600_E, "speed_13.6G"},
    {CPSS_PORT_SPEED_20000_E, "speed_20G"}, 
    {CPSS_PORT_SPEED_40000_E, "speed_40G"},
    {CPSS_PORT_SPEED_16000_E, "speed_16G"},
    { 0, NULL }
};


/*******************************************/
/* CPSS_INTERFACE_INFO_STC                 */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_PORT_MODE_SPEED_STC
*
* Description:
*       Convert to CPSS_PORT_MODE_SPEED_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_PORT_MODE_SPEED_STC(
    lua_State *L,
    CPSS_PORT_MODE_SPEED_STC *info
)
{
    F_ENUM(info, -1, ifMode, CPSS_PORT_INTERFACE_MODE_ENT);
    F_ENUM(info, -1, speed, CPSS_PORT_SPEED_ENT);
}

/*************************************************************************
* prv_c_to_lua_CPSS_PORT_MODE_SPEED_STC
*
* Description:
*       Convert CPSS_PORT_MODE_SPEED_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_PORT_MODE_SPEED_STC(
    lua_State *L,
    CPSS_PORT_MODE_SPEED_STC *info
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(info, t, ifMode, CPSS_PORT_INTERFACE_MODE_ENT);
    FO_ENUM(info, t, speed,  CPSS_PORT_SPEED_ENT);
}

add_mgm_type(CPSS_PORT_MODE_SPEED_STC)

