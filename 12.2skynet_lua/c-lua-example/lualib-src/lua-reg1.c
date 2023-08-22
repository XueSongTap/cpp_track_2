#include <lua.h>

#include <lauxlib.h>

#include <lualib.h>

#include <stdio.h>

static int
lecho (lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "mk.reg.c"); 
    // 此时栈顶放了一张表如下
    //        {["mk.reg.c"] = {1000, 2000}}
    lua_rawgeti(L, -1, 1);
    // -1 是读取栈顶的表  1 是指读这个表中索引为1 的field
    // 相当于 取出 1000 放在栈顶
    // 此时 栈上 1 的位置 为 table   2的位置为 1000
    lua_Integer n = lua_tointeger(L, -1);
    // 将栈顶的值转化为整数
    n++;
    lua_pop(L, 1);
    // 将栈顶的值 pop 出
    // 此时栈上只有一个 table
    lua_pushinteger(L, n);
    // 将运算后的 n push 到栈上
    // 此时栈上 1 的位置为 table   2的位置为 1001
    lua_rawseti(L, -2, 1);
    // 将 栈顶的值 设置到 1 位置中table 索引为 1的位置
    // 此时 table = {1001,2000}
    // 此时栈上只有一个 table 因为设置的时候 把栈顶的值丢掉了
    const char* str = lua_tostring(L, 1);
    fprintf(stdout, "reg1[n=%lld]----%s\n", n, str);
    return 0;
}

static const luaL_Reg l[] = {
	{"echo", lecho},
	{NULL, NULL},
};

int
luaopen_reg1_c(lua_State *L) { // local tbl = require "tbl.c"
    // 创建一张新的表，并预分配足够保存下数组 l 内容的空间
	// luaL_newlibtable(L, l);
	// luaL_setfuncs(L, l, 0); {[mk.reg.c] = {1000,2000}}
    // {
//        ["mk.reg.c"] = {1000, 2000}
    // }
    if (lua_getfield(L, LUA_REGISTRYINDEX, "mk.reg.c") == LUA_TNIL) {
        lua_createtable(L, 2, 0); //1
        lua_pushinteger(L, 1000); //2
        lua_rawseti(L, -2, 1); //1   {1000}
        lua_pushinteger(L, 2000);//2
        lua_rawseti(L, -2, 2);// 1  [mk.reg.c] = {1000,2000}
        lua_setfield(L, LUA_REGISTRYINDEX, "mk.reg.c");
        fprintf(stdout, "luaopen_reg1_c 注册表 mk.reg.c\n");
    }
    luaL_newlib(L, l);
	return 1;
}
