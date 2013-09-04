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

m = Map("ipsec_pre", translate("IPSEC"))
m.pageaction = false
m:section(SimpleSection).template = "ipsec/tunnel_overview"

return m
