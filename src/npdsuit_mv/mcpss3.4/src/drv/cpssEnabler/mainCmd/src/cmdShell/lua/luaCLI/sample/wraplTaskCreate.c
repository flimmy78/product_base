/*************************************************************************
* wraplTaskCreate.c
*
* DESCRIPTION:
*       luaTaskCreate() implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
#include <gtOs/gtOsTask.h>
#include <lua.h>
#include <lauxlib.h>

#include <string.h>

#include <cmdShell/lua/cmdLuaCommon.h>

/************* Externs ********************************************************/

/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Functions ******************************************************/

/*******************************************************************************
* lua_copy
*
* DESCRIPTION:
*       Copy top value from top of L1 to L2
*
* INPUTS:
*       L1
*       L2
*
* RETURNS:
*       0 if ok
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int lua_copy(lua_State* L1, lua_State* L2)
{
    switch (lua_type(L1, -1))
    {
        case LUA_TNIL:
            lua_pushnil(L2);
            return 0;
        case LUA_TBOOLEAN:
            lua_pushboolean(L2, lua_toboolean(L1, -1));
            return 0;
        case LUA_TLIGHTUSERDATA:
            lua_pushlightuserdata(L2, lua_touserdata(L1, -1));
            return 0;
        case LUA_TNUMBER:
            lua_pushnumber(L2, lua_tonumber(L1, -1));
            return 0;
        case LUA_TSTRING:
            {
                const char *str;
                size_t len = 0;
                str = lua_tolstring(L1, -1, &len);
                lua_pushlstring(L2, str, len);
            }
            return 0;
        case LUA_TUSERDATA:
            lua_newuserdata(L2, lua_objlen(L1, -1));
            memcpy(lua_touserdata(L2,-1), lua_touserdata(L1, -1), lua_objlen(L1, -1));
            return 0;
        case LUA_TFUNCTION:
            if (lua_iscfunction(L1, -1))
            {
                lua_pushcfunction(L2, lua_tocfunction(L1, -1));
                return 0;
            }
            return 1;
        case LUA_TTABLE:
            break;
        default:
            return 1;
    }
    /* table */
    lua_newtable(L2);
    lua_pushnil(L1); /* first key */
    while (lua_next(L1, -2))
    {
        /* L1: tbl key value */
        if (lua_copy(L1, L2))
        {
            lua_pop(L1, 2);
            lua_pop(L2, 1);
            return 1;
        }
        /* L2: tbl value */

        lua_pop(L1, 1); /* remove value, keep key for next iteration */
        if (lua_copy(L1, L2))
        {
            lua_pop(L1, 1);
            lua_pop(L2, 2);
            return 1;
        }
        /* L2: tbl value key */
        lua_insert(L2, -2); /* L2: tbl key value */
        lua_settable(L2, -3);
    }

    return 0;
}

/*******************************************************************************
* luaTaskWrapper
*
* DESCRIPTION:
*       task wrapper
*
* INPUTS:
*       L       - Lua state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID __TASKCONV luaTaskWrapper
(
    lua_State* L
)
{
    /* run loaded script */
    if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
    {
        /*TODO: handle error */
    }
    lua_close(L);
}

/*******************************************************************************
* luaTaskCreate: lua function
*
* DESCRIPTION:
*       Create a new lua task
*
* INPUTS:
*       name        - task name
*       script_name - script to load
*       param       - (optional) parameter to script
*                     will be set as task_Param
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTaskCreate
(
    lua_State *L
)
{
    lua_State   *L2;
    int         ret;
    GT_STATUS   status;
    GT_TASK     taskId;

    /* check params */
    if (lua_gettop(L) < 2)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushliteral(L, "bad parameter");
        return 1;
    }

    /* create new state for task */
    L2 = cmdLua_newstate(NULL);

    /* copy task name to L2 */
    lua_pushstring(L2, lua_tostring(L, 1));
    lua_setglobal(L2, "task_Name");
    /* copy script name to L2 */
    lua_pushstring(L2, lua_tostring(L, 2));
    lua_setglobal(L2, "task_Script");
    /* copy param to L2 */
    if (lua_gettop(L) > 2)
    {
        lua_settop(L, 3);
        if (lua_copy(L, L2) != 0)
        {
            lua_pushliteral(L, "failed to copy task param, wrong data type (function, thread)");
            lua_close(L2);
            return 1;
        }
        lua_setglobal(L2, "task_Param");
    }

    /* load script to new state, don't run it now */
    ret = luaL_loadfile(L2, lua_tostring(L, 2));
    if (ret != 0)
    {
        lua_pushfstring(L, "luaL_loadfile(\"%s\"): ", lua_tostring(L, 2));
        switch (ret)
        {
            case LUA_ERRSYNTAX:
                lua_pushliteral(L, "syntax error");
                break;
            case LUA_ERRMEM:
                lua_pushliteral(L, "memory allocation error");
                break;
            case LUA_ERRFILE:
                lua_pushliteral(L, "cannot open/read file");
                break;
            default:
                lua_pushfstring(L, "unknown error: %d", ret);
        }
        lua_concat(L, 2);
        lua_close(L2);
        return 1;
    }

    status = osTaskCreate(
        (char*)lua_tostring(L, 1), /* name */
        5, /* prio */
        32768, /* stack */
        (unsigned (__TASKCONV *)(void*))luaTaskWrapper, /* entry point */
        L2, /* arg */
        &taskId);

    lua_pushnumber(L, status);
    return 1;
}

/*TODO: remove after test done */
#define TEST
#ifdef TEST
/***********************************/
/***** TEST ************************/
/***********************************/
#include <gtOs/gtOsMsgQ.h>

static GT_VOID __TASKCONV cTestTask
(
    GT_MSGQ_ID msgqId
)
{
    GT_U32  msg;
    msg = 201;
    osMsgQSend(msgqId, &msg, sizeof(msg), OS_MSGQ_WAIT_FOREVER);
    msg = 202;
    osMsgQSend(msgqId, &msg, sizeof(msg), OS_MSGQ_WAIT_FOREVER);
}

/*******************************************************************************
* luaTaskCreate: lua function
*
* DESCRIPTION:
*       Create a new C task with msgqId passed as parameter
*
* INPUTS:
*       name        - task name
*       script_name - script to load
*       param
*
* RETURNS:
*       status
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaTestTaskCreate
(
    lua_State *L
)
{
    GT_MSGQ_ID  msgqId;
    GT_TASK     taskId;
    if (lua_gettop(L) < 1)
    {
        lua_getglobal(L, "print");
        lua_pushliteral(L, "luaTestTaskCreate(): no required parameter");
        lua_call(L, 1, 0);
        return 0;
    }
    if (!lua_istable(L, 1))
    {
bad_param:
        lua_getglobal(L, "print");
        lua_pushliteral(L, "luaTestTaskCreate(): wrong parameter");
        lua_call(L, 1, 0);
        return 0;
    }
    lua_getfield(L, 1, "msgqId");
    if (!lua_isnumber(L, -1))
        goto bad_param;
    msgqId = (GT_MSGQ_ID)lua_tonumber(L, -1);

    osTaskCreate(
        "ctest", /* name */
        5, /* prio */
        32768, /* stack */
        (unsigned (__TASKCONV *)(void*))cTestTask, /* entry point */
        (void*)msgqId, /* arg */
        &taskId);
    return 0;
}
#endif /* TEST */
