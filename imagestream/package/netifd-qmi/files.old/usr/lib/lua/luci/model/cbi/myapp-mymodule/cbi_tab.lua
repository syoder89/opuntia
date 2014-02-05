m = Map("cbi_file", translate("Modem Connection Information"), translate("Please fill out the entries below"))
d = m:section(TypedSection, "info", "APN, User Name and Password")
a = d:option(Value, "apn", "APN :"); 
a.optional=false; 
a.rmempty = true;

b = d:option(Value, "user", "User Name :"); 
b.optional=false; 
b.rmempty = true;

c = d:option(Value, "pass", "Password :"); 
c.optional=false; 
c.rmempty = true;
c.password = true

c2 = d:option(Value, "pincode", "PIN :"); 
c2.optional=false; 
c2.rmempty = true;

p2 = d:option(ListValue, "auth", "Authentication Protocol :")
p2:value("0", "None")
p2:value("1", "PAP")
p2:value("2", "CHAP")
p2.default = "0"

c1 = d:option(ListValue, "alive", "Connection Keep Alive :");
c1:value("0", "Disabled")
c1:value("1", "Enabled")
c1:value("3", "Enabled with WAN Restart")
c1:value("2", "Enabled with Router Reboot")
c1:value("4", "Enabled with Modem Toggle")
c1.default=0

ca3 = d:option(Value, "pingtime", "Ping Interval in Minutes :"); 
ca3.optional=false; 
ca3.rmempty = true;
ca3.datatype = "and(uinteger,min(1))"
ca3.default=2
ca3:depends("alive", "1")
ca3:depends("alive", "2")
ca3:depends("alive", "3")
ca3:depends("alive", "4")

ca4 = d:option(Value, "pingwait", "Ping Wait in Seconds :"); 
ca4.optional=false; 
ca4.rmempty = true;
ca4.datatype = "and(uinteger,min(1))"
ca4.default=10
ca4:depends("alive", "1")
ca4:depends("alive", "2")
ca4:depends("alive", "3")
ca4:depends("alive", "4")

ca5 = d:option(Value, "pingnum", "Number of Packets :"); 
ca5.optional=false; 
ca5.rmempty = true;
ca5.datatype = "and(uinteger,min(1))"
ca5.default=1
ca5:depends("alive", "1")
ca5:depends("alive", "2")
ca5:depends("alive", "3")
ca5:depends("alive", "4")

cb2 = d:option(Value, "pingserv1", "Ping Server 1 :"); 
cb2.rmempty = true;
cb2.optional=false;
cb2.datatype = "ipaddr"
cb2:depends("alive", "1")
cb2:depends("alive", "2")
cb2:depends("alive", "3")
cb2:depends("alive", "4")
cb2.default="8.8.8.8"

cb3 = d:option(Value, "pingserv2", "Ping Server 2 :"); 
cb3.rmempty = true;
cb3.optional=false;
cb3.datatype = "ipaddr"
cb3:depends("alive", "1")
cb3:depends("alive", "2")
cb3:depends("alive", "3")
cb3:depends("alive", "4")

c10 = d:option(ListValue, "ppp", "Force Modem to PPP Protocol :");
c10:value("0", "No")
c10:value("1", "Yes")
c10.default=0

c3 = d:option(Value, "delay", "Connection Delay in Seconds :"); 
c3.optional=false; 
c3.rmempty = false;
c3.datatype = "and(uinteger,min(60))"

d1 = m:section(TypedSection, "dns", "Custom DNS Servers")
b1 = d1:option(Value, "dns1", "Custom DNS Server1 :"); 
b1.rmempty = true;
b1.optional=false;
b1.datatype = "ipaddr"

b2 = d1:option(Value, "dns2", "Custom DNS Server2 :"); 
b2.rmempty = true;
b2.optional=false;
b2.datatype = "ipaddr"

d2 = m:section(TypedSection, "version", "Script Version")
b3 = d2:option(DummyValue, "ver", "Codename and Date :");

return m