#error temporary disabled code

/* A mainCmd network wrapper for cmdLua */

#include <cmdShell/common/cmdCommon.h>

#include "lua.h"
#include "lualib.h"

#include "cmdLua.h"

/*******************************************************************************
* cmdLuaNetConnect
*
* DESCRIPTION:
*       Connect to remote host
*
* INPUTS:
*       L       - Lua state
*         1st   - host name
*         2nd   - port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string or nil if error
*       connection descriptor
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetConnect(lua_State *L)
{
    GT_8*           ipAddr;
    GT_SOCKET_PORT  ipPort;
    GT_VOID *       sockAddr;
    GT_SIZE         sockAddrLen;
    GT_STATUS       ret;
    GT_SOCKET_FD    sock;

    ipAddr = (GT_8*)lua_tostring(L, 1);
    ipPort = (GT_SOCKET_PORT)lua_tointeger(L, 2);
    if (ipPort == 0)
    {
        lua_pushfstring(L, "Bad port: %s must be a number", lua_tostring(L, 2));
        return 1;
    }

    ret = cmdOsSocketCreateAddr(ipAddr, ipPort, &sockAddr, &sockAddrLen);
    if (ret != GT_OK)
    {
        lua_pushfstring(L, "failed to parse ip address: %s", lua_tostring(L, 1));
        return 1;
    }
    sock = cmdOsSocketTcpCreate(-1/* default buffers */);
    if (sock < 0)
    {
        lua_pushliteral(L, "failed to create TCP socket");
        cmdOsSocketDestroyAddr(sockAddr);
        return 1;
    }
    ret = cmdOsSocketConnect(sock, sockAddr, sockAddrLen);
    if (ret != GT_OK)
    {
        lua_pushfstring(L, "failed to connect to %s:%s",
                lua_tostring(L, 1), lua_tostring(L, 2));
        cmdOsSocketTcpDestroy(sock);
        cmdOsSocketDestroyAddr(sockAddr);
        return 1;

    }

    cmdOsSocketTcpDestroy(sock);

    /* return sock */
    lua_pushnumber(L, sock);
    return 1;
}

/*******************************************************************************
* cmdLuaNetDisconnect
*
* DESCRIPTION:
*       disconnect
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetDisconnect(lua_State *L)
{
    GT_SOCKET_FD    sock;

    if (!lua_isnumber(L, 1))
        return 0;
    sock = lua_tointeger(L, 1);

    cmdOsSocketTcpDestroy(sock);
    return 0;
}

/*******************************************************************************
* cmdLuaNetFlushInput
*
* DESCRIPTION:
*       flush input, make sure noting left in buffer (sync)
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetFlushInput(lua_State *L)
{
    GT_SOCKET_FD    sock;
    GT_VOID*        rfds;

    if (!lua_isnumber(L, 1))
        return 0;
    sock = lua_tointeger(L, 1);

    rfds = cmdOsSelectCreateSet();
    if (!rfds)
        return 0;

    while (1)
    {
        int     k;
        GT_8    buf[64];

        cmdOsSelectZeroSet(rfds);
        cmdOsSelectAddFdToSet(rfds, sock);

        k = cmdOsSelect(sock + 1, rfds, NULL, NULL, CMD_LUA_NET_FLUSH_TIMEOUT);
        if (k <= 0)
            break;
        cmdOsSocketRecv(sock, buf, sizeof(buf), GT_TRUE);
    }
    cmdOsSelectEraseSet(rfds);
    return 0;
}

/*******************************************************************************
* cmdLuaNetSend
*
* DESCRIPTION:
*       Send string to connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - string to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetSend(lua_State *L)
{
    GT_SOCKET_FD    sock;
    const char      *s;
    size_t          len;

    if (!lua_isnumber(L, 1))
        return 0;
    sock = lua_tointeger(L, 1);

    s = lua_tolstring(L, 2, &len);

    if (s)
        cmdOsSocketSend(sock, (GT_8*)s, len);
    return 0;
}

/*******************************************************************************
* cmdLuaNetWaitString
*
* DESCRIPTION:
*       Wait for a string from connection
*
* INPUTS:
*       L       - Lua state
*         1st   - connection descriptor
*         2nd   - timeout. Default is CMD_LUA_NET_WAITSTRING_TIMEOUT
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string if successful
*       nil if timeout
*       any other on error
*
* COMMENTS:
*
*******************************************************************************/
int cmdLuaNetWaitString(lua_State *L)
{
    GT_SOCKET_FD    sock;
    int             timeout = CMD_LUA_NET_WAITSTRING_TIMEOUT;
    char            buf[64];
    int             p;
    GT_VOID*        rfds;

    if (!lua_isnumber(L, 1))
    {
        lua_pushnumber(L, 1); /* error 1: bad descritor */
        return 1;
    }
    sock = lua_tointeger(L, 1);

    if (lua_isnumber(L, 2))
        timeout = lua_tointeger(L, 2);

    rfds = cmdOsSelectCreateSet();
    if (!rfds)
    {
        lua_pushnumber(L, 2); /* error 2: can't create fd_set */
        return 1;
    }
    
    lua_pushstring(L, "");
    p = 0;
    while (1)
    {
        int k;

        cmdOsSelectZeroSet(rfds);
        cmdOsSelectAddFdToSet(rfds, sock);

        k = cmdOsSelect(sock + 1, rfds, NULL, NULL, timeout);
        /* this cannot be handled due to lag in API:
        if (k < 0 && errno == EINTR)
            continue;
        */
        if (k == 0)
        {
            lua_pop(L, 1);
            lua_pushnil(L);
            break;
        }
        if (k < 0)
        {
            lua_pop(L, 1);
            lua_pushnumber(L, 3); /* error 3: select() returned error */
            break;
        }
        k = cmdOsSocketRecv(sock, buf + p, 1, GT_TRUE);
        if (k == 0)
        {
            /* eof */
            break;
        }
        if (k < 0)
        {
            lua_pop(L, 1);
            lua_pushnumber(L, 4); /* error 4: select flags input ready but recv returned error */
            break;
        }
        if (buf[p] == '\n')
        {
            if (p) {
                lua_pushlstring(L, buf, p);
                lua_concat(L, 2);
                break;
            }
        }
        p++;
        if (p == sizeof(buf))
        {
            lua_pushlstring(L, buf, p);
            lua_concat(L, 2);
            p = 0;
        }
    }
    cmdOsSelectEraseSet(rfds);
    return 1;
}

