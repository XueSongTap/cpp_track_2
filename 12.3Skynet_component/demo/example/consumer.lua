local skynet = require "skynet"

local function table_dump( object )
    if type(object) == 'table' then
        local s = '{ '
        for k,v in pairs(object) do
            if type(k) ~= 'number' then k = string.format("%q", k) end
            s = s .. '['..k..'] = ' .. table_dump(v) .. ','
        end
        return s .. '} '
    elseif type(object) == 'function' then
        return tostring(object)
    elseif type(object) == 'string' then
        return string.format("%q", object)
    else
        return tostring(object)
    end
end

local mode, key = ...
if mode == "slave" then
    local rediscluster = require "skynet.db.redis.cluster"
    local function onmessage(data,channel,pchannel)
        print("onmessage",data,channel,pchannel)
    end
    skynet.start(function ()
        local db = rediscluster.new({
                {host="127.0.0.1",port=7001},
            },
            {read_slave=true,auth=nil,db=0,},
            onmessage
        )
        skynet.fork(function ()
            while true do
                local res = db:zrange(key, 0, 0, "withscores")
                -- print(table_dump(res))
                if not next(res) then
                    skynet.sleep(100)
                else
                    local expire = tonumber(res[2])
                    -- print("time:", expire, skynet.time())
                    local now = skynet.time()*100
                    if now >= expire then
                        print(("task:%s is comsumed"):format(res[1]))
                        db:zrem(key, res[1])
                    else
                        skynet.sleep(expire - now)
                    end
                end
            end
        end)
    end)

else
    skynet.start(function ()
        for i=0,9 do
            skynet.newservice(SERVICE_NAME, "slave", "task_group:"..i)
        end
    end)
end
