local fs = require "nixio.fs"

m = Map("wwan", translate("Cellular/Wireless WAN Settings")) -- We want to edit the uci config file /etc/config/network
m:section(SimpleSection).template = "wwan_iface_overview"

m.pageaction = false

return m

