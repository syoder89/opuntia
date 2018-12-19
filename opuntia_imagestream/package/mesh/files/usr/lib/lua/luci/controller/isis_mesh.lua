--[[
LuCI - Lua Configuration MESH

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: 
]]--

module("luci.controller.isis_mesh", package.seeall)

function index()
	local page
	if not nixio.fs.access("/etc/config/isis_mesh") then
		return
	end

	page = node("admin", "network", "isis_mesh")
	page.target = cbi("isis_mesh/overview")
	page.title  = _("WIFI Mesh")
	page.index  = true

	page = entry({"admin", "network", "isis_mesh", "advanced_status"}, call("isis_mesh_advanced_status"), "MESH Advanced Status")

	page = entry({"admin", "network", "isis_mesh", "map_form"}, cbi("isis_mesh/map_form", {autoapply=true}), nil)
	page.leaf = true

	page = entry({"admin", "network", "isis_mesh", "map_delete"}, call("map_delete"), nil)
	page.leaf = true

	page = entry({"admin", "network", "isis_mesh", "map_status"}, call("map_status"), nil)
	page.leaf = true
end

function isis_mesh_advanced_status()
	local ipm = require "luci.model.isis_mesh".init()
	local ip_xfrm_state = ipm:ip_xfrm_state()
	local ip_xfrm_policy = ipm:ip_xfrm_policy()
	local iproute = ipm:iproute()
	luci.template.render("isis_mesh/advanced_status", 
		{iproute=iproute})
end

function map_delete(map)
	local ipm = require "luci.model.isis_mesh".init()
	local tmp = ipm:del_map(map)
	if tmp then
		ipm:commit("isis_mesh")
		luci.http.redirect(luci.dispatcher.build_url("admin/network/isis_mesh"))
		return
	end

	luci.http.status(404, "No such map")
end

function map_status(maps)
	local ipm = require "luci.model.isis_mesh".init()
	local rv   = { }

	local map
	for map in maps:gmatch("[%w%.%-_]+") do
		local stat = ipm:get_map_status(map)
		rv[#rv+1] = {
			name = map,
			status = stat.status,
		}
	end
	
	if #rv > 0 then
		luci.http.prepare_content("application/json")
		luci.http.write_json(rv)
		return
	end

	luci.http.status(404, "No such device")
end
