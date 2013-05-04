/*************************************************************************
* cmdLuaBitwizeFunctions.c
*
* DESCRIPTION:
*       Lua bitwize operations 
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
**************************************************************************/
#include <lua.h>

#define OPERATOR(name,operation) \
static int bitwize ## name( \
    lua_State *L \
) \
{ \
    int a, b; \
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) \
        return 0; \
    a = (int)lua_tonumber(L, 1); \
    b = (int)lua_tonumber(L, 2); \
    lua_pushnumber(L, operation); \
    return 1; \
}


OPERATOR(And, a & b);
OPERATOR(Or, a | b);
OPERATOR(Xor, a ^ b);
OPERATOR(SHL, ((b < 32) ? a << b : 0));
OPERATOR(SHR, ((b < 32) ? a >> b : 0));

static int bitwizeNot(
    lua_State *L
)
{
    /* check parameters */
    int a;
    if (!lua_isnumber(L, 1))
    {
        /* return nothing */
        return 0;
    }
    a = (int)lua_tonumber(L, 1);
    lua_pushnumber(L, ~a);
    return 1;
}

int registerBitwizeFunctions(
    lua_State *L
)
{
    lua_register(L, "bit_and", bitwizeAnd);
    lua_register(L, "bit_or" , bitwizeOr);
    lua_register(L, "bit_not", bitwizeNot);
    lua_register(L, "bit_xor", bitwizeXor);
    lua_register(L, "bit_shl", bitwizeSHL);
    lua_register(L, "bit_shr", bitwizeSHR);
    return 0;
}

/* provides link for the file */
int check_cmdLuaBitwizeFunctions()
{
    return 1;
}

