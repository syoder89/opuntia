#!/bin/sh

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

ConfigWifiIface() {
	iface_name="$1"
        config_get mode "$1" mode
        config_get device "$1" device
	if [ "$mode" == "ap" ] ; then
        	config_get ssid "$1" ssid
	elif [ "$mode" == "mesh" ] ; then
        	config_get mesh_id "$1" ssid
	else
        	config_get ssid "$1" ssid
	fi
        config_get network "$1" network
	json_add_object "$1"
	json_add_string "ssid" "$ssid"
	json_add_string "mode" "$mode"
	json_add_string "device" "$device"
	json_add_string "network" "$network"
	json_close_object
}

json_init
config_load wireless
config_foreach ConfigWifiIface wifi-iface
echo $(json_dump)
