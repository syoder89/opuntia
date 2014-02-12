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


--radiotype = section:taboption("general", Value, "radiotype", translate("Radio Type"))
--radiotype:value("gsm", translate("GSM/UTMS"))
--radiotype:value("cdma", translate("CDMA/EV-DO"))
--radiotype.default="gsm"

--firmware = section:taboption("advanced", Value, "firmware", translate("Provider Firmware"))
--firmware:value("2", "AT&T")
--firmware:value("4", "T-Mobile")
--firmware:value("6", "Other GSM/UTMS (TELCEL/Movistar/Iusacell/Tigo)")
--firmware:value("7", "Telefonica")
--firmware:value("8", "Telecom Italia")
--firmware:value("9", "Orange")

provider = section:taboption("general", ListValue, "provider", translate("Provider"))
provider:value("verizon", "Verizon") --1 310 12
provider:value("sprint", "Sprint") --3 310 120
provider:value("att", "AT&T") --2 310 410 wap.cingular
provider:value("tmobile", "T-Mobile") --4 310 260 epc.tmobile.com
provider:value("telcel", "TELCEL") --6 334 20 internet.itelcel.com webgprs webgprs2002
provider:value("movistar", "Movistar") --6 334 3 internet.movistar.mx movistar movistar
provider:value("iusacell", "Iusacell") --6 334 50 web.iusacellgsm.mx iusacellgsm iusacellgsm
provider:value("tigo_colombia", "Tigo Colombia") --6 732 103 web.colombiamovil.com.co
provider:value("movistar_columbia", "Movistar Colombia") --6 732 123 internet.movistar.com.co movistar movistar
provider:value("comcel", "Comcel") --6 732 101 internet.comcel.com.co comcel comcel
provider:value("vodafone", "Vodafone") --0 222 10 ppbundle.internet web web
provider:value("telefonica", "Telefonica") --7 234 15 wap.vodafone.co.uk wap wap
provider:value("telecom_italia", "Telecom Italia") --8 222 1 wap.tim.it WAPTIM WAPTIM
provider:value("orange", "Orange") --9 208 2 orange.fr
provider:value("gsm", translate("Other GSM/UTMS"))
provider:value("cdma", translate("Other CDMA/EV-DO"))
provider.default="att"

--provider_cdma = section:taboption("general", Value, "provider_cdma", translate("Provider"))
--provider_cdma:value("cdma", translate("Generic CDMA/EV-DO"))
--provider_cdma:value("verizon", "Verizon") --1 310 12
--provider_cdma:value("sprint", "Sprint") --3 310 120
--provider_cdma.default="verizon"
--provider_cdma:depends("radiotype", "cdma")

service = section:taboption("general", ListValue, "service", translate("Service Type"))
service:value("auto", "Automatic")
service:value("3g", translate("3G Only"))
service:value("2g", translate("2G Only"))
service.default="auto"

username = section:taboption("general", Value, "mcc", translate("Mobile Country Code (MCC)"))
username:depends("provider", "gsm")
username = section:taboption("general", Value, "mnc", translate("Mobile Network Code (MNC)"))
username:depends("provider", "gsm")

apn = section:taboption("general", Value, "apn", translate("APN"))
apn:depends("provider", "gsm")

--pincode = section:taboption("general", Value, "pincode", translate("PIN"))
--pincode:depends("provider", "gsm")

username = section:taboption("general", Value, "username", translate("PAP/CHAP username"))
username:depends("provider", "gsm")
username:depends("provider", "cdma")
username:depends("provider", "verizon")
username:depends("provider", "sprint")
password = section:taboption("general", Value, "password", translate("PAP/CHAP password"))
password.password = true
password:depends("provider", "gsm")
password:depends("provider", "cdma")
password:depends("provider", "verizon")
password:depends("provider", "sprint")

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


reboot_code = section:taboption("advanced", Value, "reboot_code", translate("SMS code to reboot router"),
	translate("If a received SMS message contains any part of this code the router will be rebooted"))

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
