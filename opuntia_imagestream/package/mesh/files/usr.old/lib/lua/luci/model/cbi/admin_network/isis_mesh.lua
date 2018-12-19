--[[
LuCI - Lua Configuration Interface

Copyright 2013 Scott Yoder <syoder@imagestream.com>

]]--

require("luci.tools.webadmin")

m = Map("isis_mesh", translate("WIFI Mesh Configuration"),
        translate("This page allows you to configure your WIFI Mesh"))

function m.on_commit(map)
	-- luci.sys.call("(env -i /etc/init.d/snmpd restart) >/dev/null 2>/dev/null")
end

s = m:section(TypedSection, "mesh", translate("Mesh settings"))
s.anonymous=true

s:option(Value, "mesh_name", translate("Mesh Name"), translate("The name for your mesh network"))

mn = m:section(NamedSection, "map", translate("Mesh Access Point"))
mn:option(Value, "hostname", translate("Hostname"), translate("The hostname for this MAP node"))

mn:option(Flag, "authorized", translate("Authorized"), "Is this MAP node allowed to join the mesh?")
mn:option(Value, "sysLocation", translate("Location"), translate("The human-readable location of the system"))
mn:option(Value, "sysLatitude", translate("Latitude"), translate("The latitude, eg: 29.575358"))
mn:option(Value, "sysLongitude", translate("Longitude"), translate("The longitude, eg: -98.302043"))

return m
