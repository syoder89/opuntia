module("luci.controller.wwan", package.seeall)

function index()
	entry({"admin", "network", "wwan"}, cbi("wwan"), _("Cellular/Wireless WAN"), 20)
--	entry({"admin", "network", "wwan"}, arcombine(template("wwan_overview"),cbi("wwan"), _("Cellular/Wireless WAN"), 20))
end
