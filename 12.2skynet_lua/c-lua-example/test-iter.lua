function IterPermutation(tab, func)
	if not next(tab) or #tab == 1 then
		return (function() end)
	end
	func = func or function (lhs, rhs)
		return lhs < rhs
	end
	table.sort(tab, func)

	local function reverse(t, first, last)
		while first < last do
			t[first], t[last] = t[last], t[first]
			first = first + 1
			last = last - 1
		end
	end

    local function iter(t, idx)
        if idx == 0 then return idx+1,t end
		local _next = #t
		while true do
			local _next1 = _next
			_next = _next - 1
			if func(t[_next], t[_next1]) then
				local _mid = #t
				while not func(t[_next], t[_mid]) do
					_mid = _mid - 1
				end
				t[_next], t[_mid] = t[_mid], t[_next]
				reverse(t, _next1, #t)
				return idx+1, t
			end
			if _next == 1 then
				reverse(t, 1, #t)
				return
			end
		end
	end
	return iter, tab, 0
end

-- 负载均衡器 Robin-round
function Balancer(max)
	local i = 0
	return function ()
		i = i % max + 1
		return i
	end
end

function Cache(func)
	return setmetatable({}, {
		__mode = "kv",  -- gc 弱表
		__index = func,
	})
end


-- lightuserdata c语言中的指针 c来创建  c来销毁
-- userdata 存储c的结构体内存  lua_newuserdata 交由lua自动销毁  gc
-- 如果c的结构体 包含c中分配的内容  __gc元方法 释放c的内存 


-- 不需要生成所有的情况
-- 边生成 边检查
-- local tab = {1,2,3,4}
-- for idx, t in IterPermutation(tab) do
--     print(("=============idx = %d=============="):format(idx))
-- 	print(table.concat(t, " "))
-- 	if func(t) then  --1 3 2 4
-- 		return
-- 	end
-- end

-- local str = "11=22;21=34;32=43;"


-- for k1, k2 in string.gmatch(str, "(%d+)=(%d+)") do
-- 	print(k1, k2)
-- end

-- print("----------------------------------")
