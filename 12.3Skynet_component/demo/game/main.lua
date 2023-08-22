local skynet = require "skynet"
local socket = require "skynet.socket"

local function accept(clientfd, addr)
    skynet.newservice("agent", clientfd, addr) -- 性能瓶颈 提前创建
end

skynet.start(function ()
    local listenfd = socket.listen("0.0.0.0", 8888)
    skynet.uniqueservice("redis")
    skynet.uniqueservice("hall")
    socket.start(listenfd, accept)-- 绑定actor 与 fd
end)
