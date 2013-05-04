/*************************************************************************
* genwrapper_API.h
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

#ifndef __genwrapper_API_h

/************************************************************************
* A helper macroses/functions for Lua <=> c structures convertation
************************************************************************/

/* enum support */
typedef struct {
    int         enumValue;
    const char *string;
} enumDescr;

#define ENUM_ENTRY(a) { a, #a }
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
);

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
);


/************************************************************************
*
* The usual case is a structure as member of another structure
* For this pursose we need two more functions for Lua <=> c conevrtation
*
* These functions doesn't allocate memory and don't use the whole
* userdata object.
*
* For these purpose a pair of functions win common API must be created:
*
* void prv_lua_to_c_XXX(lua_State *L, XXX *val);
* void prv_c_to_lua_XXX(lua_State *L, XXX *val);
*
* The function prv_lua_to_c_XXX convert Lua object at the top of stack
* to C structure of type XXX, writing data to val
*
* The function prv_c_to_lua_XXX convert structure XXX pointed by val
* to Lua object. This object keept at the top of stack
*
************************************************************************/


/* support macroses: Lua => C */
#define F_BOOL(structPtr, Lindex, field) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TBOOLEAN) \
    { \
        (structPtr)->field = lua_toboolean(L, -1) ? GT_TRUE : GT_FALSE; \
    } \
    else \
    { \
        (structPtr)->field = GT_FALSE; \
    } \
    lua_pop(L, 1);

#define F_NUMBER(structPtr, Lindex, field, fieldtype) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TNUMBER) \
    { \
        (structPtr)->field = (fieldtype)lua_tointeger(L, -1); \
    } \
    else \
    { \
        (structPtr)->field = (fieldtype)0; \
    } \
    lua_pop(L, 1);

#define F_ENUM(structPtr, Lindex, field, fieldtype) \
    lua_getfield(L, Lindex, #field); \
    (structPtr)->field = (fieldtype)prv_lua_to_c_enum(L, enumDescr_ ## fieldtype); \
    lua_pop(L, 1);

#define F_STRUCT(structPtr, Lindex, field, fieldtype) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TTABLE) \
    { \
        prv_lua_to_c_ ## fieldtype(L, &((structPtr)->field)); \
    } \
    lua_pop(L, 1);

#define F_PTR(structPtr, Lindex, field, fieldtype) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TLIGHTUSERDATA) \
    { \
        (structPtr)->field = (fieldtype)lua_touserdata(L, -1); \
    } \
    else \
    { \
        (structPtr)->field = (fieldtype)NULL; \
    } \
    lua_pop(L, 1);

#define F_ARRAY_START(structPtr, Lindex, field) \
    lua_getfield(L, Lindex, #field); \
    if (lua_type(L, -1) == LUA_TTABLE) \
    {

#define F_ARRAY_END(structPtr, Lindex, field) \
    } \
    lua_pop(L, 1);

#define F_ARRAY_NUMBER(structPtr, field, index, fieldtype) \
    lua_rawgeti(L, -1, index); \
    if (lua_type(L, -1) == LUA_TNUMBER) \
    { \
        (structPtr)->field[index] = (fieldtype)lua_tointeger(L, -1); \
    } \
    else \
    { \
        (structPtr)->field[index] = (fieldtype)0; \
    } \
    lua_pop(L, 1);

#define F_ARRAY_STRUCT(structPtr, field, index, fieldtype) \
    lua_rawgeti(L, -1, index); \
    if (lua_type(L, -1) == LUA_TTABLE) \
    { \
        prv_lua_to_c_ ## fieldtype(L, &((structPtr)->field[index])); \
    } \
    lua_pop(L, 1);


/* support macroses: C => Lua */
#define FO_BOOL(structPtr, Lindex, field) \
    lua_pushboolean(L, ((structPtr)->field == GT_TRUE) ? 1 : 0); \
    lua_setfield(L, Lindex, #field);

#define FO_NUMBER(structPtr, Lindex, field, fieldtype) \
    lua_pushnumber(L, (structPtr)->field); \
    lua_setfield(L, Lindex, #field);

#define FO_ENUM(structPtr, Lindex, field, fieldtype) \
    prv_c_to_lua_enum(L, enumDescr_ ## fieldtype, (structPtr)->field); \
    if (lua_isnil(L, -1)) \
        lua_pop(L, 1); \
    else \
        lua_setfield(L, Lindex, #field);

#define FO_STRUCT(structPtr, Lindex, field, fieldtype) \
    prv_c_to_lua_ ## fieldtype(L, &((structPtr)->field)); \
    lua_setfield(L, Lindex, #field);

#define FO_PTR(structPtr, Lindex, field, fieldtype) \
    lua_pushlightuserdata(L, (structPtr)->field); \
    lua_setfield(L, Lindex, #field);

#define FO_ARRAY_START(structPtr, Lindex, field) \
    lua_newtable(L);

#define FO_ARRAY_END(structPtr, Lindex, field) \
    lua_setfield(L, Lindex, #field);

#define FO_ARRAY_NUMBER(structPtr, field, index, fieldtype) \
    lua_pushnumber(L, (structPtr)->field[index]); \
    lua_rawseti(L, -2, index);

#define FO_ARRAY_STRUCT(structPtr, field, index, fieldtype) \
    prv_c_to_lua_ ## fieldtype(L, &((structPtr)->field[index])); \
    lua_rawseti(L, -2, index);


/* add
 *    mgmType_to_c_ ## typeName()
 *    mgmType_to_lua_ ## typeName()
 * functions assuming
 *    prv_c_to_lua_ ## typeName() and
 *    prv_lua_to_c_ ## typeName()
 * already created
 */
#define add_mgm_type(typeName) \
int mgmType_to_c_ ## typeName( \
    lua_State *L \
) \
{ \
    typeName *val; \
    lua_newuserdata(L, sizeof(typeName)); \
    if (lua_isnil(L, 1)) \
    { \
        return 1; \
    } \
 \
    val = (typeName*)lua_touserdata(L, -1); \
    lua_pushvalue(L, 1); \
    prv_lua_to_c_ ## typeName(L, val); \
    lua_pop(L, 1); \
    return 1; \
} \
 \
int mgmType_to_lua_ ## typeName( \
    lua_State *L \
) \
{ \
    typeName *val; \
    if (lua_isnil(L, 1)) \
        return 1; \
    if (!lua_isuserdata(L, 1)) \
    { \
        lua_pushnil(L); \
        return 1; \
    } \
 \
    val = (typeName*)lua_touserdata(L, 1); \
    prv_c_to_lua_ ## typeName(L, val); \
 \
    return 1; \
}



#endif /* __genwrapper_API_h */
