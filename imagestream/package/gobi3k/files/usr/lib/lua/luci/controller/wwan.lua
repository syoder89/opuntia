module("luci.controller.wwan", package.seeall)

function index()
        local uci = require("luci.model.uci").cursor()
	local has_wwan = false

	uci:foreach("wwan", "wwan-iface",
		function(s)
			has_wwan = true
			return false
		end)

	if has_wwan then
		entry({"admin", "network", "wwan"}, cbi("wwan"), _("Cellular/Wireless WAN"), 20)
	end
--	entry({"admin", "network", "wwan"}, arcombine(template("wwan_overview"),cbi("wwan"), _("Cellular/Wireless WAN"), 20))
end
