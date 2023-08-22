

local function test1(tt)
    tt = {1, 3, 5}
end

local function test2(tt)
    tt[1] = 2
    tt["a"] = "hello"
end


print("-------------test1-------------")
local t1 = {4, 5, 6}

test1(t1)

for _,v in pairs(t1) do
    print(v)
end

print("-------------test2-------------")

local t2 = {4, 5, 6}

test2(t2)

for _, v in pairs(t2) do
    print(v)
end
