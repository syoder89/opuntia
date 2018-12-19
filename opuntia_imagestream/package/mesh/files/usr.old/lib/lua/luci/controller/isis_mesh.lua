--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: qos.lua 9558 2012-12-18 13:58:22Z jow $
]]--

module("luci.controller.isis_mesh", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/isis_mesh") then
		return
	end
	
	page = entry({"admin", "network", "isis_mesh"}, cbi("admin_network/isis_mesh"), _("WIFI Mesh"))

end

