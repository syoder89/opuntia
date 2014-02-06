#!/bin/sh
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, version 2.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Copyright (C) 2012 Aleksander Morgado <aleksander@gnu.org>
# Copyright (C) 2012 André Valentin / MarcanT GmbH <avalentin@marcant.net>

. /lib/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_qmi_init_config() {
	proto_config_add_string "ipaddr"
	proto_config_add_string "netmask"
	proto_config_add_string "hostname"
	proto_config_add_string "clientid"
	proto_config_add_string "vendorid"
	proto_config_add_boolean "broadcast"
	proto_config_add_string "reqopts"
	proto_config_add_string "apn"
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_boolean "lte_apn_use"
	proto_config_add_string "lte_apn"
	proto_config_add_string "lte_username"
	proto_config_add_string "lte_password"
	proto_config_add_string "pincode"
	proto_config_add_string "technology"
	proto_config_add_string "auto"
	proto_config_add_string "provider"
	proto_config_add_string "service"
	proto_config_add_string "mcc"
	proto_config_add_string "mnc"
	proto_config_add_string "reboot_code"
}

proto_qmi_log() {
	local level="$1"
	local message="$2"
	[ -z "$message" ] && {
		logger -p "$1" -t "qmi[$$]"
		return;
	}
	shift
	logger -p "$level" -t "qmi[$$]" -- "$@"
}

proto_qmi_clear_state() {
	local $config=$1
	uci_revert_state network $config CID
	uci_revert_state network $config PDH
	uci_revert_state network $config QMIDEVICE
}

proto_qmi_start_network() {
	local config=$1
	local DEVICE=$2
	local APN="$3"
	local CID=$(uci_get_state network $config CID)
	local PDH=$(uci_get_state network $config PDH)

	if [ "x$CID" != "x" ]; then
		USE_PREVIOUS_CID="--client-cid=$CID"
	fi

	if [ "x$PDH" != "x" ]; then
		logger -p daemon.info  "error: cannot re-start network, PDH already exists"
		return 3
	fi

	# Disable enhanced autoconnect or our Option GT689W will fail with a NO EFFECT error! Scott 08-05-2013
	uqmi -d $DEVICE --disable-enhanced-autoconnect > /dev/null 2>&1

	START_NETWORK_CMD="qmicli -d $DEVICE --wds-start-network=$APN $USE_PREVIOUS_CID --client-no-release-cid"
	logger -p daemon.info  "Starting network with '$START_NETWORK_CMD'..."
	START_NETWORK_OUT=`$START_NETWORK_CMD`

	# Save the new CID if we didn't use any before
	if [ "x$CID" = "x" ]; then
		CID=`echo "$START_NETWORK_OUT" | grep CID | sed "s/'//g" | awk 'BEGIN { FS = ": " } ; { print $2 }'`
		if [ "x$CID" = "x" ]; then
			logger -p daemon.info  "error: network start failed, client not allocated"
			return 1
		else
			uci_set_state network $config CID $CID
		fi
	fi

	PDH=`echo "$START_NETWORK_OUT" | grep handle | sed "s/'//g" | awk 'BEGIN { FS = ": " } ; { print $2 }'`
	if [ "x$PDH" = "x" ]; then
		proto_qmi_log daemon.info "error: network start failed, no packet data handle"
		# Cleanup the client
		qmicli -d "$DEVICE" --wds-noop --client-cid="$CID"
		uci_revert_state network $config CID
		uci_revert_state network $config PDH
		uci_revert_state network $config QMIDEVICE
		CID=""
		PDH=""
		return 2
	else
		uci_set_state network $config PDH "$PDH"
	fi
	uci_set_state network $config QMIDEVICE "$DEVICE"
	proto_qmi_log daemon.info "Network started successfully: CID: $CID, PDH=$PDH, DEVICE=$DEVICE"
}

proto_qmi_stop_network () {
	local config=$1
	local CID=$(uci_get_state network $config CID)
	local PDH=$(uci_get_state network $config PDH)
	local DEVICE=$(uci_get_state network $config QMIDEVICE)

	if [ "x$CID" = "x" ]; then
		proto_qmi_log daemon.info "Network already stopped"
	elif [ "x$PDH" = "x" ]; then
		proto_qmi_log daemon.info "Network already stopped; need to cleanup CID $CID"
		# Cleanup the client
		qmicli -d "$DEVICE" --wds-noop --client-cid="$CID"
	else
		STOP_NETWORK_CMD="qmicli -d $DEVICE --wds-stop-network=$PDH --client-cid=$CID"
		proto_qmi_log daemon.info "Stopping network with '$STOP_NETWORK_CMD'..."
		STOP_NETWORK_OUT=`$STOP_NETWORK_CMD`
		proto_qmi_log daemon.info "Network stopped successfully"
	fi
	uci_revert_state network $config CID
	uci_revert_state network $config PDH
	uci_revert_state network $config QMIDEVICE
}

proto_qmi_packet_service_status () {
	local config=$1
	local CID=$(uci_get_state network $config CID)
	local PDH=$(uci_get_state network $config PDH)
	local DEVICE=$(uci_get_state network $config QMIDEVICE)

	if [ "x$CID" != "x" ]; then
		USE_PREVIOUS_CID="--client-cid=$CID --client-no-release-cid"
	fi

	STATUS_CMD="qmicli -d $DEVICE --wds-get-packet-service-status $USE_PREVIOUS_CID"

	STATUS_OUT=`$STATUS_CMD`

	CONN=`echo "$STATUS_OUT" | grep "Connection status" | sed "s/'//g" | awk 'BEGIN { FS = ": " } ; { print $2 }'`
	if [ "x$CONN" = "x" ]; then	
		proto_qmi_log daemon.info "error: couldn't get packet service status"
		return 2
	else
		if [ "x$CONN" != "xconnected" ]; then
			proto_qmi_log daemon.debug "Status: $CONN"
			return 64
		fi
	fi
}

proto_qmi_check_led() {
    config_get dev $1 dev
    config_get sysfs $1 sysfs

    if [ "$dev" = "$2" ] ; then
    	led=$sysfs
    fi
}

proto_qmi_find_led() {
	led=""
	config_load "system"
	config_foreach proto_qmi_check_led led $1
	eval [ ! -z $led ]
}

proto_qmi_led_ok() {
	proto_qmi_find_led $1 && echo netdev > /sys/class/leds/$led/trigger && echo $1 > /sys/class/leds/$led/device_name \
			&& echo "link tx rx" > /sys/class/leds/$led/mode && echo 1 > /sys/class/leds/$led/brightness
}

proto_qmi_led_off() {
	proto_qmi_find_led $1 && echo 0 > /sys/class/leds/$led/brightness
}

proto_qmi_led_connecting() {
	proto_qmi_find_led $1 && echo 255 > /sys/class/leds/$led/brightness && echo timer > /sys/class/leds/$led/trigger
}

LCKFILE="/var/run/man3g.pid"

do_lock() {
    let cnt=30
    LAST_PID=0
    while [ $((cnt)) -gt 0 ] ; do
        if [ -e ${LCKFILE} ] ; then
                PID=$(cat ${LCKFILE})
                kill -0 ${PID} > /dev/null 2>&1
                if [ "$?" = "0" ] ; then
                        if [ $((cnt % 5)) -eq 0 ] ; then
                        	proto_qmi_log daemon.info "Waiting for another man3g process to finish... (pid ${PID})"
			fi
                else
                        if [ "${LAST_PID}" = "${PID}" ] ; then
                                proto_qmi_log daemon.err "Stale lock file from pid ${PID}! Process is not running. Removing lock..."
                                break
                        fi
                fi
                LAST_PID=${PID}
                sleep 1
        else
                break
        fi
        let cnt=cnt-1
    done
    trap do_exit SIGTERM
    echo "$$" > ${LCKFILE}
}

do_unlock() {
    rm -f ${LCKFILE}
}

do_exit() {
    proto_qmi_log daemon.err "Exiting while lock held, unlocking..."
    do_unlock
    exit
}

trigger_watchdog() {
        proto_qmi_log "Triggering hardware watchdog forcing a hardware reset in 60 seconds..."
        killall -9 watchdog
}

check_modem() {
	MODEMDEV=$1
	failcnt=3
	while [ $((failcnt)) -gt 0 ] ; do
        	do_lock
        	csq=`gcom -d ${MODEMDEV} -s /etc/gcom/man3g_csq.gcom | head -n 1 | cut -d "," -f 1`
        	do_unlock
        	if [ "$csq" = "" ] ; then
                	proto_qmi_log "Check modem: failed to retrieved signal quality!"
        	else
			return
        	fi
                let failcnt=failcnt-1
	done
	trigger_watchdog
	sleep 120
}

proto_qmi_setup() {
	local config="$1"
	local iface="$2"
	local ipaddr hostname clientid vendorid broadcast reqopts apn username password pincode auto lte_apn_use lte_apn lte_username lte_password provider service mcc mnc reboot_code
	json_get_vars ipaddr hostname clientid vendorid broadcast reqopts apn username password pincode auto data lte_apn_use lte_apn lte_username lte_password provider service mcc mnc reboot_code

	config_load network
	config_get technology $config technology

	proto_qmi_led_connecting ${iface}

	case "$provider" in
		verizon)
			type="cdma"
			pri="8"
			modem="2"
			mcc="310"
			mnc="12"
		;;
		sprint)
			type="cdma"
			pri="7"
			modem="3"
			mcc="310"
			mnc="120"
		;;
		att)
			type="gsm"
			pri="1"
			modem="1"
			mcc="310"
			mnc="410"
			apn="wap.cingular"
		;;
		tmobile)
			type="gsm"
			pri="4"
			modem="1"
			mcc="310"
			mnc="260"
			apn="epc.tmobile.com"
		;;
		telcel)
			type="gsm"
			pri="0"
			modem="1"
			mcc="334"
			mnc="20"
			apn="internet.itelcel.com"
			username="webgprs"
			password="webgprs2002"
		;;
		movistar)
			type="gsm"
			pri="0"
			modem="1"
			mcc="334"
			mnc="3"
			apn="internet.movistar.mx"
			username="movistar"
			password="movistar"
		;;
		iusacell)
			type="gsm"
			pri="0"
			modem="1"
			mcc="334"
			mnc="50"
			apn="web.iusacellgsm.mx"
			username="iusacellgsm"
			password="iusacellgsm"
		;;
		tigo_colombia)
			type="gsm"
			pri="0"
			modem="1"
			mcc="732"
			mnc="103"
			apn="web.colombiamovil.com.co"
		;;
		movistar_colombia)
			type="gsm"
			pri="0"
			modem="1"
			mcc="732"
			mnc="123"
			apn="internet.movistar.com.co"
			username="movistar"
			password="movistar"
		;;
		comcel)
			type="gsm"
			pri="0"
			modem="1"
			mcc="732"
			mnc="101"
			apn="internet.comcel.com.co"
			username="comcel"
			password="comcel"
		;;
		vodafone)
			type="gsm"
			pri="6"
			modem="1"
			mcc="222"
			mnc="10"
			apn="ppbundle.internet"
			username="web"
			password="web"
		;;
		telefonica)
			type="gsm"
			pri="2"
			modem="1"
			mcc="234"
			mnc="15"
			apn="wap.vodafone.co.uk"
			username="wap"
			password="wap"
		;;
		telecom_italia)
			type="gsm"
			pri="3"
			modem="1"
			mcc="222"
			mnc="1"
			apn="wap.tim.it"
			username="WAPTIM"
			password="WAPTIM"
		;;
		orange)
			type="gsm"
			pri="9"
			modem="1"
			mcc="208"
			mnc="2"
			apn="orange.fr"
		;;
		gsm)
			pri="0"
			modem="1"
		;;
		cdma)
			pri="8"
			modem="2"
		;;
		*)
			proto_qmi_log daemon.err "Unknown provider $provider"
			exit 1
		;;
	esac

	# Load technology list
	# Setup APN config
	apn_standard="${apn}"
	[ -n "${username}" ] && {
		apn_standard="${apn_standard},both,${username}"
	}
	[ -n "${password}" ] && {
		apn_standard="${apn_standard},${password}"
	}
	apn_lte="${lte_apn}"
	[ -n "${lte_username}" ] && {
		apn_lte="${apn_lte},both,${lte_username}"
	}
	[ -n "${password}" ] && {
		apn_lte="${apn_lte},${lte_password}"
	}
	
	# Reset auto state if forced by technology selection
	uci_revert_state network $config auto
	uci set network.$config.apn=$apn
	uci commit network.$config.apn
	
	local CDCDEV
	CDCDEV=/dev/$(basename $(ls /sys/class/net/${iface}/device/usbmisc/cdc-wdm* -d)) || {
		CDCDEV="$device"
		proto_qmi_log daemon.err "Control device not found, using network.${config}.device: $CDCDEV"
		return 1
	}
	proto_qmi_log daemon.info "Wan Device: ${iface}, Control CDC: ${CDCDEV}, APN: $apn_standard, LTE APN: $apn_lte, Pincode: $pincode"
	if [ -z "$CDCDEV" ]; then
		proto_qmi_log daemon.err "Device $CDCDEV is empty"
	fi
	while ! [ -c "$CDCDEV" ]; do
		proto_qmi_log daemon.debug "Waiting for device creation: ${CDCDEV}"
		sleep 2
	done
	# Check that the modem is not hung
	check_modem $CDCDEV
	# Just in case there is still context data
	proto_qmi_stop_network ${config}
	
	do_lock
# Very bad things happen when we do this! We get QMI hangs all over the place!
#	# Reset the modem
#	proto_qmi_log daemon.info "Resetting modem"
#	qmicli -d $CDCDEV --dms-set-operating-mode=offline
#	qmicli -d $CDCDEV --dms-set-operating-mode=reset
#	sleep 10
#        let retries=10
#        while [ $((retries)) -gt 0 ] ; do
#		qmicli -d $CDCDEV --dms-set-operating-mode=online > /dev/null 2>&1
#		if [ "$?" = "0" ] ; then
#			break
#		fi
#		sleep 1
#		let retries=retries-1
#	done
#	proto_qmi_log daemon.info "Modem online"

        if [ "$modem" != "2" ] && [ "$modem" != "3" ] ; then
		rat=""
		if [ "$service" = "2g" ] ; then
			rat="gsm"
		elif [ "$service" = "3g" ] ; then
			rat="umts"
		fi
		# Not sure how to implement this! The uqmi command fails with missing argument so our modem
		# seems to require some mandatory TLV that the NAS spec doesn't list. See the QMI NAS spec
		# for TLV 0x33 (3.26 QMI_NAS_SET_SYSTEM_SELECTION_PREFERENCE)
	fi

	# Set the correct firmware image

        let has_firmware=0
        let retries=5
        while [ $((retries)) -gt 0 ] ; do
                current_modem=`qmicli -d $CDCDEV --dms-list-stored-images | grep CURRENT -A 1 | grep modem | cut -b 9- | sed -e 's/]*//g'`
                if [ "$?" = "0" ] ; then
                        current_pri=`qmicli -d $CDCDEV --dms-list-stored-images | grep CURRENT -A 1 | grep pri | cut -b 7- | sed -e 's/]*//g'`
                        if [ "$?" = "0" ] && [ "$current_modem" != "" ] && [ "$current_pri" != "" ] ; then
                                has_firmware=1
                                break
                        fi
                fi
                let retries=retries-1
                sleep 1
        done

        if [ "$has_firmware" = "1" ] ; then
		if [ $((current_modem)) != $((modem)) ] || [ $((current_pri)) != $((pri)) ] ; then
			proto_qmi_log daemon.info "Changing modem firmware to $modem / $pri. Current firmware is $current_modem / $current_pri."
			watchdog_pid=`cat /var/run/qmi-watchdog-${config}.pid`
			qmicli -d $CDCDEV --dms-select-stored-image=modem${modem},pri${pri}
			qmicli -d $CDCDEV --dms-set-operating-mode=offline
			qmicli -d $CDCDEV --dms-set-operating-mode=reset
			sleep 10
			current_modem=`qmicli -d $CDCDEV --dms-list-stored-images | grep CURRENT -A 1 | grep modem | cut -b 9- | sed -e 's/]*//g'`
			current_pri=`qmicli -d $CDCDEV --dms-list-stored-images | grep CURRENT -A 1 | grep pri | cut -b 7- | sed -e 's/]*//g'`
			if [ $((current_modem)) != $((modem)) ] || [ $((current_pri)) != $((pri)) ] ; then
				proto_qmi_log daemon.info "Failed to change modem firmware! Current firmware is $current_modem / $current_pri."
			fi
		fi
	fi

	# Check PIN
	[ -n "$pincode" ] && {
		set -o pipefail
		if ! qmicli -d $CDCDEV "--dms-uim-verify-pin=PIN,${pincode}" 2>&1 | proto_qmi_log daemon.info; then
			qmicli -d $CDCDEV --dms-uim-get-pin-status | proto_qmi_log daemon.info
			proto_qmi_log daemon.info "PIN Verification failed, shutting down and block restart."
			proto_notify_error "$config" PIN_FAILED
			proto_block_restart "$interface"
			do_unlock
			return 1
		fi
	}
	# Print info about system selection for debugging purpose
	qmicli -d $CDCDEV --nas-get-system-selection-preference 2>&1 | proto_qmi_log daemon.debug

	do_unlock
	# Wait for registration
	while ! qmicli -d $CDCDEV --nas-get-serving-system|grep 'Registration state'|grep "'registered'" > /dev/null; do
		sleep 1;
		proto_qmi_log daemon.info "Waiting for registration"
	done

	# Print current network info
	qmicli -d $CDCDEV --nas-get-serving-system 2>&1 | proto_qmi_log daemon.debug

	# Select APN	
	if qmicli -d $CDCDEV --nas-get-serving-system | grep -q "'lte'" > /dev/null && [ "${lte_apn_use}" = "1" ]; then
		current_apn="$apn_lte"
	else
		current_apn="$apn_standard"
	fi

	# Try to start network	
	set -o pipefail
	while ! proto_qmi_start_network ${config} $CDCDEV "${current_apn}" 2>&1 | proto_qmi_log daemon.info; do
		sleep 5
	done

	proto_qmi_led_ok ${iface}

	REBOOT_CODE_STR=""
	if [ "$reboot_code" != "" ] ; then
		REBOOT_CODE_STR="--reboot-code $reboot_code"
	fi
	3g_connmgr -i ${iface} $REBOOT_CODE_STR &

	# Show status and start watchdog
	qmicli -d $CDCDEV --nas-get-serving-system 2>&1 | proto_qmi_log daemon.debug
	(
		set -o pipefail
		let counter=0
		while sleep 20; do
			proto_qmi_packet_service_status ${config}
			STATUS=$?
			[ "$STATUS" -gt 0 ] && {
				proto_qmi_log daemon.err "QMI Status shows error ${STATUS}, shutting down ${config}, control device $CDCDEV"
				(ifdown $config; sleep 30; ifup $config) </dev/null > /dev/null 2>&1 &
				exit 1
			}
			if ! router=$(ip neigh show dev ${iface} | cut -d ' ' -f1 ); then
				proto_qmi_log daemon.info "Neighbor not found"
				continue
			fi
			if ! arping -q -c1 -w 5 -I ${iface} $(ip neigh show dev ${iface}|cut -d' ' -f1); then
				let counter=${counter}+1
				proto_qmi_log daemon.err "Arping gateway timed out, Error counter: $counter"
			else
				# Reset counter on success!
				let counter=0
			fi
			[ "$counter" -gt 5 ] && {
				proto_qmi_led_off ${iface}
				proto_qmi_log daemon.err "Error counter to high: $counter, shutting down ${config}, control device ${CDCDEV}"
				(ifdown $config; sleep 5; sleep 30; ifup $config) </dev/null > /dev/null 2>&1 &
				exit 1
			}
		done
	) </dev/null > /dev/null 2>&1 &
	local watchdog_pid=$!
	echo $watchdog_pid > /var/run/qmi-watchdog-${config}.pid
	proto_qmi_log daemon.info "Started watchdog pid $watchdog_pid"

	local opt dhcpopts
	for opt in $reqopts; do
		append dhcpopts "-O $opt"
	done

	[ "$broadcast" = 1 ] && broadcast="-B" || broadcast=
	[ -n "$clientid" ] && clientid="-x 0x3d:${clientid//:/}" || clientid="-C"

	proto_export "INTERFACE=$config"
	proto_run_command "$config" udhcpc \
		-p /var/run/udhcpc-$iface.pid \
		-s /lib/netifd/dhcp-qmi.script \
		-f -t 0 -i "$iface" \
		-x lease:60 \
		${ipaddr:+-r $ipaddr} \
		${hostname:+-H $hostname} \
		${vendorid:+-V $vendorid} \
		$clientid $broadcast $dhcpopts
}

proto_qmi_teardown() {
	local interface="$1"
	local iface="$2"
	local CID=$(uci_get_state network $interface CID)
	local DEVICE=$(uci_get_state network $interface QMIDEVICE)
	[ -e /var/run/qmi-watchdog-${interface}.pid ] && {
		kill $(cat /var/run/qmi-watchdog-${interface}.pid)
		rm /var/run/qmi-watchdog-${interface}.pid
	}
	[ -e /var/run/3g_connmgr_${iface}.pid ] && {
		kill $(cat /var/run/3g_connmgr_${iface}.pid)
		rm /var/run/3g_connmgr_${iface}.pid
	}
	do_lock
	proto_qmi_stop_network ${interface}
	do_unlock
	proto_qmi_led_off ${iface}
	proto_kill_command "$interface"
	echo "$interface done"
}

add_protocol qmi
