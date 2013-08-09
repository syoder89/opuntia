#!/bin/sh
#

source /etc/variable.file

local NAPN=$(uci get cbi_file.Network.apn)
local NUSER=$(uci get cbi_file.Network.user)
local NPASS=$(uci get cbi_file.Network.pass)
local NAUTH=$(uci get cbi_file.Network.auth)
local PINCODE=$(uci get cbi_file.Network.pincode)

if [ $QMIWWAN = 1 ]; then
	/etc/qmihelp start wwan1 $NAUTH $NAPN $NUSER $NPASS $PINCODE
else
	/etc/qmihelp start wwan0 $NAUTH $NAPN $NUSER $NPASS $PINCODE
fi
