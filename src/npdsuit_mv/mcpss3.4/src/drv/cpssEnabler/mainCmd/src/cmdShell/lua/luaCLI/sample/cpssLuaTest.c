
#include <lua.h>


int cpssLuaTest_Hello(
    lua_State *L
)
{
    lua_getglobal(L, "print");
    lua_pushstring(L, "cpssLuaTest_Hello(): works");
    lua_call(L, 1, 0);
    return 0;
}

