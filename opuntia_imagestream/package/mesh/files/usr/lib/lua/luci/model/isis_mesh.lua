--[[
LuCI - MESH model

Copyright 2013 Juan Flores <juan.flores@gdc-cala.com.mx>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

]]--

local type, next, pairs, ipairs, loadfile, table
	= type, next, pairs, ipairs, loadfile, table

local tonumber, tostring, math = tonumber, tostring, math

local require = require
local print = print

local bus = require "ubus"
local nxo = require "nixio"
local nfs = require "nixio.fs"
local ipc = require "luci.ip"
local sys = require "luci.sys"
local utl = require "luci.util"
local dsp = require "luci.dispatcher"
local uci = require "luci.model.uci"
local lng = require "luci.i18n"

module "luci.model.isis_mesh"

local _uci_real, _uci_state
local _maps

function _get(c, s, o)
	return _uci_real:get(c, s, o)
end

function init(cursor)
	_uci_real  = cursor or _uci_real or uci.cursor()
	_uci_state = _uci_real:substate()

	_maps = { }
	return _M
end

function del_map(self, n)
	local r = _uci_real:delete("isis_mesh", n)
	return r
end

function get_maps(self)
	local nets = { }
	local nls = { }

	_uci_real:foreach("isis_mesh", "map",
		function(s)
			nls[s['.index']] = {
				name = s['.name'],
				authorized = _get("isis_mesh", s['.name'], "authorized"),
				macaddr = s['.name']:gsub("_", ":"),
				hostname = _get("isis_mesh", s['.name'], "hostname")
			}
			
		end)

	local n
	for n in utl.kspairs(nls) do
		nets[#nets+1] = nls[n]
	end

	return nets
end

function get_map(self, n)
	local sec
	sec = _uci_real:get("isis_mesh", n)
	if n and sec == "map" then
		return sec
	end
end

function add_map(self, n, options)
	n = n:gsub(":", "_")
	local oldmap = self:get_map(n)
	if n and #n > 0 and n:match("^[a-fA-F0-9_]+$") and not oldmap then
		local s = _uci_real:section("isis_mesh", "map", n, options)
		if s then
			return s
		end
	elseif oldmap then
		if options then
			local k, v
			for k, v in pairs(options) do
				oldmap:set(k, v)
			end
		end
		return oldmap
	end
end


function _get_command_status(self, n)
	local result = { }
	local stat = ""
--	for l in utl.execi("mesh status remote_" .. n .. "-" .. n) do
--		local r = utl.split(l, "\:", 1)
--		if r[2] and #r[2] > 0 then
--			stat = stat .. r[2] .. "<br>"
--		end
--	end

	if(stat == "") then
		stat = "Not available"
	end

	return stat
end

function get_map_status(self, n)
	local stat, output
	output = self:_get_command_status(n)
	stat = {
		id = n,
		status = output
	}
	return stat
end

function save(self, ...)
	_uci_real:save(...)
	_uci_real:load(...)
end

function commit(self, ...)
	_uci_real:commit(...)
	_uci_real:load(...)
end

