--[[
LuCI - Lua Configuration Interface

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0
]]--

require("luci.tools.webadmin")

m = Map("snmpd", translate("SNMP Configuration"),
        translate("This page allows you to configure SNMP"))

ag = m:section(TypedSection, "agent", translate("Agent settings"))
ag.anonymous = true

ag:option(Value, "agentaddress", translate("address"), translate("Agent address"))

s = m:section(TypedSection, "system", translate("System settings"))
s.anonymous = true

s:option(Value, "sysLocation", translate("location"), translate("The [typically physical] location of the system"))

s:option(Value, "sysName", translate("name"), translate("System Name"))

s:option(Value, "sysContact", translate("contact"), translate("System Contact"))

v1 = m:section(TypedSection, "v1v2c", translate("V1/V2c Settings"))
v1.anonymous = true
v1.addremove = false

ev1 = v1:option(Flag, "enable", translate("enable"), "")

ro = v1:option(Value, "rocommunity", translate("community"), translate("Read Only Community"))
ro.rmempty=true
ro:depends("enable", 1)

rw = v1:option(Value, "rwcommunity", translate("community"), translate("Write Only Community"))
rw.rmempty=true
rw:depends("enable", 1)

v3 = m:section(TypedSection, "v3", translate("V3 Settings"))
v3.anonymous = true
v3.addremove = false

ev3 = v3:option(Flag, "enable", translate("enable"), "")

user = v3:option(Value, "user", translate("user"), translate("User"))
user.rmempty=true
user:depends("enable", 1)

pass = v3:option(Value, "password", translate("Password")) 
pass.rmempty=true
pass.password=true
pass:depends("enable", 1)

auth = v3:option(ListValue, "authproto", translate("Authentication protocol"))
auth:value("MD5", "MD5")
auth:value("SHA", "SHA")
auth.default="SHA"
auth:depends("enable", 1)

priv = v3:option(ListValue, "privproto", translate("Privacy protocol"))
priv:value("DES", "DES")
priv:value("AES", "AES")
priv.default="AES"
priv:depends("enable", 1)

type = v3:option(ListValue, "sectype", translate("Security type"))
type:value("noauth", "None")
type:value("auth", "Authentication")
type:value("priv", "Privacy")
type:value("authpriv", "Authentication + Privacy")
type.default="authpriv"
type:depends("enable", 1)

return m

