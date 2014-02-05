module("luci.controller.monitor", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/monitor") then
		return
	end

	local page

	page = entry({"admin", "network", "monitor"}, cbi("quota/monitor", {autoapply=true}), _("Bandwidth Monitoring"))
	page = entry({"admin", "network", "get_user"}, call("action_get_user"))
	page.dependent = true
end

function Split(str, delim, maxNb)
    -- Eliminate bad cases...
    if string.find(str, delim) == nil then
        return { str }
    end
    if maxNb == nil or maxNb < 1 then
        maxNb = 0    -- No limit
    end
    local result = {}
    local pat = "(.-)" .. delim .. "()"
    local nb = 0
    local lastPos
    for part, pos in string.gfind(str, pat) do
        nb = nb + 1
        result[nb] = part
        lastPos = pos
        if nb == maxNb then break end
    end
    -- Handle the last field
    if nb ~= maxNb then
        result[nb + 1] = string.sub(str, lastPos)
    end
    return result
end

function form(tem)
	temp = tonumber(tem)
	if temp < 1000 then
		tstr = string.format("%d", temp)
		ufm = "KB"
	else
		if temp < 1000000 then
			tstr = string.format("%g", temp/1000)
			ufm = "MB"
		else
			tstr = string.format("%g", temp/1000000)
			ufm = "GB"
		end
	end
	local str = tstr .. " " .. ufm
	return str
end

function action_get_user()
	local rv = {}

	local num = 0
	local ip = nil
	local mac = nil
	local download = nil
	local total = nil
	local str = ""
	local file = io.open("/tmp/usage.db", "r")
	if file ~= nil then
		repeat
			local line = file:read("*line")
			if line == nil then
				break
			end
			num = num + 1
			local t = Split(line, ",", 7)
			local pi = t[7]
			local host = nixio.getnameinfo(pi)
			local per, pere
			if host == nil then
				host = pi
			else
				per, pere = string.find(host, ".lan")
				local tt = host
				if per ~= nil then
					tt = string.sub(host, 1, per-1) 
					--tt = host
				end
				host = pi .. " ( " .. tt .. " )"
			end
			if ip ~= nil then
				ip = ip .. "$$" .. host
			else
				ip = host
			end
			if mac ~= nil then
				mac = mac .. "$$" .. t[1]
			else
				mac = t[1]
			end

			local temp = t[2]
			str = form(temp)
			if download ~= nil then
				download = download .. "$$" .. str
			else
				download = str
			end
			local temp = t[3]
			str = form(temp)
			if upload ~= nil then
				upload = upload .. "$$" .. str
			else
				upload = str
			end
			local tot = t[2] + t[3]
			str = form(tot)
			if total ~= nil then
				total = total .. "$$" .. str
			else
				total = str
			end
		until 1 == 0
		file:close()
	end

	rv["length"] = num
	rv["ip"] = ip
	rv["mac"] = mac
	rv["up"] = upload
	rv["down"] = download
	rv["total"] = total

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end
