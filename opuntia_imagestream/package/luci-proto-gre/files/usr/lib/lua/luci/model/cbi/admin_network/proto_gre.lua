--[[
LuCI - Lua Configuration Interface

Copyright 2013 Juan Flores <juan.flores@gd-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0
]]--

require("luci.tools.webadmin")

local map, section, net = ...
local ifc = net:get_interface()

local ipaddr, peeraddr, ttl, tunlink, tos, ikey, okey, icsum, ocsum, iseqno, oseqno, keepalive, keep_interval, keep_timeout
local mtu, metric, tos

ipaddr = section:taboption("general", Value, "ipaddr", translate("Local endpoint"))
ipaddr.datatype = "ipaddr"

peeraddr = section:taboption("general", Value, "peeraddr", translate("Remote endpoint"))
peeraddr.datatype = "ipaddr"

tunlink = section:taboption("general", ListValue, "tunlink", translate("Interface to bind tunnel"))
luci.tools.webadmin.cbi_add_networks(tunlink)

ikey = section:taboption("advanced", Value, "ikey", translate("Incoming Key"))
ikey.datatype = "uinteger"

okey = section:taboption("advanced", Value, "okey", translate("Outgoing Key"))
okey.datatype = "uinteger"

icsum = section:taboption("advanced", Flag, "icsum", translate("Require incoming checksum"))
ocsum = section:taboption("advanced", Flag, "ocsum", translate("Require outgoing checksum"))

iseqno = section:taboption("advanced", Flag, "iseqno", translate("Perform incoming packet serialization"))
oseqno = section:taboption("advanced", Flag, "oseqno", translate("Perform outgoing packet serialization"))

tos = section:taboption("advanced", ListValue, "tos", translate("Type of Service"))
tos:value("inherit", translate("Inherit from inner header"))
tos:value("none", translate("None"))

ttl = section:taboption("advanced", Value, "ttl", translate("TTL"))
ttl.datatype = "uinteger"

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
