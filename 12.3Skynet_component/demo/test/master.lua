local skynet = require "skynet"

local CMD = {}
-- local slaves = {}
local NUM = 16
local NUM_MASK = NUM-1
local iterator = (function()
    local i = 0
    return function ()
        i = (i & NUM_MASK)+1;
        return i
    end
end)()

function CMD.get_idx(...)
    local idx = iterator();
    print("recv hello", idx, ...)
    local name = ...
    -- skynet.ret(skynet.pack(idx))
    -- skynet.retpack(idx, ...)
    -- skynet.retpack(idx)
    local rs = skynet.response()
    skynet.fork(function ()  -- 创建一个协程
        skynet.sleep(500)  -- 休息5秒
        rs(true, idx, name)
    end)
    -- skynet.send(slaves[idx], "lua", "hello", idx)
end

skynet.start(function ()
    print("master launch")
    -- for i=1, NUM do
    --     slaves[#slaves+1] = skynet.newservice("slave")
    -- end
    skynet.dispatch("lua", function (_, _, cmd,...)
        local f = assert(CMD[cmd])
        f(...)
    end)
end)
