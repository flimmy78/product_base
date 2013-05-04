/*************************************************************************
* wraplCpssTypes.c
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*
*           typedef struct{
*                GT_U32      ports[2];
*           }CPSS_PORTS_BMP_STC;
*
*           CPSS_INTERFACE_INFO_STC
*           GT_ETHERADDR
*           GT_IPADDR
*           GT_IPADDRPTR
*           GT_IPV6ADDR
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
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>


enumDescr enumDescr_CPSS_INTERFACE_TYPE_ENT[] = {
    { CPSS_INTERFACE_PORT_E, "port" },
    { CPSS_INTERFACE_TRUNK_E, "trunk" }, 
    { CPSS_INTERFACE_VIDX_E, "vidx" },
    { CPSS_INTERFACE_VID_E, "vid" },
    { CPSS_INTERFACE_DEVICE_E, "device" },
    { CPSS_INTERFACE_FABRIC_VIDX_E, "fabric-vidx" },
    { CPSS_INTERFACE_INDEX_E, "index" },
    { 0, NULL }
};


/*******************************************/
/* GT_ETHERADDR                            */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_ETHERADDR
*
* Description:
*       Convert to GT_ETHERADDR
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_GT_ETHERADDR(
    lua_State *L,
    GT_ETHERADDR *e
)
{
    static const char hexcode[] = "0123456789ABCDEF";
    int k;
    const char *s;
    const char *p;
    cpssOsMemSet(e, 0, sizeof(*e));
    if (!lua_isstring(L, -1))
        return;
    s = lua_tostring(L, -1);

    for (k = 0; k < 6; k++)
    {
        if (*s == 0) break;
        if (*s == ':') s++;
        p = cpssOsStrChr(hexcode, cpssOsToUpper(*s++));
        if (p)
            e->arEther[k] = (p - hexcode) << 4;
        else
            e->arEther[k] = 0;

        p = cpssOsStrChr(hexcode, cpssOsToUpper(*s++));
        if (p)
            e->arEther[k] += (p - hexcode);
    }
}

/*************************************************************************
* prv_c_to_lua_GT_ETHERADDR
*
* Description:
*       Convert GT_ETHERADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_ETHERADDR(
    lua_State *L,
    GT_ETHERADDR *e
)
{
    char s[32];
    sprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x",
            e->arEther[0], e->arEther[1], e->arEther[2],
            e->arEther[3], e->arEther[4], e->arEther[5]);
    lua_pushstring(L, s);
}

add_mgm_type(GT_ETHERADDR);

/*******************************************/
/* CPSS_INTERFACE_INFO_STC                 */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Convert to CPSS_INTERFACE_INFO_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_INTERFACE_INFO_STC *info
)
{
    F_ENUM(info, -1, type, CPSS_INTERFACE_TYPE_ENT);
    lua_getfield(L, -1, "devPort");
    if (lua_istable(L, -1))
    {
        F_NUMBER(&(info->devPort), -1, devNum, GT_U8);
        F_NUMBER(&(info->devPort), -1, portNum, GT_U8);
    }
    lua_pop(L, 1);
    F_NUMBER(info, -1, trunkId, GT_TRUNK_ID);
    F_NUMBER(info, -1, vidx, GT_U16);
    F_NUMBER(info, -1, vlanId, GT_U16);
    F_NUMBER(info, -1, devNum, GT_U8);
    F_NUMBER(info, -1, fabricVidx, GT_U8);
    F_NUMBER(info, -1, index, GT_U32);
    
}

/*************************************************************************
* prv_c_to_lua_CPSS_INTERFACE_INFO_STC
*
* Description:
*       Convert CPSS_INTERFACE_INFO_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_INTERFACE_INFO_STC(
    lua_State *L,
    CPSS_INTERFACE_INFO_STC *info
)
{
    int t, t1;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ENUM(info, t, type, CPSS_INTERFACE_TYPE_ENT);
    lua_newtable(L);
    t1 = lua_gettop(L);
        FO_NUMBER(&(info->devPort), t1, devNum, GT_U8);
        FO_NUMBER(&(info->devPort), t1, portNum, GT_U8);
    lua_setfield(L, t, "devPort");
    FO_NUMBER(info, t, trunkId, GT_TRUNK_ID);
    FO_NUMBER(info, t, vidx, GT_U16);
    FO_NUMBER(info, t, vlanId, GT_U16);
    FO_NUMBER(info, t, devNum, GT_U16);
    FO_NUMBER(info, t, fabricVidx, GT_U8);
    FO_NUMBER(info, t, index, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(CPSS_INTERFACE_INFO_STC)

/*******************************************/
/* CPSS_PORTS_BMP_STC                      */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_CPSS_PORTS_BMP_STC
*
* Description:
*       Convert to "c" CPSS_PORTS_BMP_STC
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_CPSS_PORTS_BMP_STC(
    lua_State *L,
    CPSS_PORTS_BMP_STC *val
)
{
    F_ARRAY_START(val, -1, ports);
        F_ARRAY_NUMBER(val, ports, 0, GT_U32);
        F_ARRAY_NUMBER(val, ports, 1, GT_U32);
    F_ARRAY_END(val, -1, ports);
}

/*************************************************************************
* prv_c_to_lua_CPSS_PORTS_BMP_STC
*
* Description:
*       Convert CPSS_PORTS_BMP_STC to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_CPSS_PORTS_BMP_STC(
    lua_State *L,
    CPSS_PORTS_BMP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, ports);
        FO_ARRAY_NUMBER(val, ports, 0, GT_U32);
        FO_ARRAY_NUMBER(val, ports, 1, GT_U32);
    FO_ARRAY_END(val, t, ports);
}

add_mgm_type(CPSS_PORTS_BMP_STC);

/*******************************************/
/* GT_IPADDR                               */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_IPADDR
*
* Description:
*       Convert to GT_IPADDR
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_GT_IPADDR(
    lua_State *L,
    GT_IPADDR *ip
)
{
    cpssOsMemSet(ip, 0, sizeof(*ip));
    if (lua_type(L, -1) == LUA_TNUMBER)
    {
        ip->u32Ip = (GT_U32)lua_tointeger(L, -1);
    }
    else if (lua_type(L, -1) == LUA_TSTRING)
    {
        /* parse dotted IP string */
        int k;
        int n;
        const char *s;
        s = lua_tostring(L, -1);

        k = 0;
        for (; *s && k < 4; s++)
        {
            if (*s == '.')
            {
                k++;
            }
            else if ((*s) >= '0' && (*s) <= '9')
            {
                n = ip->arIP[k] * 10 + (*s) - '0';
                if (n > 255)
                    break;
                ip->arIP[k] = n;
            }
            else
            {
                break;
            }
        }
    }
}

/*************************************************************************
* prv_c_to_lua_GT_IPADDR
*
* Description:
*       Convert GT_IPADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPADDR(
    lua_State *L,
    GT_IPADDR *ip
)
{
    char s[16];
    sprintf(s, "%d.%d.%d.%d",
            ip->arIP[0], ip->arIP[1],
            ip->arIP[2], ip->arIP[3]);
    lua_pushstring(L, s);
}

int mgmType_to_c_GT_IPADDR(
    lua_State *L
)
{
    GT_IPADDR ipAddr;
    lua_pushvalue(L, 1);
    prv_lua_to_c_GT_IPADDR(L, &ipAddr);
    lua_pop(L, 1);
    lua_pushnumber(L, ipAddr.u32Ip);
    return 1;
}
int mgmType_to_lua_GT_IPADDR(
    lua_State *L
)
{
    GT_IPADDR ipAddr;
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    ipAddr.u32Ip = lua_tointeger(L, 1);
    prv_c_to_lua_GT_IPADDR(L, &ipAddr);
    return 1;
}

/* GT_IPADDRPTR */
int mgmType_to_c_GT_IPADDRPTR(
    lua_State *L
)
{
    GT_IPADDR *val;
    lua_newuserdata(L, sizeof(GT_IPADDR));
    if (lua_isnil(L, 1))
    {
        return 1;
    }

    val = (GT_IPADDR*)lua_touserdata(L, -1);
    lua_pushvalue(L, 1);
    prv_lua_to_c_GT_IPADDR(L, val);
    lua_pop(L, 1);
    return 1;
}

int mgmType_to_lua_GT_IPADDRPTR(
    lua_State *L
)
{
    GT_IPADDR *val;
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isuserdata(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }

    val = (GT_IPADDR*)lua_touserdata(L, 1);
    prv_c_to_lua_GT_IPADDR(L, val);

    return 1;
}

/*******************************************/
/* GT_IPV6ADDR                             */
/*******************************************/
/*************************************************************************
* prv_lua_to_c_GT_IPV6ADDR
*
* Description:
*       Convert to GT_IPV6ADDR
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_GT_IPV6ADDR(
    lua_State *L,
    GT_IPV6ADDR *ip
)
{
    int k, n;
    GT_U32 cur;
    const char *s;

    cpssOsMemSet(ip, 0, sizeof(*ip));
    if (lua_type(L, -1) != LUA_TSTRING)
    {
        return;
    }
    /* parse the following strings: 2190:0190:0000:0000:0000:0000:0000:0000 */
    s = lua_tostring(L, -1);

    k = 0;
    n = 0;
    cur = 0;
    for (; *s && k < 16; s++)
    {
        if ((*s) >= '0' && (*s) <= '9')
        {
            cur = (cur << 4) + (*s) - '0';
        }
        else if ((*s) >= 'a' && (*s) <= 'f')
        {
            cur = (cur << 4) + (*s) - 'a' + 10;
        }
        else if ((*s) >= 'A' && (*s) <= 'F')
        {
            cur = (cur << 4) + (*s) - 'A' + 10;
        }
        else if (*s == ':')
        {
            n = 3;
        }
        else
        {
            break;
        }
        n++;
        if (n >= 4)
        {
            ip->arIP[k++] = (GT_U8)((cur >> 8) & 0x00ff);
            ip->arIP[k++] = (GT_U8)(cur & 0x00ff);
            cur = 0;
            n = 0;
            if (s[1] == ':')
                s++;
        }
    }
    if (k < 16 && n > 0)
    {
        ip->arIP[k++] = (GT_U8)((cur >> 8) & 0x00ff);
        ip->arIP[k++] = (GT_U8)(cur & 0x00ff);
    }
}

/*************************************************************************
* prv_c_to_lua_GT_IPV6ADDR
*
* Description:
*       Convert GT_IPV6ADDR to lua
*
* Parameters:
*
* Returns:
*       value at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_IPV6ADDR(
    lua_State *L,
    GT_IPV6ADDR *ip
)
{
    /* convert to string: 2190:0190:0000:0000:0000:0000:0000:0000 */
    char s[40];
    sprintf(s, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            ip->arIP[0],  ip->arIP[1],  ip->arIP[2],  ip->arIP[3],
            ip->arIP[4],  ip->arIP[5],  ip->arIP[6],  ip->arIP[7],
            ip->arIP[8],  ip->arIP[9],  ip->arIP[10], ip->arIP[11],
            ip->arIP[12], ip->arIP[13], ip->arIP[14], ip->arIP[15]);
    lua_pushstring(L, s);
}

add_mgm_type(GT_IPV6ADDR)
