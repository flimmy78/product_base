/*************************************************************************
* cmdLuaCommon.c
*
* DESCRIPTION:
*       Common function for cmdLua
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/os/cmdStream.h>
#include <cmdShell/os/cmdExecute.h>

#ifndef ALU_LUA
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
# include <string.h>
#else /* ALU_LUA */

# define CMD_LUA_FS
# include "../lua-5.1/src/lua.h"
# include "../lua-5.1/src/lualib.h"
# include "../lua-5.1/src/lauxlib.h"

# define cmdOsMalloc    malloc
# define cmdOsFree      free
# define cmdOsMemCpy    memcpy
# define cmdOsMemCmp    memcmp
# define cmdOsMemSet    memset
# define cmdOsSprintf   sprintf

#endif

#include <cmdShell/lua/cmdLuaCommon.h>

/***** Externs  ********************************************************/
#ifdef  CMD_LUA_FS
int cmdLuaFSregisterLoader(lua_State *L);
int cmdLuaFSregisterAPI(lua_State *L);
#endif /* CMD_LUA_FS */

int GenWrapper_init(lua_State *L);

/***** Private declarations ********************************************/

/***** Public Functions ************************************************/

/*******************************************************************************
* luaPrint
*
* DESCRIPTION:
*       lua print function
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaPrint(
    lua_State *L
)
{
    cmdStreamPTR IOStream;
    int n;
    int i;

    lua_getglobal(L, "IOStream");
    IOStream = (cmdStreamPTR)lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (IOStream == NULL)
        return 0;

    n=lua_gettop(L);
    for (i=1; i<=n; i++)
    {
        if (i>1)
            IOStream->writeLine(IOStream, "\t");
        if (lua_isstring(L,i))
            IOStream->writeLine(IOStream, lua_tostring(L,i));
        else if (lua_isnil(L,i))
            IOStream->writeLine(IOStream, "nil");
        else if (lua_isboolean(L,i))
            IOStream->writeLine(IOStream, lua_toboolean(L,i) ? "true" : "false");
        else
        {
            /* printf("%s:%p",luaL_typename(L,i),lua_topointer(L,i)); */
            IOStream->writeLine(IOStream, "<");
            IOStream->writeLine(IOStream, luaL_typename(L,i));
            IOStream->writeLine(IOStream, ">");
        }
    }
    IOStream->writeLine(IOStream, "\n");
    return 0;
}

/*******************************************************************************
* luaCLI_splitline
*
* DESCRIPTION:
*       execute command
*
* INPUTS:
*       L       - lua_State
*        1st    - line
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       table of words
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_splitline(lua_State *L)
{
    int was_space = 1;
    int s, p;
    const char *line;
    int linelen;
    if (lua_type(L, 1) != LUA_TSTRING)
    {
        return 0;
    }
    lua_settop(L, 1);
    line = lua_tostring(L, 1);
    linelen = lua_objlen(L, 1);
    lua_newtable(L); /* 2 */
    for (s = p = 0; p < linelen; p++)
    {
        int is_space = 0;
        switch(line[p])
        {
            case 0:
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '\v':
                is_space = 1;
                break;

        }
        if (was_space == is_space)
            continue;
        if (was_space == 1)
        {
            s = p;
        }
        else
        {
            lua_pushlstring(L, line + s, p - s);
            lua_rawseti(L, 2, lua_objlen(L, 2) + 1);
        }
        was_space = is_space;
    }
    if (was_space == 0 && s < p)
    {
        lua_pushlstring(L, line + s, p - s);
        lua_rawseti(L, 2, lua_objlen(L, 2) + 1);
    }
    return 1;
}

/*******************************************************************************
* luaCLI_registerCfunction
*
* DESCRIPTION:
*       Add commands
*
* INPUTS:
*       L       - lua_State
*        1st    - C function name
*        2nd    - lua prefix (optional)
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_registerCfunction(lua_State *L)
{
    lua_CFunction funcPtr;
    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_registerCfunction() error: "
            "first parameter must be string");
        lua_call(L, 1, 0);
        return 0;
    }
    if (lua_gettop(L) > 1)
    {
        lua_settop(L, 2);
        if (!lua_isstring(L, 2))
        {
            lua_getglobal(L, "print");
            lua_pushstring(L, "luaCLI_registerCfunction() error: "
                "second parameter must be string");
            lua_call(L, 1, 0);
            return 0;
        }
    }
    else
    {
        lua_pushstring(L, "");
    }
    lua_pushvalue(L, 1);
    lua_concat(L, 2);

    funcPtr = (lua_CFunction) osShellGetFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_registerCfunction() error: "
            "symbol not found: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        lua_call(L, 1, 0);
        return 0;
    }
    lua_register(L, lua_tostring(L, 2), funcPtr);
    return 0;
}

/*******************************************************************************
* luaCLI_callCfunction
*
* DESCRIPTION:
*       Call Lua C function without registering
*
* INPUTS:
*       L       - lua_State
*        1st    - C function name
*        ...
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_callCfunction(lua_State *L)
{
    lua_CFunction funcPtr;
    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_callCfunction() error: "
            "first parameter must be string");
        lua_call(L, 1, 0);
        return 0;
    }

    funcPtr = (lua_CFunction) osShellGetFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_callCfunction() error: "
            "symbol not found: ");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        lua_call(L, 1, 0);
        return 0;
    }
    lua_pushcfunction(L, funcPtr);
    lua_replace(L, 1);
    lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
    return lua_gettop(L);
}

/*******************************************************************************
* luaCLI_require
*
* DESCRIPTION:
*       Builtin command: require
*
* INPUTS:
*       L       - lua_State
*        1st    - module name
*
* OUTPUTS:
*       luaPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int luaCLI_require(lua_State *L)
{
    if (lua_istable(L, 1))
    {
        lua_rawgeti(L, 1, 1);
        lua_replace(L, 1);
    }
    if (!lua_isstring(L, 1))
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "luaCLI_require() error: "
            "parameter must be a string");
        lua_call(L, 1, 0);
        return 0;
    }
    /* load libLuaCliDefs.lua */
    lua_getglobal(L, "require");
    lua_pushvalue(L, 1);
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        lua_getglobal(L, "print");
        lua_pushstring(L, "Failed to load ");
        lua_pushvalue(L, 1);
        lua_pushstring(L, ": ");
        lua_pushvalue(L, -5);
        lua_concat(L, 4);
        lua_call(L, 1, 0);
    }
    return 0;
}

/*******************************************************************************
* luaCLI_alloc_wrapper
*
* DESCRIPTION:
*       The type of the memory-allocation function used by Lua states.
*       This allocator function provides a functionality similar to realloc,
*       but not exactly the same
*
* INPUTS:
*       ud      - user defined pointer, ignored
*       ptr     - a pointer to the block being allocated/reallocated/freed
*       osize   - the original size of the block
*       nsize   - the new size of the block
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to allocated/reallocated block
*
* COMMENTS:
*       (From Lua documentation)
*       ptr is NULL if and only if osize is zero.
*       When nsize is zero, the allocator must return NULL;
*       if osize is not zero, it should free the block pointed to by ptr.
*       When nsize is not zero, the allocator returns NULL if and only if
*         it cannot fill the request.
*       When nsize is not zero and osize is zero, the allocator should
*         behave like malloc.
*       When nsize and osize are not zero, the allocator behaves like realloc.
*       Lua assumes that the allocator never fails when osize >= nsize
*
*******************************************************************************/
static void* luaCLI_alloc_wrapper
(
    void    *ud,
    void    *ptr,
    size_t  osize,
    size_t  nsize
)
{
    void    *nptr;
#ifdef LUACLI_MEMSTAT
    LUACLI_MEM_STAT_STC *memStat = (LUACLI_MEM_STAT_STC*)ud;
#endif /* LUACLI_MEMSTAT */
    if (nsize == 0)
    {
        if (ptr != NULL)
        {
            cmdOsFree(ptr);
#ifdef LUACLI_MEMSTAT
            if (memStat)
            {
                memStat->totalUsed -= osize;
                memStat->nChunks--;
            }
#endif /* LUACLI_MEMSTAT */
        }
        return NULL;
    }

    nptr = cmdOsMalloc(nsize);
    if (nptr == NULL && osize >= nsize)
    {
        return ptr;
    }
#ifdef LUACLI_MEMSTAT
    if (memStat)
    {
        memStat->totalUsed += nsize;
        memStat->nChunks++;
    }
#endif /* LUACLI_MEMSTAT */
    if (nptr != NULL && ptr != NULL)
    {
        cmdOsMemCpy(nptr, ptr, (osize < nsize) ? osize : nsize);
    }
    if (ptr != NULL)
    {
        cmdOsFree(ptr);
#ifdef LUACLI_MEMSTAT
        if (memStat)
        {
            memStat->totalUsed -= osize;
            memStat->nChunks--;
        }
#endif /* LUACLI_MEMSTAT */
    }
    return nptr;
}

/*******************************************************************************
* cmdLua_newstate
*
* DESCRIPTION:
*       Init new Lua state, enable minimal functionality
*       Should be used to create luaCLI and for new Lua task
*
* INPUTS:
*       memstat     - pointer to memstat structure or NULL
*
* OUTPUTS:
*       None
*
* RETURNS:
*       luaPtr
*
* COMMENTS:
*       None
*
*******************************************************************************/
lua_State* cmdLua_newstate
(
    void *ud
)
{
    lua_State *L;

    L = lua_newstate(luaCLI_alloc_wrapper, ud);

    luaL_openlibs(L);
    /* register(override) print function */
    lua_register(L, "print", luaPrint);
    lua_register(L, "splitline", luaCLI_splitline);
    lua_register(L, "cmdLuaCLI_require", luaCLI_require);
    lua_register(L, "cmdLuaCLI_registerCfunction", luaCLI_registerCfunction);
    lua_register(L, "cmdLuaCLI_callCfunction", luaCLI_callCfunction);

    /* initialize C wrappers */
#ifdef  CMD_LUA_GALTIS
    lua_register(L, "cmdLuaRunGaltisWrapper", cmdLuaRunGaltisWrapper);
#endif /* CMD_LUA_GALTIS */

#ifdef  CMD_LUA_FS
    cmdLuaFSregisterLoader(L);
    cmdLuaFSregisterAPI(L);
#endif /* CMD_LUA_FS */

    /* register generic wrapper and generic types */
    lua_pushcfunction(L, GenWrapper_init);
    lua_call(L, 0, 0);

    return L;
}
