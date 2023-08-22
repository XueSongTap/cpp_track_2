package.cpath = "luaclib/?.so" --c库的路径

local so = require "tbl.c"

so.echo("hello world") -- 新的虚拟栈
so.echo("hello world1")-- 新的虚拟栈
so.echo("hello world2")-- 新的虚拟栈


--[[
    1. c调用lua  c有多个协程 每个协程一个虚拟栈
    2. lua调用c  每次调用都有一个虚拟栈
]]
