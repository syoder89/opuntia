#!/bin/sh
INCLUDE_ONLY=1

. ../netifd-proto.sh
. ./ppp.sh
init_proto "$@"

save_var() {
	rm -f /etc/variable.file
	KEEPALIVE=1
	echo 'NETCREATE="'"$NETCREATE"'"' >> /etc/variable.file
	echo 'NETSIERRA="'"$NETSIERRA"'"' >> /etc/variable.file
	echo 'SIERRACTIVE="'"$SIERRACTIVE"'"' >> /etc/variable.file
	echo 'QMIWWAN="'"$QMIWWAN"'"' >> /etc/variable.file
	echo 'KEEPALIVE="'"$KEEPALIVE"'"' >> /etc/variable.file
	echo 'RBDELAY="'"$RBDELAY"'"' >> /etc/variable.file
}

proto_3g_init_config() {
	no_device=1
	available=1
	ppp_generic_init_config
	proto_config_add_string "device"
	proto_config_add_string "apn"
	proto_config_add_string "service"
	proto_config_add_string "pincode"
}

proto_3g_setup() {
	local interface="$1"
	local chat

	if [ ! -f /tmp/bootend.file ]; then
		return 0
	fi

	json_get_var device device
	json_get_var apn apn
	json_get_var service service
	json_get_var pincode pincode

	[ -e "$device" ] || {
		proto_set_available "$interface" 0
		return 1
	}

	case "$service" in
		cdma|evdo)
			chat="/etc/chatscripts/evdo.chat"
		;;
		*)
			chat="/etc/chatscripts/3g.chat"
			cardinfo=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom)
			if echo "$cardinfo" | grep -q Novatel; then
				case "$service" in
					umts_only) CODE=2;;
					gprs_only) CODE=1;;
					*) CODE=0;;
				esac
				export MODE="AT\$NWRAT=${CODE},2"
			elif echo "$cardinfo" | grep -q Option; then
				case "$service" in
					umts_only) CODE=1;;
					gprs_only) CODE=0;;
					*) CODE=3;;
				esac
				export MODE="AT_OPSYS=${CODE}"
			elif echo "$cardinfo" | grep -q "Sierra Wireless"; then
				SIERRA=1
			elif echo "$cardinfo" | grep -qi huawei; then
				case "$service" in
					umts_only) CODE="14,2";;
					gprs_only) CODE="13,1";;
					*) CODE="2,2";;
				esac
				export MODE="AT^SYSCFG=${CODE},3FFFFFFF,2,4"
			fi

			if [ -n "$pincode" ]; then
				PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
					proto_notify_error "$interface" PIN_FAILED
					proto_block_restart "$interface"
					return 1
				}
			fi
			[ -n "$MODE" ] && gcom -d "$device" -s /etc/gcom/setmode.gcom

			# wait for carrier to avoid firmware stability bugs
			[ -n "$SIERRA" ] && {
				gcom -d "$device" -s /etc/gcom/getcarrier.gcom || return 1
			}
		;;
	esac

	source /etc/variable.file
	pkill getsig
	uci set cbi_file.Version.getsig="0"
	uci commit cbi_file

	connect="${apn:+USE_APN=$apn }/usr/sbin/chat -t5 -v -E -f $chat"

	save_var
	/etc/getsig &

	ppp_generic_setup "$interface" \
		noaccomp \
		nopcomp \
		novj \
		nobsdcomp \
		noauth \
		lock \
		crtscts \
		115200 "$device"
	return 0
}

proto_3g_teardown() {
	proto_kill_command "$interface"
}

add_protocol 3g
