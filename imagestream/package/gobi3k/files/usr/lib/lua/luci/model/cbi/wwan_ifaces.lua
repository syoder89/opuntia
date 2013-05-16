local fs = require "nixio.fs"
local ut = require "luci.util"
local pt = require "luci.tools.proto"
local nw = require "luci.model.network"
local fw = require "luci.model.firewall"

arg[1] = arg[1] or ""

local has_dnsmasq  = fs.access("/etc/config/dhcp")
local has_firewall = fs.access("/etc/config/firewall")

m = Map("wwan", translate("Cellular/Wireless WAN Settings")) -- We want to edit the uci config file /etc/config/network

if has_firewall then
	m:chain("firewall")
end

nw.init(m.uci)
fw.init(m.uci)


local net = nw:get_network(arg[1])

local function backup_ifnames(is_bridge)
	if not net:is_floating() and not m:get(net:name(), "_orig_ifname") then
		local ifcs = net:get_interfaces() or { net:get_interface() }
		if ifcs then
			local _, ifn
			local ifns = { }
			for _, ifn in ipairs(ifcs) do
				ifns[#ifns+1] = ifn:name()
			end
			if #ifns > 0 then
				m:set(net:name(), "_orig_ifname", table.concat(ifns, " "))
				m:set(net:name(), "_orig_bridge", tostring(net:is_bridge()))
			end
		end
	end
end


-- redirect to overview page if network does not exist anymore (e.g. after a revert)
if not net then
	luci.http.redirect(luci.dispatcher.build_url("admin/network/wwan"))
	return
end

-- dhcp setup was requested, create section and reload page
if m:formvalue("cbid.dhcp._enable._enable") then
	m.uci:section("dhcp", "dhcp", nil, {
		interface = arg[1],
		start     = "100",
		limit     = "150",
		leasetime = "12h"
	})

	m.uci:save("dhcp")
	luci.http.redirect(luci.dispatcher.build_url("admin/network/network", arg[1]))
	return
end

local ifc = net:get_interface()

s = m:section(NamedSection, arg[1], "interface", translate("Common Configuration"))
s.addremove = false

s:tab("general",  translate("General Setup"))
s:tab("advanced", translate("Advanced Settings"))
s:tab("physical", translate("Physical Settings"))

if has_firewall then
	s:tab("firewall", translate("Firewall Settings"))
end


st = s:taboption("general", DummyValue, "__status", translate("Status"))

local function set_status()
	-- if current network is empty, print a warning
	if not net:is_floating() and net:is_empty() then
		st.template = "cbi/dvalue"
		st.network  = nil
		st.value    = translate("There is no device assigned yet, please attach a network device in the \"Physical Settings\" tab")
	else
		st.template = "admin_network/iface_status"
		st.network  = arg[1]
		st.value    = nil
	end
end

m.on_init = set_status
m.on_after_save = set_status


p = s:taboption("general", ListValue, "proto", translate("Protocol"))
p.default = net:proto()


if not net:is_installed() then
	p_install = s:taboption("general", Button, "_install")
	p_install.title      = translate("Protocol support is not installed")
	p_install.inputtitle = translate("Install package %q" % net:opkg_package())
	p_install.inputstyle = "apply"
	p_install:depends("proto", net:proto())

	function p_install.write()
		return luci.http.redirect(
			luci.dispatcher.build_url("admin/system/packages") ..
			"?submit=1&install=%s" % net:opkg_package()
		)
	end
end


p_switch = s:taboption("general", Button, "_switch")
p_switch.title      = translate("Really switch protocol?")
p_switch.inputtitle = translate("Switch protocol")
p_switch.inputstyle = "apply"

local _, pr
for _, pr in ipairs(nw:get_protocols()) do
	p:value(pr:proto(), pr:get_i18n())
	if pr:proto() ~= net:proto() then
		p_switch:depends("proto", pr:proto())
	end
end


auto = s:taboption("advanced", Flag, "auto", translate("Bring up on boot"))
auto.default = (net:proto() == "none") and auto.disabled or auto.enabled

if has_firewall then
	fwzone = s:taboption("firewall", Value, "_fwzone",
		translate("Create / Assign firewall-zone"),
		translate("Choose the firewall zone you want to assign to this interface. Select <em>unspecified</em> to remove the interface from the associated zone or fill out the <em>create</em> field to define a new zone and attach the interface to it."))

	fwzone.template = "cbi/firewall_zonelist"
	fwzone.network = arg[1]
	fwzone.rmempty = false

	function fwzone.cfgvalue(self, section)
		self.iface = section
		local z = fw:get_zone_by_network(section)
		return z and z:name()
	end

	function fwzone.write(self, section, value)
		local zone = fw:get_zone(value)

		if not zone and value == '-' then
			value = m:formvalue(self:cbid(section) .. ".newzone")
			if value and #value > 0 then
				zone = fw:add_zone(value)
			else
				fw:del_network(section)
			end
		end

		if zone then
			fw:del_network(section)
			zone:add_network(section)
		end
	end
end


function p.write() end
function p.remove() end
function p.validate(self, value, section)
	if value == net:proto() then
		if not net:is_floating() and net:is_empty() then
			local ifn = ((br and (br:formvalue(section) == "bridge"))
				and ifname_multi:formvalue(section)
			     or ifname_single:formvalue(section))

			for ifn in ut.imatch(ifn) do
				return value
			end
			return nil, translate("The selected protocol needs a device assigned")
		end
	end
	return value
end


local form, ferr = loadfile(
	ut.libpath() .. "/model/cbi/admin_network/proto_%s.lua" % net:proto()
)

if not form then
	s:taboption("general", DummyValue, "_error",
		translate("Missing protocol extension for proto %q" % net:proto())
	).value = ferr
else
	setfenv(form, getfenv(1))(m, s, net)
end


local _, field
for _, field in ipairs(s.children) do
	if field ~= st and field ~= p and field ~= p_install and field ~= p_switch then
		if next(field.deps) then
			local _, dep
			for _, dep in ipairs(field.deps) do
				dep.deps.proto = net:proto()
			end
		else
			field:depends("proto", net:proto())
		end
	end
end


t = s:taboption("general", ListValue, "radiotype", "Radio Type") -- Creates an element list (select box)
t:value("cdma", "CDMA") -- Key and value pairs
t:value("umts", "GSM/UMTS")
t.default = "umts"

apn = s:taboption("general", Value, "apn", "APN", "Network APN")
apn:depends("radiotype", "umts");
username = s:taboption("general", Value, "username", "Username", "")
password = s:taboption("general", Value, "password", "Password", "")

return m
