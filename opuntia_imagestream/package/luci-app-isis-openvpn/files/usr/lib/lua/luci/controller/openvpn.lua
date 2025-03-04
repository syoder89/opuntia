--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: openvpn.lua 9507 2012-11-26 12:53:43Z jow $
]]--

module("luci.controller.openvpn", package.seeall)

function index()
	entry( {"admin", "network", "openvpn"}, cbi("openvpn/openvpn"), _("OpenVPN") )
	entry( {"admin", "network", "openvpn", "basic"},    cbi("openvpn/openvpn-basic"),    nil ).leaf = true
	entry( {"admin", "network", "openvpn", "advanced"}, cbi("openvpn/openvpn-advanced"), nil ).leaf = true
end

