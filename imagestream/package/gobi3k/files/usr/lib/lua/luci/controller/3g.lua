module("luci.controller.3g", package.seeall)

function index()
	entry({"admin", "network", "3g"},
		alias("admin", "network", "3g", "general"),
		_("3G"), 60).i18n = "3G"

	entry({"admin", "network", "3g", "general"},
		cbi("3g"),
		_("General Settings"), 10).leaf = true
end
