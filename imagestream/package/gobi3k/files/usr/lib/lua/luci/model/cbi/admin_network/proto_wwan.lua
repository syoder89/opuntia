--[[
LuCI - Lua Configuration Interface

Copyright 2011 Jo-Philipp Wich <xm@subsignal.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0
]]--

local map, section, net = ...

local device, apn, service, pincode, username, password
local ipv6, maxwait, defaultroute, metric, peerdns, dns,
      keepalive_failure, keepalive_interval, demand


service = section:taboption("general", Value, "service", translate("Service Type"))
service:value("", translate("-- Please choose --"))
service:value("umts", "UMTS/EDGE/GPRS")
service:value("umts_only", translate("UMTS only"))
service:value("gprs_only", translate("GPRS/EDGE only"))
service:value("cdma", "CDMA/EV-DO")


apn = section:taboption("general", Value, "apn", translate("APN"))
apn:depends("service", "umts")
apn:depends("service", "umts_only")
apn:depends("service", "gprs_only")

pincode = section:taboption("general", Value, "pincode", translate("PIN"))
pincode:depends("service", "umts")
pincode:depends("service", "umts_only")
pincode:depends("service", "gprs_only")

username = section:taboption("general", Value, "username", translate("PAP/CHAP username"))
password = section:taboption("general", Value, "password", translate("PAP/CHAP password"))
password.password = true

local hostname, accept_ra, send_rs
local bcast, defaultroute, peerdns, dns, metric, clientid, vendorclass


hostname = section:taboption("general", Value, "hostname",
        translate("Hostname to send when requesting DHCP"))

hostname.placeholder = luci.sys.hostname()
hostname.datatype    = "hostname"


if luci.model.network:has_ipv6() then

        accept_ra = s:taboption("general", Flag, "accept_ra", translate("Accept router advertisements"))
        accept_ra.default = accept_ra.enabled


        send_rs = s:taboption("general", Flag, "send_rs", translate("Send router solicitations"))
        send_rs.default = send_rs.disabled
        send_rs:depends("accept_ra", "")

end

defaultroute = section:taboption("advanced", Flag, "defaultroute",
        translate("Use default gateway"),
        translate("If unchecked, no default route is configured"))

defaultroute.default = defaultroute.enabled


peerdns = section:taboption("advanced", Flag, "peerdns",
        translate("Use DNS servers advertised by peer"),
        translate("If unchecked, the advertised DNS server addresses are ignored"))

peerdns.default = peerdns.enabled


dns = section:taboption("advanced", DynamicList, "dns",
        translate("Use custom DNS servers"))

dns:depends("peerdns", "")
dns.datatype = "ipaddr"
dns.cast     = "string"


metric = section:taboption("advanced", Value, "metric",
        translate("Use gateway metric"))

metric.placeholder = "0"
metric.datatype    = "uinteger"

clientid = section:taboption("advanced", Value, "clientid",
        translate("Client ID to send when requesting DHCP"))


vendorclass = section:taboption("advanced", Value, "vendorid",
        translate("Vendor Class to send when requesting DHCP"))


luci.tools.proto.opt_macaddr(section, ifc, translate("Override MAC address"))


mtu = section:taboption("advanced", Value, "mtu", translate("Override MTU"))
mtu.placeholder = "1500"
mtu.datatype    = "max(1500)"
