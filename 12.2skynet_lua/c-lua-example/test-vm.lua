package.cpath = "luaclib/?.so"
local so = require "uv.c"

function Init(args)
    print("call [init] function", args)
end

function Loop()
    print("call [loop] function")
    for k, v in ipairs({1,2,3,4,5}) do
        so.echo(v)
    end
end

function Release()
    print("call [release] function")
end
