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
	
local mode = s:option(ListValue, "mode", translate( "Mode" ) )
  mode:value("", "Client")
  mode:value("p2p", "Point to point")
  mode:value("server", "Server")

local dev_type = s:option( ListValue, "dev_type", translate("Type") )
  dev_type:value( "tun", "TUN" )
  dev_type:value( "tap", "TAP" )
  dev_type.description = translate( "Use TUN for routing based connections and TAP for bridging" )

local proto = s:option( ListValue,"proto", translate("Protocol") )
  proto:value( "udp", "UDP" )
  proto:value( "tcp-client", "TCP Client" )
  proto:value( "tcp-server", "TCP Server" )

local host = s:option( Value, "local", translate("Source IP address") )

local port = s:option( Value, "lport", translate("Source port") )

local remote = s:option(Value, "remote", translate("Destination IP address") )
  remote:depends("mode", "p2p")
  remote:depends("mode", "")

local rport = s:option( Value, "rport", translate("Destination port") )
  rport.value = "1194"
  rport:depends("mode", "p2p")
  rport:depends("mode", "")

local ifconfig = s:option( Value, "ifconfig", translate("Tunnel IP Addresses"), translate("For tap mode use an IP address and subnet mask. For tun mode use the local IP and remote point-to-point IP addresses") )
  ifconfig.placeholder = "192.168.55.1 192.168.55.2"

local keepalive = s:option(Value, "keepalive", translate("Keepalive"),  translate("Interval and Timeout in seconds - Helper directive to simplify the expression of --ping and --ping-restart in server mode configurations") )
  keepalive.default = "10 60"

local compress = s:option( ListValue, "compress", translate( "Compression" ) )
  compress:value( "", "None" )
  compress:value( "lzo", "LZO" )
  compress:value( "lz4", "LZ4" )

local secret_type = s:option( ListValue, "secret_type", translate("Type of Static Key") )
  secret_type:value( "passphrase", "Password Phrase Generator" )
  secret_type:value( "file", "Upload File" )
  secret_type:depends("mode", "p2p")

local sec_phrase = s:option(Value, "secret_phrase", translate("Secret Phrase") )
  sec_phrase:depends({mode = "p2p", secret_type = "passphrase"})

local server = s:option(Value, "server", translate("Local VPN network") )
  server.value = "10.10.0.0 255.255.255.0"
  server:depends({mode = "server", dev_type = "tun"})
  server.description = translate( "Serverside_Network_IP Netmask" )

local client_to_client = s:option(Flag, "client_to_client", translate("Allow client-to-client traffic") )
  client_to_client:depends("mode", "server")

local route = s:option(DynamicList, "route", translate("Add route"))
local routegw = s:option(Value, "route_gateway", translate("Route gateway"))

local ca = s:option(FileUpload, "ca", translate("Certificate authority") )
  ca:depends("mode", "server")
  ca:depends("mode", "")

local cert = s:option( FileUpload, "cert", translate("Local certificate") )
  cert:depends("mode", "server")
  cert:depends("mode", "")

local key = s:option(FileUpload, "secret", translate("Local key") )
  key:depends({mode = "p2p", secret_type = "file"})

local tls_key = s:option(FileUpload, "key", translate("Local key") )
  tls_key:depends("mode", "server")
  tls_key:depends("mode", "")
 
local pkcs12 = s:option( FileUpload, "pkcs12", translate("PKCS#12 file containing keys") )
  pkcs12:depends("mode", "server")
  pkcs12:depends("mode", "")

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
    luci.sys.call("/etc/init.d/openvpn stop %s" % name)
    luci.sys.call("/etc/init.d/openvpn start %s" % name)
end

return m
