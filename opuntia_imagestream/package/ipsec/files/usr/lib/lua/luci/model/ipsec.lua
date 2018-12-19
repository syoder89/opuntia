--[[
LuCI - IPSEC model

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

module "luci.model.ipsec"

local _uci_real, _uci_state
local _tunnels

function _get(c, s, o)
	return _uci_real:get(c, s, o)
end

function init(cursor)
	_uci_real  = cursor or _uci_real or uci.cursor()
	_uci_state = _uci_real:substate()

	_tunnels = { }
	return _M
end

function ip_xfrm_state(self)
	return utl.exec("ip xfrm state")
end

function ip_xfrm_policy(self)
	return utl.exec("ip xfrm policy")
end

function iproute(self)
	return utl.exec("ip route list table 220")
end

function del_tunnel(self, n)
	local r = _uci_real:delete("ipsec_pre", n)
	return r
end

function get_tunnels(self)
	local nets = { }
	local nls = { }

	_uci_real:foreach("ipsec_pre", "tunnel",
		function(s)
			nls[s['.index']] = {
				name = s['.name'],
				enabled = s['enabled'],
				local_subnet = s['local_subnet']
			}
			
		end)

	local n
	for n in utl.kspairs(nls) do
		nets[#nets+1] = nls[n]
	end

	return nets
end

function get_tunnel(self, n)
	local sec
	sec = _uci_real:get("ipsec_pre", n)
	if n and sec == "tunnel" then
		return sec
	end
end

function add_tunnel(self, n, options)
	local oldtunnel = self:get_tunnel(n)
	if n and #n > 0 and n:match("^[a-zA-Z0-9_]+$") and not oldtunnel then
		local s = _uci_real:section("ipsec_pre", "tunnel", n, options)
		if s then
			return s
		end
	elseif oldtunnel then
		if options then
			local k, v
			for k, v in pairs(options) do
				oldtunnel:set(k, v)
			end
		end
		return oldtunnel
	end
end


function _get_command_status(self, n)
	local result = { }
	local stat = ""
	for l in utl.execi("ipsec status remote_" .. n .. "-" .. n) do
		local r = utl.split(l, "\:", 1)
		if r[2] and #r[2] > 0 then
			stat = stat .. r[2] .. "<br>"
		end
	end

	if(stat == "") then
		local enabled = _uci_real:get("ipsec", "remote_" .. n, "enabled")
		if(enabled=="0") then
			stat = "Administratively shut down"
		else
			stat = "Not running"
		end
	end
	return stat
end

function get_tunnel_status(self, n)
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

