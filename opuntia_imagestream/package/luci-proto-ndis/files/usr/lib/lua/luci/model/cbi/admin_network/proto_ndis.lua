-- Copyright 2018 Scott Yoder <syoder@imagestream.com>
-- Licensed to the public under the Apache License 2.0.

local map, section, net = ...

local usb_idx, apn, pincode, username, password
local auth, ipv6


usb_idx = section:taboption("general", Value, "usb_idx", translate("mPCIe Slot"))
usb_idx.rmempty = false
usb_idx:value("", translate("-- Please choose --"))
usb_idx:value("1", "mPCIe3")
usb_idx:value("2", "mPCIe2")

mode = section:taboption("general", Value, "mode", translate("Service Type"))
mode.default = "auto"
mode:value("preferlte", translate("Prefer LTE"))
mode:value("preferumts", translate("Prefer UMTS"))
mode:value("lte", "LTE")
mode:value("umts", "UMTS/GPRS")
mode:value("auto", translate("auto"))

apn = section:taboption("general", Value, "apn", translate("APN"), translate("Usually leave APN blank - automatically set based on SIM"))


pincode = section:taboption("general", Value, "pincode", translate("PIN"))


username = section:taboption("general", Value, "username", translate("PAP/CHAP username"))


password = section:taboption("general", Value, "password", translate("PAP/CHAP password"))
password.password = true

auth = section:taboption("general", Value, "auth", translate("Authentication Type"))
auth:value("", translate("-- Please choose --"))
auth:value("3", "PAP/CHAP (both)")
auth:value("1", "PAP")
auth:value("2", "CHAP")
auth:value("0", "NONE")

if luci.model.network:has_ipv6() then
    ipv6 = section:taboption("advanced", Flag, "ipv6", translate("Enable IPv6 negotiation"))
    ipv6.default = ipv6.disabled
end
