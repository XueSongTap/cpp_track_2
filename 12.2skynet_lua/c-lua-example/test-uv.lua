package.cpath = "luaclib/?.so"

local so = require "uv.c"

so.echo("hello world1")
so.echo("hello world2")
so.echo("hello world3")
so.echo("hello world4")
so.echo("hello world5")
so.echo("hello world6")
so.echo("hello world7")
so.echo("hello world8")
so.echo("hello world9")

--./lua/src/lua test-uv.lua