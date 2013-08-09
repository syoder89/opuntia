module("luci.controller.admin.modem", package.seeall)

function index() 
	entry({"admin", "modem"}, firstchild(), "Modem", 30).dependent=false
	entry({"admin", "modem", "tab_from_cbi"}, cbi("myapp-mymodule/cbi_tab", {autoapply=true}), "Connection", 1)
	entry({"admin", "modem", "tab2_from_cbi"}, template("myapp-mymodule/cbi_tab1"), "Information", 2)
	entry({"admin", "modem", "tab3_from_cbi"}, cbi("myapp-mymodule/cbi_tab1"), "Support", 3)
	entry({"admin", "modem", "tab4_from_cbi"}, template("myapp-mymodule/cbi_tab2"), "Network Status", 4)
	entry({"admin", "modem", "tab5_from_cbi"}, cbi("myapp-mymodule/cbi_tab2"), "Custom", 5)
	entry({"admin", "modem", "tab6_from_cbi"}, template("myapp-mymodule/cbi_tab4"), "SMS Messaging", 6)
	entry({"admin", "modem", "get_csq"}, call("action_get_csq"))
	entry({"admin", "modem", "toggle_log"}, call("action_toggle_log"))
	entry({"admin", "modem", "change_port"}, call("action_change_port"))
	entry({"admin", "modem", "change_mode"}, call("action_change_mode"))
	entry({"admin", "modem", "check_read"}, call("action_check_read"))
	entry({"admin", "modem", "del_sms"}, call("action_del_sms"))
	entry({"admin", "modem", "send_sms"}, call("action_send_sms"))
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function action_send_sms()
	local set = luci.http.formvalue("set")
	number = trim(string.sub(set, 1, 20))
	txt = string.sub(set, 21)
	msg = string.gsub(txt, "\n", " ")
	os.execute("/etc/sendsms " .. number .. " " .. "\"\13" .. msg .. "\26\"")
end

function action_del_sms()
	local set = tonumber(luci.http.formvalue("set"))
	if set ~= nil and set > 0 then
		set = set - 1;
		os.execute("/etc/delsms " .. set)
	end
end

function action_check_read()
	local rv ={}
	getsig = luci.model.uci.cursor():get("cbi_file", "Version", "getsig")
	if getsig == "0" then
		rv["ready"] = "2"
	else
		smsread = luci.model.uci.cursor():get("cbi_file", "Version", "smsread")
		if smsread == "0" then
			rv["ready"] = "1"
			full = nil
			local file = io.open("/tmp/smsresult.at", "r")
			if file == nil then
				rv["ready"] = "0"
			else
				repeat
					line = file:read("*line")
					if line ~= nil then
						if full == nil then
							full = line
						else
							full = full .. "|" .. line 
						end
					end
				until line == nil
				file:close()

				rv["line"] = full
			end 
		else
			rv["ready"] = "0"
		end
	end

	--os.execute("/etc/qmiprint.sh Doing Check read")

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function lshift(x, by)
  return x * 2 ^ by
end

function rshift(x, by)
  return math.floor(x / 2 ^ by)
end

function action_change_mode()
	local set = tonumber(luci.http.formvalue("set"))
	local modemtype = rshift(set, 4)
	local temp = lshift(modemtype, 4)
	local netmode = set - temp
	os.execute("/etc/modechge " .. modemtype .. " " .. netmode)
end

function action_change_port()
	local set = tonumber(luci.http.formvalue("set"))
	if set ~= nil and set > 0 then
		if set == 1 then
			os.execute("/etc/portchge dwn")
		else
			os.execute("/etc/portchge up")
		end
	end
end

function action_get_csq()
	local file = io.open("/tmp/status.file", "r")

	local rv ={}

	rv["port"] = file:read("*line")
	rv["csq"] = file:read("*line")
	rv["per"] = file:read("*line")
	rv["rssi"] = file:read("*line")
	rv["modem"] = file:read("*line")
	rv["cops"] = file:read("*line")
	rv["mode"] = file:read("*line")
	rv["lac"] = file:read("*line")
	rv["lacn"] = file:read("*line")
	rv["cid"] = file:read("*line")
	rv["cidn"] = file:read("*line")
	rv["mcc"] = file:read("*line")
	rv["mnc"] = file:read("*line")
	rv["rnc"] = file:read("*line")
	rv["rncn"] = file:read("*line")
	rv["down"] = file:read("*line")
	rv["up"] = file:read("*line")
	rv["ecio"] = file:read("*line")
	rv["rscp"] = file:read("*line")
	rv["ecio1"] = file:read("*line")
	rv["rscp1"] = file:read("*line")
	rv["netmode"] = file:read("*line")
	rv["cell"] = file:read("*line")
	rv["modtype"] = file:read("*line")

	file:close()

	cmode = luci.model.uci.cursor():get("cbi_file", "Version", "cmode")
	if cmode == "0" then
		rv["netmode"] = "10"
	end	
	rv["log"] = luci.model.uci.cursor():get("cbi_file", "Version", "log")

	rssi = rv["rssi"]
	ecio = rv["ecio"]
	rscp = rv["rscp"]
	ecio1 = rv["ecio1"]
	rscp1 = rv["rscp1"]

	if ecio ~= "-" then
		rv["ecio"] = ecio .. " dB"
	end
	if rscp ~= "-" then
		rv["rscp"] = rscp .. " dBm"
	end
	if ecio1 ~= " " then
		rv["ecio1"] = " (" .. ecio1 .. " dB)"
	end
	if rscp1 ~= " " then
		rv["rscp1"] = " (" .. rscp1 .. " dBm)"
	end

	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

function action_toggle_log()
	os.execute("/etc/togglelog")
	local rv ={}
	rv["log"] = luci.model.uci.cursor():get("cbi_file", "Version", "log")
	luci.http.prepare_content("application/json")
	luci.http.write_json(rv)
end

