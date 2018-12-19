--[[
LuCI - Lua Configuration IPSEC

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$

]]--

local ipm  = require "luci.model.isis_mesh".init()
local ds = require "luci.dispatcher"
local uci = require "luci.model.uci".cursor()
local m, s, o, p

local new = 1
arg[1] = arg[1] or ""
local name=arg[1]
local macaddr = arg[1]:gsub("_", ":")

for _, map in ipairs(ipm:get_maps()) do
	if(map.name == arg[1]) then
		new = 0;
		break
	end
end

if( new == 1) then
	if( arg[1] == "") then
		luci.http.redirect(luci.dispatcher.build_url("admin","network", "isis_mesh"))
	end
	new_name = arg[1]:gsub(":", "_")
	uci:section( "isis_mesh", "map", new_name, {
		authorized = "0", 
   	} ) 
	uci:save("isis_mesh")
	luci.http.redirect(luci.dispatcher.build_url("admin","network", "isis_mesh" ,"map_form", new_name))
end

m = Map("isis_mesh", nil, nil)

function m.on_commit(map)
	if (map.changed) then
                local hname = hostname:formvalue(name)

		local found=0
        	uci:foreach("dhcp", "host",
                	function(s)
                        	if s.mac == macaddr then
                                	uci:set("dhcp", s['.name'], "name", hname)
					uci:commit("dhcp")
					found = 1
                        	end
                	end)
		if (found == 0) then
			uci:section( "dhcp", "host", nil, { name = hname, mac = macaddr } )
			uci:commit("dhcp")
		end
	end
end

p = m:section( SimpleSection )

p.template = "isis_mesh/title"
p.mode     = "basic"
p.instance = arg[1]

s = m:section(NamedSection, arg[1], translate("Mesh Access Point Settings"))
s.anonymous = false
s.addremove = false

hostname = s:option(Value, "hostname", translate("Hostname"), translate("The hostname for this MAP node"))

s:option(Flag, "authorized", translate("Authorized"), "Is this MAP node allowed to join the mesh?")
s:option(Value, "sysLocation", translate("Location"), translate("The human-readable location of the system"))
s:option(Value, "sysLatitude", translate("Latitude"), translate("The latitude, eg: 29.575358"))
s:option(Value, "sysLongitude", translate("Longitude"), translate("The longitude, eg: -98.302043"))

return m
