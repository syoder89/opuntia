--[[
LuCI - Lua Configuration Interface

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: ipsec.lua 9558 2013-08-19 13:58:22Z jow $
]]--

module("luci.controller.ipsec", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/ipsec_pre") then
		return
	end

	page = entry({"admin", "network", "ipsec"}, cbi("ipsec"), _("IPSEC"))

end
