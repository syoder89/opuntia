--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2010-2012 Jo-Philipp Wich <xm@subsignal.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

]]--

local ds = require "luci.dispatcher"
local m, s, o

m = Map("ipsec_pre", 
	translate("IPSEC"),
	translate("Allows to add tunnels."))

s = m:section(TypedSection, "tunnel", translate("Tunnels Settings"))
s.anonymous = false
s.addremove = true

s:option(Flag, "enabled", translate("Enabled"))

gt = s:option(Value, "gateway", translate("Gateway"))
gt.datatype = "ip4addr"

model = s:option(ListValue, "exchange_mode", translate("Mode"))
model.widget = "select"
model:value("main", "Main")
model:value("aggressive", "Aggressive")

s:option(Value, "local_identifier", translate("Local Identifier"))

s:option(Value, "remote_identifier", translate("Remote Identifier"))

auth = s:option(ListValue, "authentication_method", translate("Authentication method"))
auth.widget = "select"
auth:value("psk", "PSK")
auth:value("certificate", "Certificate")

psk = s:option(Value, "pre_shared_key", translate("Pre shared key"))
psk:depends("authentication_method", "psk")

cert = s:option(TextValue, "certificate", translate("Certificate"))
cert:depends("authentication_method", "certificate")

t = s:option(Value, "local_subnet", translate("Local subnet"))
t.datatype = "ip4addr"
t.rmempty = false

n = s:option(Value, "remote_subnet", translate("Remote subnet"))
n.datatype = "ip4addr"
n.rmempty = true

g = s:option(Value, "local_nat", translate("Local NAT"))
g.datatype = "ip4addr"
g.rmempty = true

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