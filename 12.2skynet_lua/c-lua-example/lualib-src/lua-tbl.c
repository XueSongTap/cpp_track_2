#include <lua.h>

#include <lauxlib.h>

#include <lualib.h>

#include <stdio.h>

static int
lecho (lua_State *L) {
    const char* str = lua_tostring(L, -1);
    fprintf(stdout, "%s\n", str);
    return 0;
}

static const luaL_Reg l[] = {// 导出给lua使用数组
	{"echo", lecho},
	{NULL, NULL},
};

int
luaopen_tbl_c(lua_State *L) { // local tbl = require "tbl.c"
    // 创建一张新的表，并预分配足够保存下数组 l 内容的空间
	// luaL_newlibtable(L, l);
	// luaL_setfuncs(L, l, 0);
    luaL_newlib(L, l);
	return 1;
}
