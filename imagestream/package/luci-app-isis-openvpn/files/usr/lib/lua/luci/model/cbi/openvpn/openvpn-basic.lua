--[[
LuCI - Lua Configuration OpenVPN

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2013 Christian Richarz <christian.richarz@multidata.de>
Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

$Id: openvpn-basic-lua 9999 2013-06-27 13:01
]]--

require( "luci.model.uci" )

name = arg[1]
m = Map( "openvpn" )
p = m:section( SimpleSection )

p.template = "openvpn/pageswitch"
p.mode     = "basic"
p.instance = name

s = m:section( NamedSection, name, "openvpn" )
	
local client = s:option( Flag, "client", translate( "Configure client mode" ) )
  client.value = "1"
  
local mode = s:option(ListValue, "mode", translate( "Server Mode" ) )
  mode:value("p2p", "Point to point")
  mode:value("server", "Server")
  mode:depends("client", "")

local dev_type = s:option( ListValue, "dev_type", translate("Type") )
  dev_type:value( "tun", "TUN" )
  dev_type:value( "tap", "TAP" )
  dev_type.description = translate( "Use TUN for routing based connections and TAP for bridging" )

local proto = s:option( ListValue,"proto", translate("Protocol") )
  proto:value( "udp", "UDP" )
  proto:value( "tcp-client", "TCP Client" )
  proto:value( "tcp-server", "TCP Server" )

local host = s:option( Value, "local", translate("Source IP address") )
  host:depends("client", "")

local port = s:option( Value, "lport", translate("Source port") )
  port.value = "1194"
  port:depends("client", "") 

local remote = s:option(Value, "remote", translate("Destination IP address") )
  remote:depends("mode", "p2p")
  remote:depends("client", "1")

local rport = s:option( Value, "rport", translate("Destination port") )
  rport.value = "1194"
  rport:depends("mode", "p2p")
  rport:depends("client", "1")

local secret_type = s:option( ListValue, "secret_type", translate("Type of Static Key") )
  secret_type:value( "passphrase", "Password Phrase Generator" )
  secret_type:value( "file", "Upload File" )
  secret_type:depends("mode", "p2p")

local sec_phrase = s:option(Value, "secret_phrase", translate("Secret Phrase") )
  sec_phrase:depends({mode = "p2p", secret_type = "passphrase"})

local key = s:option(FileUpload, "secret", translate("Local Static Key") )
  key:depends({mode = "p2p", secret_type = "file"})
 
local server = s:option(Value, "server", translate("Local VPN network") )
  server.value = "10.10.0.0 255.255.255.0"
  server:depends({mode = "server", dev_type = "tun"})
  server.description = translate( "Serverside_Network_IP Netmask" )
local client_to_client = s:option(Flag, "client_to_client", translate("Allow client-to-client traffic") )
  client_to_client:depends("mode", "server")

local route = s:option(DynamicList, "route", translate("Add route"))
local routegw = s:option(Value, "route_gateway", translate("Route gateway"))

local ca = s:option(FileUpload, "ca", translate("Certificate authority") )
  ca:depends("client", "1")
  ca:depends("mode", "server")
local cert = s:option( FileUpload, "cert", translate("Local certificate") )
  cert:depends("client", "1")
  cert:depends("mode", "server")
local pkcs12 = s:option( FileUpload, "pkcs12", translate("PKCS#12 file containing keys") )
  pkcs12:depends("client", "1")
  pkcs12:depends("mode", "server")

function m.on_before_save(self) 
	if(self.changed) then
		local type = secret_type:formvalue(name)
		local phrase = sec_phrase:formvalue(name)
		
		if(type == "passphrase") then
			local filename = "/lib/uci/upload/" .. key:cbid(tostring(name))
			local pass = luci.util.exec("php-cli /usr/lib/lua/luci/tools/openvpn_passphrase_generator.php " .. phrase)
			file = io.open(filename, "w")
			file:write(pass)
			file:close()
			key.map:set(name, key.option, filename)
		end
	end
end

function m.on_after_commit(self)
    luci.sys.call("/etc/init.d/openvpn.luci down %s" % name)
    luci.sys.call("/etc/init.d/openvpn.luci up %s" % name)
end

return m
