local skynet = require "skynet"
local queue = require "skynet.queue"
local socket = require "skynet.socket"

local cs = queue()

local tinsert = table.insert
local tremove = table.remove
-- local tconcat = table.concat
local CMD = {}

local queues = {}

local resps = {}

local function sendto(clientfd, arg)
    -- local ret = tconcat({"fd:", clientfd, arg}, " ")
    -- socket.write(clientfd, ret .. "\n")
    socket.write(clientfd, arg .. "\n")
end

function CMD.ready(client)
    if not client or not client.name then
        return skynet.retpack(false, "准备：非法操作")
    end
    if resps[client.name] then
        return skynet.retpack(false, "重复准备")
    end
    tinsert(queues, 1, client)  -- 万人同时在线 匹配逻辑目前这样是不行
    -- 1. ready 进行匹配的
    -- 2. 一个服务中 进行操作的
    -- fixed: 开启多个 hall  n* thread
    -- 所以数据 不能够分散放在 hall  需要数据中心   缓存机制  
    -- 1.  提供配置  hall 定时刷新到 数据中心的频率  
    -- 2. 配置项  
    -- 性能： 分布式一些思想 

    -- 1. 一条连接 一个用户   预先分配 10000个
    -- 2. 开启  n* thread    agent 做成无状态的  前提是  agent 业务不复杂 gc
    resps[client.name] = skynet.response()
    if #queues >= 3 then
        local roomd = skynet.newservice("room") -- 性能瓶颈  逻辑进行中动态大量创建 
        local members = {tremove(queues), tremove(queues), tremove(queues)}
        for i=1, 3 do
            local cli = members[i]
            resps[cli.name](true, roomd)
            resps[cli.name] = nil
        end
        skynet.send(roomd, "lua", "start", members)
        return
    end
    sendto(client.fd, "等待其他玩家加入")
end

function CMD.offline(name)
    for pos, client in ipairs(queues) do
        if client.name == name then
            tremove(queues, pos)
            break
        end
    end
    if resps[name] then
        resps[name](true, false, "退出")
        resps[name] = nil
    end
    skynet.retpack()
end

skynet.start(function ()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        local func = CMD[cmd]
        if not func then
            skynet.retpack({ok = false, msg = "非法操作"})
            return
        end
        cs(func, ...)
    end)
end)
