local netmod = luci.model.network

local _, p
for _, p in ipairs({"qmi"}) do

	local proto = netmod:register_protocol(p)

	function proto.get_i18n(self)
		if p == "qmi" then
			return luci.i18n.translate("WWAN/Cellular")
		end
	end

	function proto.ifname(self)
		return self:_ubus("device")
	end

	function proto.is_installed(self)
		return true
	end

	function proto.is_floating(self)
		return false
	end

	function proto.is_virtual(self)
		return false
	end

	function proto.get_interfaces(self)
		return netmod.protocol.get_interfaces(self)
	end

	function proto.contains_interface(self, ifc)
		return netmod.protocol.contains_interface(self, ifc)
	end

end

