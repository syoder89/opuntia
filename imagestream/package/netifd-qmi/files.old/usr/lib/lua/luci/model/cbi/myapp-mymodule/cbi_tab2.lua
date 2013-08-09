
require "luci.fs"
require "luci.sys"
require "luci.util"

m = SimpleForm("atcmd", translate("AT Command Execution"), translate("Execute AT Commands by entering them in the box below and clicking on Submit"))
m.reset = false
m.submit = Submit

t = m:field(Value, "cmd")
t.rmempty = true
t.rows = 1

function m.handle(self, state, data)
	if state == FORM_VALID then
		if data.cmd then
			luci.fs.writefile("/tmp/cmd.at", data.cmd:gsub("\r\n", "\n"))
			luci.fs.writefile("/tmp/result.at", "Empty")
			flag = 0
			while flag == 0 do
				ret = luci.fs.readfile("/tmp/result.at") or ""
				if ret ~= "Empty" and ret ~= nil then
					flag = 1
				end
			end
		end
	end
	return true
end


r = m:field(TextValue, "result")
r.rmempty = true
r.rows = 20

function r.cfgvalue()
	rval = luci.fs.readfile("/tmp/result.at") or ""
	luci.fs.writefile("/tmp/result.at", " ")
	return rval
end


return m