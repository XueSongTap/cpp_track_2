local skynet = require "skynet"

local CMD = {}

function CMD.hello(...)
    print(skynet.address(skynet.self()), "hello", ...)
end

skynet.start(function ()
    skynet.dispatch("lua", function (_, _, cmd, ...)
        local f = assert(CMD[cmd])
        f(...)
    end)
end)
