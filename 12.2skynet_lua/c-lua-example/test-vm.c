#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static void
call_func_0(lua_State *L, const char* funcname)
{
    lua_getglobal(L, funcname);
    lua_pushinteger(L, 1);
    lua_call(L, 1, 0);
}

int main(int argc, char** argv)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (argc > 1) {

        lua_pushboolean(L, 1);
        lua_setfield(L, LUA_REGISTRYINDEX, "LUA_NOENV");

        if ( LUA_OK != luaL_dofile(L, argv[1]) ) {
            const char* err = lua_tostring(L, -1);
            fprintf(stderr, "err:\t%s\n", err);
            return 1;
        }

        call_func_0(L, "Init");
        call_func_0(L, "Loop");
        call_func_0(L, "Release");
        lua_close(L);
    }

    return 0;
}
