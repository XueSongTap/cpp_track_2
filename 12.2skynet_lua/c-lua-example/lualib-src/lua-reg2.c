#include <lua.h>

#include <lauxlib.h>

#include <lualib.h>

#include <stdio.h>

static int
lecho (lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "mk.reg.c");
    lua_rawgeti(L, -1, 2);
    lua_Integer n = lua_tointeger(L, -1);
    n++;
    lua_pop(L, 1);
    lua_pushinteger(L, n);
    lua_rawseti(L, -2, 2);
    const char* str = lua_tostring(L, 1);
    fprintf(stdout, "reg2[n=%lld]----%s\n", n, str);
    return 0;
}

static const luaL_Reg l[] = {
	{"echo", lecho},
	{NULL, NULL},
};

int
luaopen_reg2_c(lua_State *L) { 
    // local tbl = require "tbl.c"
    // 创建一张新的表，并预分配足够保存下数组 l 内容的空间
	// luaL_newlibtable(L, l);
	// luaL_setfuncs(L, l, 0);
    if (lua_getfield(L, LUA_REGISTRYINDEX, "mk.reg.c") == LUA_TNIL) {
        lua_createtable(L, 2, 0);
        lua_pushinteger(L, 1000);
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, 2000);
        lua_rawseti(L, -2, 2);
        lua_setfield(L, LUA_REGISTRYINDEX, "mk.reg.c");
        fprintf(stdout, "luaopen_reg2_c 注册表 mk.reg.c\n");
    }
    luaL_newlib(L, l);
	return 1;
}
