#include <lua.h>

#include <lauxlib.h>

#include <lualib.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

struct log {
    int count;
};

static int
lagain(lua_State *L) {
    struct log *p = (struct log *)luaL_checkudata(L, 1, "mk.ud.log");
    lua_getuservalue(L, -1);
    const char* str = lua_tostring(L, -1);
    fprintf(stdout, "ud[n=%d]----%s\n", p->count, str);
    return 0;
}

static int
lecho(lua_State *L) {
    struct log *p = (struct log *)luaL_checkudata(L, 1, "mk.ud.log");
    const char* str = lua_tostring(L, -1);
    p->count++;
    lua_setuservalue(L, -2);
    fprintf(stdout, "ud[n=%d]----%s\n", p->count, str);
	return 0;
}

static int
lnew (lua_State *L) {
    struct log *q = (struct log*)lua_newuserdata(L, sizeof(struct log));
    q->count = 0;
    lua_pushstring(L, "");
    lua_setuservalue(L, -2);
	if (luaL_newmetatable(L, "mk.ud.log")) {
        luaL_Reg m[] = {
			{"echo", lecho},
            {"again", lagain},
			{NULL, NULL},
		};
		luaL_newlib(L, m);
		lua_setfield(L, -2, "__index");
        lua_setmetatable(L, -2);
    }
    return 1;
}

static const luaL_Reg l[] = {
	{"new", lnew},
	{NULL, NULL},
};

int
luaopen_ud_c(lua_State *L) {
    luaL_newlib(L, l);
	return 1;
}
