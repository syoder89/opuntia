--[[
LuCI - Lua Configuration Interface

Copyright 2013 Juan Flores <juan.flores@gd-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0
]]--

local map, section, net = ...
local ifc = net:get_interface()

local source, destination, tun_addr, tun_net, key, ttl, sequencing, keepalive, keep_interval, keep_timeout
local mtu, metric

source = section:taboption("general", Value, "source", translate("Source Address"))
source.datatype = "ipaddr"

destination = section:taboption("general", Value, "destination", translate("Destination Address"))
destination.datatype = "ipaddr"

tun_addr = section:taboption("general", Value, "ipaddr", translate("IPv4 address"))
tun_addr.datatype = "ipaddr"

tun_net = section:taboption("general", Value, "ptpaddr", translate("IPv4 point-to-point address"))
tun_net.datatype = "ipaddr"

key = section:taboption("advanced", Value, "key", translate("Key"))
key.datatype = "uinteger"

ttl = section:taboption("advanced", Value, "ttl", translate("TTL"))
ttl.datatype = "uinteger"

sequencing = section:taboption("advanced", Flag, "sequencing", translate("Enable Sequencing"))

keepalive = section:taboption("advanced", Flag, "keepalive", translate("Enable Keep Alive"))

keep_interval = section:taboption("advanced", Value, "keepalive_interval", translate("Keep Alive Interval"))
keep_interval.description = translate("(seconds)")
keep_interval.datatype = "uinteger"
keep_interval.default = 10
keep_interval:depends("keepalive", 1)

keep_timeout = section:taboption("advanced", Value, "keepalive_retries", translate("Keep Alive Retries"))
keep_timeout.datatype = "uinteger"
keep_timeout.default = 3
keep_timeout:depends("keepalive", 1)

mtu = section:taboption("advanced", Value, "mtu", translate("Override MTU"))
mtu.placeholder = "1500"
mtu.datatype    = "max(9200)"
