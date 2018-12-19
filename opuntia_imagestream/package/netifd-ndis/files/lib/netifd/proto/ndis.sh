#!/bin/sh

[ -n "$INCLUDE_ONLY" ] || {
	. /lib/functions.sh
	. ../netifd-proto.sh
	init_proto "$@"
}

proto_ndis_init_config() {
	no_device=1
	available=1
	proto_config_add_string "device:device"
	proto_config_add_string apn
	proto_config_add_string auth
	proto_config_add_string username
	proto_config_add_string password
	proto_config_add_string pincode
	proto_config_add_string delay
	proto_config_add_string mode
	proto_config_add_string pdptype
	proto_config_add_string usb_idx
	proto_config_add_int profile
	proto_config_add_defaults
}

proto_ndis_log() {
        local level="$1"
        local message="$2"
        [ -z "$message" ] && {
                logger -p "$1" -t "ndis[$$]"
                return;
        }
        shift
        logger -p "$level" -t "ndis[$$]" -- "$@"
}

proto_ndis_get_device() {
        devpath="$device"
        [ -n "$devpath" ] || {
                [ -n "$usb_idx" ] || return 1
		echo "Finding device for usb index $usb_idx..."
                devpath=$(echo /sys/devices/pci*/*/usb${usb_idx}/*-*/*/*/usbmisc/cdc-wdm*)
                devpath="${devpath}/../../.."
        }

	device="$(readlink -f $devpath/*.2/ttyUSB*)"
	[ -e "$device" ] || return 1
	device="/dev/$(basename $device)"
	echo "Using device $device"
}

proto_ndis_show_status() {
	# Show status
        rm -f /tmp/man3g-$interface.cache && man3g -i $interface status | proto_ndis_log daemon.debug
}

proto_ndis_setup() {
	local interface="$1"

	local manufacturer initialize setmode connect finalize ifname devname devpath getaddrs

	local device apn auth username password pincode delay mode pdptype profile $PROTO_DEFAULT_OPTIONS
	json_get_vars device apn auth username password pincode delay mode pdptype profile usb_idx $PROTO_DEFAULT_OPTIONS

	[ "$metric" = "" ] && metric="0"

	[ -n "$profile" ] || profile=1
	[ -n "$delay" ] || delay=1

	pdptype=`echo "$pdptype" | awk '{print toupper($0)}'`
	[ "$pdptype" = "IP" -o "$pdptype" = "IPV6" -o "$pdptype" = "IPV4V6" ] || pdptype="IP"

	[ -n "$ctl_device" ] && device=$ctl_device

	proto_ndis_get_device

	[ -e "$device" ] || {
		echo "Control device not valid trying again in 5 seconds..."
		sleep 5
		proto_ndis_get_device
		[ -e "$device" ] || {
			echo "Control device not valid"
			proto_set_available "$interface" 0
			return 1
		}
	}

	devname="$(basename "$device")"
	ifname="$( ls "$devpath"/*.5/net )"
	# On module reset the device might not be present yet
	[ -n "$ifname" ] || {
		sleep 1
		ifname="$( ls "$devpath"/*.5/net )"
	}
	[ -n "$ifname" ] || {
		echo "The interface could not be found."
		proto_notify_error "$interface" NO_IFACE
#		proto_set_available "$interface" 0
		return 1
	}

	proto_ndis_led_connecting ${interface}
	[ -n "$delay" ] && sleep "$delay"

	do_lock
	manufacturer=`gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk 'NF && $0 !~ /AT\+CGMI/ { sub(/\+CGMI: /,""); print tolower($1); exit; }'`
	[ -n "$manufacturer" ] || {
		do_unlock
		echo "Failed to get modem information from $device"
		proto_notify_error "$interface" GETINFO_FAILED
		return 1
	}
	echo "Manufacturer is \"$manufacturer\""

	json_load "$(cat /etc/gcom/ndis.json)"
	json_select "$manufacturer"
	[ $? -ne 0 ] && {
		do_unlock
		echo "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
#		proto_set_available "$interface" 0
		return 1
	}
	json_get_values initialize initialize
	for i in $initialize; do
		eval COMMAND="$i" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			do_unlock
			echo "Failed to initialize modem"
			proto_notify_error "$interface" INITIALIZE_FAILED
			return 1
		}
	done

	[ -n "$pincode" ] && {
		PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
			do_unlock
			echo "Unable to verify PIN"
			proto_notify_error "$interface" PIN_FAILED
			proto_block_restart "$interface"
			return 1
		}
	}

	json_get_values configure configure
	echo "Configuring modem"
	for i in $configure; do
		eval COMMAND="$i" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			do_unlock
			echo "Failed to configure modem"
			proto_notify_error "$interface" CONFIGURE_FAILED
			return 1
		}
	done

	[ -n "$mode" ] && {
		json_select modes
		json_get_var setmode "$mode"
		echo "Setting mode"
		eval COMMAND="$setmode" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			do_unlock
			echo "Failed to set operating mode"
			proto_notify_error "$interface" SETMODE_FAILED
			return 1
		}
		json_select ..
	}

	echo "Disconnecting modem"
	json_get_vars disconnect
	eval COMMAND="$disconnect" gcom -d "$device" -s /etc/gcom/runcommand.gcom

	echo "Y" > /sys/class/net/$ifname/qmi/raw_ip
	echo "Starting network $interface"
	json_get_vars connect
	echo "Connecting modem"
	eval COMMAND="$connect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
		do_unlock
		echo "Failed to connect"
		proto_notify_error "$interface" CONNECT_FAILED
		proto_ndis_show_status
		return 1
	}

	json_get_vars finalize
	json_get_vars getaddrs

	echo "Setting up $ifname"
	proto_init_update "$ifname" 1
	proto_add_data
	json_add_string "manufacturer" "$manufacturer"
	proto_close_data

	if [ -n "$getaddrs" ] ; then
		eval PROFILE="$profile" $(gcom -d "$device" -s $getaddrs) || {
			do_unlock
			echo "Failed to get IP address information"
			proto_notify_error "$interface" GETADDRS_FAILED
			return 1
		}

		proto_add_ipv4_address "$ip" "$subnet"
                [ "$defaultroute" = 0 ] || proto_add_ipv4_route "0.0.0.0" 0 "$gateway" "" "$metric"
                [ "$peerdns" = 0 ] || {
                        proto_add_dns_server "$dns1"
#                        proto_add_dns_server "$dns2"
                }
                proto_send_update "$interface"
	else
		proto_send_update "$interface"
		[ "$pdptype" = "IP" -o "$pdptype" = "IPV4V6" ] && {
			json_init
			json_add_string name "${interface}_4"
			json_add_string ifname "@$interface"
			json_add_string proto "dhcp"
			proto_add_dynamic_defaults
			ubus call network add_dynamic "$(json_dump)"
		}

		[ "$pdptype" = "IPV6" -o "$pdptype" = "IPV4V6" ] && {
			json_init
			json_add_string name "${interface}_6"
			json_add_string ifname "@$interface"
			json_add_string proto "dhcpv6"
			json_add_string extendprefix 1
			proto_add_dynamic_defaults
			ubus call network add_dynamic "$(json_dump)"
		}
	fi

	[ -n "$finalize" ] && {
		eval COMMAND="$finalize" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
			do_unlock
			echo "Failed to configure modem"
			proto_notify_error "$interface" FINALIZE_FAILED
			return 1
		}
	}
	proto_ndis_led_ok ${interface}
	do_unlock
	REBOOT_CODE_STR=""
	if [ "$reboot_code" != "" ] ; then
		REBOOT_CODE_STR="--reboot-code $reboot_code"
	fi
	3g_connmgr -i ${interface} $REBOOT_CODE_STR &
	proto_ndis_show_status
}

proto_ndis_teardown() {
	local interface="$1"

	local manufacturer disconnect

	local device profile
	json_get_vars device profile usb_idx

	[ -n "$ctl_device" ] && device=$ctl_device

	[ -n "$profile" ] || profile=1

	echo "Stopping network $interface"
	proto_ndis_get_device

	proto_ndis_led_off ${interface}
	[ -f /var/run/3g_connmgr_${interface}.pid ] && kill $(cat /var/run/3g_connmgr_${interface}.pid)

	[ -e "$device" ] || {
		echo "Control device not valid trying again in 5 seconds..."
		sleep 5
		proto_ndis_get_device
		[ -e "$device" ] || {
			echo "Control device not valid"
			return 1
		}
	}

	json_load "$(ubus call network.interface.$interface status)"
	json_select data
	do_lock
	manufacturer=`gcom -d "$device" -s /etc/gcom/getcardinfo.gcom | awk 'NF && $0 !~ /AT\+CGMI/ { sub(/\+CGMI: /,""); print tolower($1); exit; }'`
	[ -n "$manufacturer" ] || {
		do_unlock
		echo "Failed to get modem information from $device"
		proto_notify_error "$interface" GETINFO_FAILED
		return 1
	}
	echo "Manufacturer is \"$manufacturer\""
	json_load "$(cat /etc/gcom/ndis.json)"
	json_select "$manufacturer" || {
		do_unlock
		echo "Unsupported modem"
		proto_notify_error "$interface" UNSUPPORTED_MODEM
		return 1
	}

	json_get_vars disconnect
	eval COMMAND="$disconnect" gcom -d "$device" -s /etc/gcom/runcommand.gcom || {
		do_unlock
		echo "Failed to disconnect"
		proto_notify_error "$interface" DISCONNECT_FAILED
		return 1
	}
	do_unlock

	proto_init_update "*" 0
	proto_send_update "$interface"
}

proto_ndis_check_led() {
    config_get name $1 name
    config_get sysfs $1 sysfs

    if [ "$name" = "$2" ] ; then
        led=$sysfs
        ledidx=$1
    fi
}

proto_ndis_find_led() {
        led=""
        config_load "system"
        config_foreach proto_ndis_check_led led $1
        eval [ ! -z $led ]
}

proto_ndis_set_led() {
        for i in $* ; do
                uci set system.$ledidx.$i
        done
        uci commit system.$ledidx
        /etc/init.d/led start
}

proto_ndis_led_ok() {
        proto_ndis_find_led $1 && proto_ndis_set_led trigger=netdev dev=$ifname default=1
}

proto_ndis_led_off() {
        proto_ndis_find_led $1 && proto_ndis_set_led trigger=none dev=$ifname default=0
}

proto_ndis_led_connecting() {
        proto_ndis_find_led $1 && proto_ndis_set_led trigger=timer dev=$ifname default=255
}

LCKFILE="/var/run/man3g.pid"

do_lock() {
    let cnt=600
    LAST_PID=0
    while [ $((cnt)) -gt 0 ] ; do
        if [ -e ${LCKFILE} ] ; then
                PID=$(cat ${LCKFILE})
                if [ "$PID" == "$$" ] ; then
                        return
                fi
                kill -0 ${PID} > /dev/null 2>&1
                if [ "$?" = "0" ] ; then
                        if [ $((cnt % 50)) -eq 0 ] ; then
                                echo "Waiting for another man3g process to finish... (pid ${PID})"
                        fi
                else
                        if [ "${LAST_PID}" = "${PID}" ] ; then
                                echo "Stale lock file from pid ${PID}! Process is not running. Removing lock..."
                                break
                        fi
                fi
                LAST_PID=${PID}
                usleep 100000
        else
                break
        fi
        let cnt=cnt-1
    done
#    trap do_exit SIGTERM
    echo "$$" > ${LCKFILE}
    echo "$$ Locking"
    # verify that we got it!
    do_lock
    LOCKED=1
}

do_unlock() {
    echo "$$ Unlocking"
    rm -f ${LCKFILE}
    LOCKED=0
}

do_exit() {
    echo "Exiting while lock held, unlocking..."
    if [ "$LOCKED" = "1" ] ; then
        do_unlock
    fi
    exit
}

[ -n "$INCLUDE_ONLY" ] || {
	add_protocol ndis
}
