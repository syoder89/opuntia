#!/bin/sh
# gre.sh - IPv6-in-IPv4 tunnel backend
# Copyright (c) 2010-2012 OpenWrt.org

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. /lib/functions/network.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_gre_log() {
        local level="$1"
        local message="$2"
        [ -z "$message" ] && {
                logger -p "$1" -t "gre[$$]"
                return;
        }
        shift
        logger -p "$level" -t "gre[$$]" -- "$@"
}

proto_gre_setup() {
	local cfg="$1"
	local iface="$2"
	local link="$cfg"

	json_get_vars source destination keepalive keepalive_interval keepalive_retries key sequencing ttl ipaddr ptpaddr

	if [ "$iface" = "" ] ; then
		iface="$link"
		uci set network.${cfg}.iface="$iface"
		uci commit network.${cfg}.ifname
	fi

	proto_init_update "$iface" 1

	proto_add_ipv4_address "$ipaddr" "" "" "$ptpaddr"
	proto_add_tunnel
	json_add_string mode gre
	json_add_int mtu "${mtu:-1280}"
	json_add_int ttl "${ttl:-64}"
	[[ "$local" != "" ]] && json_add_string local "$source"
	json_add_string remote "$destination"
	[[ "$key" != "" ]] && json_add_string key "$key"
	[[ "$sequencing" != "" ]] && json_add_string seq
	[[ "$keepalive" = "1" ]] && json_add_string keepalive "$keepalive_interval retries $keepalive_retries"
	proto_close_tunnel

	[[ "$local" != "" ]] && local="local $source"
	[[ "$key" != "" ]] && key="key $key"
	[[ "$sequencing" != "" ]] && sequencing="seq"
	[[ "$keepalive" = "1" ]] && keepalive="keepalive $keepalive_interval retries $keepalive_retries"
	[[ "$ttl" != "" ]] && ttl="ttl $ttl"

	cmd="/usr/sbin/ip \
		tunnel add $iface mode gre remote $destination $local \
		$key $sequencing $keepalive $ttl"
	$cmd 2>&1 | proto_gre_log daemon.debug
	
	if [ "$?" != "0" ] ; then
		proto_gre_log daemon.err "Failed to add ip tunnel. Cmd: $cmd"
	fi

	proto_send_update "$cfg" 2>&1 | proto_gre_log daemon.debug
	/usr/sbin/ip link set $iface up
}

proto_gre_teardown() {
	local cfg="$1"

	iface="$cfg"
	/usr/sbin/ip link set $iface down
	/usr/sbin/ip tunnel del $iface
}

proto_gre_init_config() {
	no_device=1
	available=1

	proto_config_add_string "source"
	proto_config_add_string "destination"
	proto_config_add_boolean "keepalive"
	proto_config_add_int "keepalive_interval"
	proto_config_add_int "keepalive_retries"
	proto_config_add_string "key"
	proto_config_add_boolean "sequencing"
	proto_config_add_string "ttl"
	proto_config_add_string "mtu"
	proto_config_add_string "ipaddr"
	proto_config_add_string "ptpaddr"
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol gre
}
