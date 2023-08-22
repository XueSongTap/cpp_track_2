-- local tab1 = {
--     ["mark"] = 100,
--     ["king"] = 90,
--     [1] = 10,
--     [(function() end)] = 100,
--     [{1,2,3}] = {1,2,3}
-- }

-- local function name ()

-- end

-- local tab2 = {[1] = "mark", [2] = "king", [3] = 1, [4] = (function() end)}

-- for k, v in pairs(tab1) do
--     print(k, v)
-- end

-- for _, v in ipairs(tab2) do
--     print(v)
-- end


-- do
--     local tab3 = setmetatable ({
--         ["mark"] = 100,
--         ["king"] = 90,
--     }, {
--         __index = function (t, k)
--             return "nil1000"
--         end,
--         __newindex = function (t, k, v)
--             error("can't set")
--         end,
--         __gc = function ()
--             print("tab3 gc")
--         end
--     })
--     tab3["mark"] = 1000

--     print("set mark = 1000+++++", tab3["mark"])
-- end
-- collectgarbage()

-- print("gc end")

local function rets()
    return 1, 2, 3, 4
end

local ret1, ret2, _, ret4 = rets()
print(ret1, ret2, ret4)
print(rets())

local a = 3
local b = 4
a, b = b, a
print(a, b)

local val = 0
if nil then
    print("success")
end

local function iteractor(NUM, START)
    local i = START or 0
    local max = NUM or 16
    return function ()
        i = i % max + 1
        return i
    end
end

local iter = iteractor()

for i = 1, 20 do
    print(iter())
end

-- local t1 = {1, 2, 3}
-- local t2 = {1, 2}

-- local cache = setmetatable({}, {__mode = "kv"})
-- local refs = {}
-- do
--     cache["darren"] = t1
--     cache["mark"] = t2
--     refs["mark"] = t2
--     t1 = nil
--     t2 = nil
-- end
-- collectgarbage("collect")
-- for k, v in pairs(cache) do
--     print(k, v)
-- end
-- for k, v in pairs(refs) do
--     print(k, v)
-- end