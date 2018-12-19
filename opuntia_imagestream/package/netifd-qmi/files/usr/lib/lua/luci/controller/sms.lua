--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

]]--

module("luci.controller.sms", package.seeall)

function index()
	if not nixio.fs.access("/bin/man3g") then
		return
	end

	local page

	page = entry({"admin", "services", "sms"}, template("admin_network/sms"), _("SMS Messaging"))
	page.dependent = true

	entry({"admin", "services", "sms", "status"}, call("sms_status")).leaf = true
	entry({"admin", "services", "sms", "delete"}, call("sms_delete")).leaf = true
end

function sms_status()
	local man3g = io.popen("man3g display-sms --machine | sed 's/\"//g'")
	if man3g then
		local sms = { }
		while true do
			local ln = man3g:read("*l")
			if not ln then
				break
			end
			local id, status, smsc, sender, date, message
			id=ln
			status = man3g:read("*l")
			smsc = man3g:read("*l")
			sender = man3g:read("*l")
			date = man3g:read("*l")
			len = man3g:read("*l")
			message = man3g:read("*l")

			sms[#sms+1] = {
				id = id,
				status = status,
				smsc = smsc,
				sender = sender,
				date = date,
				message = message
			}
		end

		man3g:close()

		luci.http.prepare_content("application/json")
		luci.http.write_json(sms)
	end
end

function sms_delete(num)
	local idx = tonumber(num)
	local uci = luci.model.uci.cursor()

	if idx and idx > 0 then
		luci.sys.call("man3g delete-sms %d 2>/dev/null" % idx)

		luci.http.status(200, "OK")
		return
	end

	luci.http.status(400, "Bad request")
end
