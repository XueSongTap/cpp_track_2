package.cpath = "luaclib/?.so"

local so = require "ud.c"

local ud = so.new()

ud:echo("hello world1")
ud:again()
ud:echo("hello world2")
ud:again()
ud:echo("hello world3")
ud:again()
ud:echo("hello world4")
ud:again()
ud:echo("hello world5")
ud:again()
ud:echo("hello world6")
ud:again()
ud:echo("hello world7")
ud:again()
ud:echo("hello world8")
ud:again()
ud:echo("hello world9")
ud:again()

--./lua/src/lua test-ud.lua