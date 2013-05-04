/*************************************************************************
* genwrapper_API.c
*
* DESCRIPTION:
*       a generic wrapper to CPSS APIs
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cmdShell/os/cmdExecute.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <lua.h>

#include <cmdShell/lua/genwrapper_API.h>

#define MAX_PARAMS 20

typedef enum
{
    CPSS_API_PARAMETER_TYPE_IN,
    CPSS_API_PARAMETER_TYPE_OUT,
    CPSS_API_PARAMETER_TYPE_INOUT,
    CPSS_API_PARAMETER_TYPE_WRONG
} CPSS_API_PARAMETER_TYPE;


/*************************************************************************
* mgmType_to_c_int
* mgmType_to_c_GT_U8
* mgmType_to_c_GT_U16
* mgmType_to_c_GT_U32
*
* Description:
*       Convert to integer/GT_U8/GT_U16/GT_U32
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_c_int(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        lua_pushnumber(L, 0);
    else if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_int
* mgmType_to_lua_GT_U8
* mgmType_to_lua_GT_U16
* mgmType_to_lua_GT_U32
*
* Description:
*       Convert integer/GT_U8/GT_U16/GT_U32 to number
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_lua_int(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_c_bool
* mgmType_to_c_GT_BOOL
*
* Description:
*       Convert to integer(GT_BOOL)
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_c_bool(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        lua_pushnumber(L, 0);
    else if (lua_isboolean(L, 1))
        lua_pushnumber(L, lua_toboolean(L, 1));
    else if (lua_isnumber(L, 1))
        lua_pushnumber(L, lua_tointeger(L, 1) == 0 ? 0 : 1);
    else
        lua_pushnumber(L, 0);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_bool
* mgmType_to_lua_GT_BOOL
*
* Description:
*       Convert integer(GT_BOOL) to boolean
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_lua_bool(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushboolean(L, lua_tointeger(L, 1) == 0 ? 0 : 1);
    return 1;
}

/*************************************************************************
* interpretParameterType
*
* Description:
*       Convert parameter type ("IN", "OUT", "INOUT") to enum
*
* Parameters:
*       string at the top of
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static CPSS_API_PARAMETER_TYPE interpretParameterType(lua_State *L)
{
    lua_pushliteral(L, "IN");
    if (lua_equal(L, -1, -2))
    {
        lua_pop(L, 2);
        return CPSS_API_PARAMETER_TYPE_IN;
    }
    lua_pop(L, 1);
    lua_pushliteral(L, "OUT");
    if (lua_equal(L, -1, -2))
    {
        lua_pop(L, 2);
        return CPSS_API_PARAMETER_TYPE_OUT;
    }
    lua_pushliteral(L, "INOUT");
    if (lua_equal(L, -1, -2))
    {
        lua_pop(L, 2);
        return CPSS_API_PARAMETER_TYPE_INOUT;
    }
    lua_pop(L, 2);
    return CPSS_API_PARAMETER_TYPE_WRONG;
}
/*************************************************************************
* cpssGenWrapper
*
* Description:
*       Call CPSS API function
*
* Parameters:
*  cpssAPIname     - Fx: "prvCpssDrvHwPpReadRegister"
*  params          - table:
*                    each row is table of 3 elements:
*                    1. direction (IN | OUT | INOUT)
*                    2. parameter type
*                       the valid types:
*                       "int"
*                       "bool"
*                       "GT_U8",
*                       "GT_U16",
*                       "GT_U32",
*                       "string"
*                       "custom_type_name" with a set of functions:
*                          mgmType_to_c_${custom_type_name}(param)
*                                  - convert lua variable/table to
*                                    userdata object
*                          mgmType_to_lua_${custom_type_name}(param)
*                                  - convert userdata object to lua
*                    3. parameter name
*                    4. parameter value(optional, for IN and INOUT only)
*
*
* Returns:
*  GT_STATUS, array of output values if successful
*  string if error
*
*
* Example
* status, values = cpssGenWrapper(
*                      "prvCpssDrvHwPpReadRegister", {
*                           { "IN", "GT_U8", "devNum", 0 },
*                           { "IN", "GT_U32","regAddr", 0x200000 },
*                           { "OUT","GT_U32","regData" }
*                      })
* regData = values.regData
*************************************************************************/
static int cpssGenWrapper(
    lua_State *L
)
{
    GT_U32                  paramsArray[MAX_PARAMS];
    GT_U32                  valuesArray[MAX_PARAMS];
    CPSS_API_PARAMETER_TYPE ptypesArray[MAX_PARAMS];
    int                     vtypesArray[MAX_PARAMS];
    GT_STATUS retval;
    typedef GT_STATUS (*CPSS_API_FUNC_PTR)(GT_U32, ...);
    CPSS_API_FUNC_PTR funcPtr;
    int nparams;
    int k,values;

    if (lua_gettop(L) < 2)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    if (!lua_istable(L, 2))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }

    lua_settop(L, 2); /* remove extra parameters */

    funcPtr = (CPSS_API_FUNC_PTR) osShellGetFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_pushliteral(L, "function '");
        lua_pushvalue(L, 1);
        lua_pushliteral(L, "' not found");
        lua_concat(L, 2);
        return 1;
    }

    /* prepare parameters */
    nparams = lua_objlen(L, 2);
    for (k = 0; k < nparams; k++)
    {
        int param;
        lua_rawgeti(L, 2, k+1); /* params[k+1] */
        param = lua_gettop(L); /* param = params[k+1] */

        lua_rawgeti(L, param, 1); /* params[k+1][1] */
        ptypesArray[k] = interpretParameterType(L);
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_WRONG)
        {
            lua_pushfstring(L, "bad parameter type for parameter %d, must be IN|OUT|INOUT", k);
            return 1;
        }
        paramsArray[k] = 0;
        valuesArray[k] = 0;
        vtypesArray[k] = 0;
        /* convert lua value before call (allocate ram for output) */
        lua_pushliteral(L, "mgmType_to_c_");
        lua_rawgeti(L, param, 2);
        if (!lua_isstring(L, -1))
        {
            lua_pushfstring(L, "no type specified for parameter %d", k+1);
            return 1;
        }
        lua_concat(L, 2);
        lua_getglobal(L, lua_tostring(L, -1));
        if (!lua_isfunction(L, -1))
        {
            lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -2));
            return 1;
        }
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_OUT)
        {
            lua_pushnil(L);
        }
        else
        {
            lua_rawgeti(L, param, 4);
        }
        if (lua_pcall(L, 1, 1, 0))
        {
            lua_pushfstring(L, "error while executing %s: ", lua_tostring(L, -2));
            lua_insert(L, -2);
            lua_concat(L, 2);
            return 2;
        }
        /* remove function name */
        lua_remove(L, -2);
        if (lua_isnil(L, -1))
        {
            lua_pushfstring(L, "the parameter %d cannot be converted to required type", k+1);
            return 1;
        }

        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_IN)
        {
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                paramsArray[k] = (GT_U32)lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            else if (lua_type(L, -1) == LUA_TUSERDATA)
            {
                lua_insert(L, -2);
                paramsArray[k] = (GT_U32)(lua_touserdata(L, -2));
                /* don't free object here */
            }
            else
            /* handle strings */
            {
                lua_pushfstring(L, "p1: not supported param %d", k+1);
                return 1;
            }
        }
        else /* ptypesArray[k] == OUT || INOUT */
        {
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                paramsArray[k] = (GT_U32)(&(valuesArray[k]));
                valuesArray[k] = (GT_U32)lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            else if (lua_type(L, -1) == LUA_TUSERDATA)
            {
                lua_insert(L, -2);
                paramsArray[k] = (GT_U32)(lua_touserdata(L, -2));
                valuesArray[k] = lua_gettop(L) - 1;
                vtypesArray[k] = 1;
                /* don't free object here */
            }
            else
            {
                lua_pushliteral(L, "p2: not supported");
                return 1;
            }
        }
        lua_pop(L, 1); /* params[k+1] */
    }

    /* call function */
    retval = funcPtr(
        paramsArray[0],
        paramsArray[1],
        paramsArray[2],
        paramsArray[3],
        paramsArray[4],
        paramsArray[5],
        paramsArray[6],
        paramsArray[7],
        paramsArray[8],
        paramsArray[9]);

    /* push results */
    lua_pushnumber(L, retval);
    lua_newtable(L);
    values = lua_gettop(L);

    for (k = 0; k < nparams; k++)
    {
        int param;
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_IN)
            continue;

        lua_rawgeti(L, 2, k+1); /* params[k+1] */
        param = lua_gettop(L); /* param = params[k+1] */

        /* convert return values to lua */
        lua_pushliteral(L, "mgmType_to_lua_");
        lua_rawgeti(L, param, 2);
        lua_concat(L, 2);
        lua_getglobal(L, lua_tostring(L, -1));
        if (!lua_isfunction(L, -1))
        {
            lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -2));
            return 1;
        }
        if (vtypesArray[k] == 0)
        {
            lua_pushnumber(L, valuesArray[k]);
        }
        else /* vtypesArray[nparams] == 1 */
        {
            lua_pushvalue(L, valuesArray[k]); /* previosly stored index */
        }
        /*TODO*/
        if (lua_pcall(L, 1, 1, 0))
        {
            lua_pushfstring(L, "error while executing %s: ", lua_tostring(L, -2));
            lua_insert(L, -2);
            lua_concat(L, 2);
            return 2;
        }
        /* remove function name */
        lua_remove(L, -2);

        lua_rawgeti(L, param, 3);
        lua_insert(L, -2);
        lua_setfield(L, values, lua_tostring(L, -2));
        lua_pop(L, 1); /* params[k+1][3] */

        lua_pop(L, 1); /* params[k+1] */
    }
    return 2;
}


/*************************************************************************
* prv_lua_to_c_enum
*
* Description:
*       Convert lua value to enum
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       integer
*
*************************************************************************/
int prv_lua_to_c_enum(
    lua_State   *L,
    enumDescr   *descr
)
{
    if (lua_isnumber(L, -1))
        return lua_tointeger(L, -1);
    if (!descr)
        return 0;
    if (lua_isstring(L, -1))
    {
        const char *s = lua_tostring(L, -1);
        while(descr->string)
        {
            if (cpssOsStrCmp(s, descr->string) == 0)
                return descr->enumValue;
            descr++;
        }
    }
    return 0;
}

/*************************************************************************
* prv_c_to_lua_enum
*
* Description:
*       Convert enum to lua
*
* Parameters:
*       cmd
*
* Returns:
*       string at the top of stack
*
*************************************************************************/
void prv_c_to_lua_enum(
    lua_State   *L,
    enumDescr   *descr,
    int         enumValue
)
{
    if (descr)
    {
        for(; descr->string; descr++)
        {
            if (enumValue == descr->enumValue)
            {
                lua_pushstring(L, descr->string);
                return;
            }
        }
    }
    lua_pushnil(L);
}


/*************************************************************************
* prv_lua_to_c_GT_U64
*
* Description:
*       Convert to "c" GT_U64
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_GT_U64(
    lua_State *L,
    GT_U64  *val
)
{
    F_ARRAY_START(val, -1, l);
        F_ARRAY_NUMBER(val, l, 0, GT_U32);
        F_ARRAY_NUMBER(val, l, 1, GT_U32);
    F_ARRAY_END(val, -1, l);
}

/*************************************************************************
* prv_c_to_lua_GT_U64
*
* Description:
*       Convert GT_U64 to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_U64(
    lua_State *L,
    GT_U64 *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, l);
        FO_ARRAY_NUMBER(val, l, 0, GT_U32);
        FO_ARRAY_NUMBER(val, l, 1, GT_U32);
    FO_ARRAY_END(val, t, l);
}

add_mgm_type(GT_U64);



int GenWrapper_init(lua_State *L)
{
    lua_register(L, "cpssGenWrapper", cpssGenWrapper);
    lua_register(L, "mgmType_to_c_int", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U8", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U16", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U32", mgmType_to_c_int);
    lua_register(L, "mgmType_to_lua_int", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U8", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U16", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U32", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_c_bool", mgmType_to_c_bool);
    lua_register(L, "mgmType_to_c_GT_BOOL", mgmType_to_c_bool);
    lua_register(L, "mgmType_to_lua_bool", mgmType_to_lua_bool);
    lua_register(L, "mgmType_to_lua_GT_BOOL", mgmType_to_lua_bool);
    lua_register(L, "mgmType_to_c_GT_U64", mgmType_to_c_GT_U64);
    lua_register(L, "mgmType_to_lua_GT_U64", mgmType_to_lua_GT_U64);
    return 0;
}
