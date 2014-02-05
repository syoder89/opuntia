--[[
LuCI - Lua Configuration IPSEC

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$

]]--

local ipm  = require "luci.model.ipsec".init()
local ds = require "luci.dispatcher"
local uci = require "luci.model.uci".cursor()
local m, s, o, p

local new_name="new_tunnel"
local count = 1
local new = 1
arg[1] = arg[1] or new_name

for _, tunnel in ipairs(ipm:get_tunnels()) do
	if(tunnel.name == arg[1]) then
		new = 0;
		break
	elseif(tunnel.name:gmatch("new_tunnel%d")) then
		count = count + 1
	end
end
new_name = new_name .. count

if( new == 1) then
	new_name = arg[1] or new_name
	uci:section( "ipsec_pre", "tunnel", new_name, {
		enabled	= "1", 
		exchange_mode = "main",
		authentication_method = "psk"
   	} ) 
	uci:save("ipsec_pre")
	luci.http.redirect(luci.dispatcher.build_url("admin","network", "ipsec" ,"tunnel_form", new_name))
end

m = Map("ipsec_pre", nil, nil)

p = m:section( SimpleSection )

p.template = "ipsec/title"
p.mode     = "basic"
p.instance = arg[1]

s = m:section(NamedSection, arg[1], translate("Tunnel Settings"))
s.anonymous = false
s.addremove = false

s:option(Flag, "enabled", translate("Enabled"))

gt = s:option(Value, "gateway", translate("Gateway"))

key = s:option(ListValue, "keyexchange", translate("Key Exchange"))
key.widget = "select"
key:value("ikev1", "IKE v1")
key:value("ikev2", "IKE v2")

model = s:option(ListValue, "exchange_mode", translate("Mode"))
model.widget = "select"
model:value("main", "Main")
model:value("aggressive", "Aggressive")

s:option(Value, "local_identifier", translate("Local Identifier"))

s:option(Value, "remote_identifier", translate("Remote Identifier"))

--auth = s:option(ListValue, "authentication_method", translate("Authentication method"))
--auth.widget = "select"
--auth:value("psk", "PSK")
--auth:value("certificate", "Certificate")

psk = s:option(Value, "pre_shared_key", translate("Pre shared key"))
--psk:depends("authentication_method", "psk")

--cert = s:option(TextValue, "certificate", translate("Certificate"))
--cert:depends("authentication_method", "certificate")

t = s:option(Value, "local_subnet", translate("Local subnet"))

n = s:option(Value, "remote_subnet", translate("Remote subnet"))

g = s:option(Value, "local_nat", translate("Local NAT"))

e1 = s:option(ListValue, "esp_encryption", translate("ESP Encryption"))
e1:value("", "Automatic") 
e1:value("3des", "3DES") 
e1:value("aes", "AES")
e1:value("aes128", "AES128")
e1:value("aes192", "AES192")
e1:value("aes256", "AES256")
e1:value("blowfish", "Blowfish")
e1:value("null", "Null")

i1 = s:option(ListValue, "esp_integrity", translate("ESP Integrity"))
i1:value("md5", "MD5") 
i1:value("sha", "SHA") 
i1:value("aesxcbc", "AESXCBC") 
i1:value("sha256", "SHA256") 
i1:value("sha512", "SHA512") 
i1:depends("esp_encryption", "3des")
i1:depends("esp_encryption", "aes")
i1:depends("esp_encryption", "aes128")
i1:depends("esp_encryption", "aes192")
i1:depends("esp_encryption", "aes256")
i1:depends("esp_encryption", "blowfish")
i1:depends("esp_encryption", "null")

g1 = s:option(ListValue, "esp_dh_group", translate("ESP DH Group"))
g1:value("modp768", "MODP768")
g1:value("modp1024", "MODP1024") 
g1:value("modp1536", "MODP1536") 
g1:value("modp2048", "MODP2048") 
g1:value("modp3072", "MODP3072") 
g1:value("modp4096", "MODP4096") 
g1:value("modp6144", "MODP6144") 
g1:value("modp8192", "MODP8192") 
g1:value("modp10245160", "MODP10245160") 
g1:value("modp20485224", "MODP20485224") 
g1:value("modp20485256", "MODP20485256") 
g1:depends("esp_encryption", "3des")
g1:depends("esp_encryption", "aes")
g1:depends("esp_encryption", "aes128")
g1:depends("esp_encryption", "aes192")
g1:depends("esp_encryption", "aes256")
g1:depends("esp_encryption", "blowfish")
g1:depends("esp_encryption", "null")

e2 = s:option(ListValue, "ike_encryption", translate("IKE Encryption"))
e2:value("", "Automatic")
e2:value("3des", "3DES")  
e2:value("aes", "AES")
e2:value("aes128", "AES128")
e2:value("aes192", "AES192")
e2:value("aes256", "AES256")
e2:value("blowfish", "Blowfish")
e2:value("null", "Null")

i2 = s:option(ListValue, "ike_integrity", translate("IKE Integrity"))
i2:value("md5", "MD5") 
i2:value("sha", "SHA") 
i2:value("aesxcbc", "AESXCBC") 
i2:value("sha256", "SHA256") 
i2:value("sha512", "SHA512") 
i2:depends("ike_encryption", "3des")
i2:depends("ike_encryption", "aes")
i2:depends("ike_encryption", "aes128")
i2:depends("ike_encryption", "aes192")
i2:depends("ike_encryption", "aes256")
i2:depends("ike_encryption", "blowfish")
i2:depends("ike_encryption", "null")

g2 = s:option(ListValue, "ike_dh_group", translate("IKE DH Group"))
g2:value("modp768", "MODP768")
g2:value("modp1024", "MODP1024") 
g2:value("modp1536", "MODP1536") 
g2:value("modp2048", "MODP2048") 
g2:value("modp3072", "MODP3072") 
g2:value("modp4096", "MODP4096") 
g2:value("modp6144", "MODP6144") 
g2:value("modp8192", "MODP8192") 
g2:value("modp10245160", "MODP10245160") 
g2:value("modp20485224", "MODP20485224") 
g2:value("modp20485256", "MODP20485256") 
g2:depends("ike_encryption", "3des")
g2:depends("ike_encryption", "aes")
g2:depends("ike_encryption", "aes128")
g2:depends("ike_encryption", "aes192")
g2:depends("ike_encryption", "aes256")
g2:depends("ike_encryption", "blowfish")
g2:depends("ike_encryption", "null")

return m
