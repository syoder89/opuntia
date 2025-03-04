--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: qos.lua 9558 2012-12-18 13:58:22Z jow $
]]--

module("luci.controller.snmp", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/snmpd") then
		return
	end
	
	entry({"admin", "network", "snmp"}, cbi("snmp"), _("SNMP")).acl_depends = { "luci-app-snmp" }
end
