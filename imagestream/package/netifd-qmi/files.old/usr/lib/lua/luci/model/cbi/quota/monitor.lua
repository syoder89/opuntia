local wa = require "luci.tools.webadmin"

function Split(str, delim, maxNb)
    -- Eliminate bad cases...
    if string.find(str, delim) == nil then
        return { str }
    end
    if maxNb == nil or maxNb < 1 then
        maxNb = 0    -- No limit
    end
    local result = {}
    local pat = "(.-)" .. delim .. "()"
    local nb = 0
    local lastPos
    for part, pos in string.gfind(str, pat) do
        nb = nb + 1
        result[nb] = part
        lastPos = pos
        if nb == maxNb then break end
    end
    -- Handle the last field
    if nb ~= maxNb then
        result[nb + 1] = string.sub(str, lastPos)
    end
    return result
end

local upload = 0
local download = 0
local file = io.open("/tmp/usage.db", "r")
if file ~= nil then
	repeat
		local line = file:read("*line")
		if line == nil then
			break
		end
		local t = Split(line, ",", 6)
		download = download + t[2]
		upload = upload + t[3]
	until 1 == 0
	file:close()
end
total = upload + download
if total < 1000 then
	tstr = string.format("%d", total)
	tfm = "KB"
else
	if total < 1000000 then
		tstr = string.format("%g", total/1000)
		tfm = "MB"
	else
		tstr = string.format("%g", total/1000000)
		tfm = "GB"
	end
end
if upload < 1000 then
	upstr = string.format("%d", upload)
	ufm = "KB"
else
	if upload < 1000000 then
		upstr = string.format("%g", upload/1000)
		ufm = "MB"
	else
		upstr = string.format("%g", upload/1000000)
		ufm = "GB"
	end
end
if download < 1000 then
	dwnstr = string.format("%d", download)
	dfm = "KB"
else
	if download < 1000000 then
		dwnstr = string.format("%g", download/1000)
		dfm = "MB"
	else
		dwnstr = string.format("%g", download/1000000)
		dfm = "GB"
	end
end

os.execute("/etc/wrtbwmon writedl " .. dwnstr .. " " .. dfm .. " " .. upstr .. " " .. ufm .. " " .. tstr .. " " .. tfm)

m = Map("monitor", translate("Bandwidth Monitoring"),
	translate("Monitor Bandwidth Usage over a Specified Period"))

m.on_after_commit = function(self)
	os.execute("/etc/monitor")
end

s = m:section(TypedSection, "monitor", translate("Bandwidth Usage Settings"))
s.addremove = false
s.anonymous = false

p2 = s:option(ListValue, "backup", "Backup Usage Database Every :")
p2:value("0", "--Never--")
p2:value("1", "Hour")
p2:value("2", "2 hours")
p2:value("3", "3 hours")
p2:value("4", "4 hours")
p2:value("5", "5 hours")
p2:value("6", "6 hours")
p2:value("7", "7 hours")
p2:value("8", "8 hours")
p2:value("9", "9 hours")
p2:value("10", "10 hours")
p2:value("11", "11 hours")
p2:value("12", "12 hours")
p2:value("13", "13 hours")
p2:value("14", "14 hours")
p2:value("15", "15 hours")
p2:value("16", "16 hours")
p2:value("17", "17 hours")
p2:value("18", "18 hours")
p2:value("19", "19 hours")
p2:value("20", "20 hours")
p2:value("21", "21 hours")
p2:value("22", "22 hours")
p2:value("23", "23 hours")
p2:value("24", "24 hours")
p2.default = "4"

p6 = s:option(ListValue, "mon_reset", "Usage Monitoring Resets :")
p6:value("never", "--Never--")
p6:value("hour", "Every Hour")
p6:value("day", "Every Day")
p6:value("week", "Every Week")
p6:value("month", "Every Month")
p6.default = "month"

daym = s:option(ListValue, "day_mon", translate("Reset Day of Month :"))
daym.rmempty = true
daym:value("1", "1st")
daym:value("2", "2nd")
daym:value("3", "3rd")
daym:value("4", "4th")
daym:value("5", "5th")
daym:value("6", "6th")
daym:value("7", "7th")
daym:value("8", "8th")
daym:value("9", "9th")
daym:value("10", "10th")
daym:value("11", "11th")
daym:value("12", "12th")
daym:value("13", "13th")
daym:value("14", "14th")
daym:value("15", "15th")
daym:value("16", "16th")
daym:value("17", "17th")
daym:value("18", "18th")
daym:value("19", "19th")
daym:value("20", "20th")
daym:value("21", "21st")
daym:value("22", "22nd")
daym:value("23", "23rd")
daym:value("24", "24th")
daym:value("25", "25th")
daym:value("26", "26th")
daym:value("27", "27th")
daym:value("28", "28th")
daym:value("29", "29th")
daym:value("30", "30th")
daym:value("31", "31st")
daym:depends("mon_reset", "month")
daym.default = "1"

dayw = s:option(ListValue, "day_wk", translate("Reset Day of Week :"))
dayw.rmempty = true
dayw:value("1", "Monday")
dayw:value("2", "Tueday")
dayw:value("3", "Wednesday")
dayw:value("4", "Thursday")
dayw:value("5", "Friday")
dayw:value("6", "Saturday")
dayw:value("0", "Sunday")
dayw:depends("mon_reset", "week")
dayw.default = "1"

hour = s:option(ListValue, "hour", translate("Reset Hour :"))
hour.rmempty = true
hour:value("0", "12:00 AM")
hour:value("1", "01:00 AM")
hour:value("2", "02:00 AM")
hour:value("3", "03:00 AM")
hour:value("4", "04:00 AM")
hour:value("5", "05:00 AM")
hour:value("6", "06:00 AM")
hour:value("7", "07:00 AM")
hour:value("8", "08:00 AM")
hour:value("9", "09:00 AM")
hour:value("10", "10:00 AM")
hour:value("11", "11:00 AM")
hour:value("12", "12:00 PM")
hour:value("13", "01:00 PM")
hour:value("14", "02:00 PM")
hour:value("15", "03:00 PM")
hour:value("16", "04:00 PM")
hour:value("17", "05:00 PM")
hour:value("18", "06:00 PM")
hour:value("19", "07:00 PM")
hour:value("20", "08:00 PM")
hour:value("21", "09:00 PM")
hour:value("22", "10:00 PM")
hour:value("23", "11:00 PM")
hour:depends("mon_reset", "month")
hour:depends("mon_reset", "week")
hour:depends("mon_reset", "day")
hour.default = "0"

s1 = m:section(TypedSection, "monitor", translate("Bandwidth Usage Amounts in Monitored Period"))
s1.addremove = false
s1.anonymous = false

upl = s1:option(DummyValue, "up", translate("Total Uploaded Amount :"))
dwnl = s1:option(DummyValue, "down", translate("Total Downloaded Amount :"))
totl = s1:option(DummyValue, "total", translate("Total Bandwidth Amount :"))
ototl = s1:option(DummyValue, "oldtot", translate("Previous Period Total Bandwidth Amount :"))

m:section(SimpleSection).template = "myapp-mymodule/user"

return m
