--[[
LuCI - Lua Configuration IPSEC

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2008 Jo-Philipp Wich <xm@leipzig.freifunk.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id$
]]--

local fs = require "nixio.fs"

m = Map("isis_mesh", translate("WIFI Mesh Overview"))
m.pageaction = false
m:section(SimpleSection).template = "isis_mesh/map_overview"

return m
