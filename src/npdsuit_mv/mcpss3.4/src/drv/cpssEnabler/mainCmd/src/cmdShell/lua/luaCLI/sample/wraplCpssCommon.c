#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <lua.h>

int check_wraplCpssCommon()
{
    printf("File wraplCpssCommon.c inside");
    return 1;
}

int wrlIsDevIdValid(
    lua_State *L
)
{
    lua_pushboolean(L, 
            PRV_CPSS_IS_DEV_EXISTS_MAC(lua_tointeger(L, 1)));
    return 1;
}

int wrlDevList(
    lua_State *L
)
{
    int devId;
    int c = 0;
    lua_newtable(L);
    for (devId = 0; devId < PRV_CPSS_MAX_PP_DEVICES_CNS; devId++)
    {
        if (0 != PRV_CPSS_IS_DEV_EXISTS_MAC(devId))
        {
            lua_pushnumber(L, devId);
            lua_rawseti(L, -2, ++c);
        }
    }
    return 1;
}

int wrlReadRegister(
    lua_State *L
)
{
    GT_U32      regData;
    GT_STATUS   rc;

    rc = prvCpssDrvHwPpReadRegister(
        (GT_8)lua_tointeger(L, 1),
        (GT_U32)lua_tointeger(L, 2),
        &regData);

    if (rc != GT_OK)
    {
        lua_pushfstring(L, "%d", rc);
        return 1;
    }

    lua_pushnumber(L, regData);
    return 1;
}

int wrlWriteRegister(
    lua_State *L
)
{
    GT_STATUS   rc;

    rc = prvCpssDrvHwPpWriteRegister(
        (GT_8)lua_tointeger(L, 1),
        (GT_U32)lua_tointeger(L, 2),
        (GT_U32)lua_tointeger(L, 3));

    lua_pushnumber(L, rc);
    return 1;
}
