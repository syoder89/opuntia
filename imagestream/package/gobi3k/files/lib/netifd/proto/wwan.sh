#!/bin/sh
INCLUDE_ONLY=1

. ../netifd-proto.sh
. ./dhcp.sh
init_proto "$@"

LOGGER="/usr/bin/logger"

proto_wwan_init_config() {
	no_ifname=1
	available=1
	proto_dhcp_init_config
	proto_config_add_string "ifname"
	proto_config_add_string "provider"
	proto_config_add_string "apn"
	proto_config_add_string "service"
	proto_config_add_string "username"
	proto_config_add_string "password"
	proto_config_add_string "mcc"
	proto_config_add_string "mnc"
}

proto_wwan_setup() {
	local interface="$1"

	json_get_var ifname ifname
	json_get_var provider provider
	json_get_var apn apn
	json_get_var service service
	json_get_var username username
	json_get_var password password
	json_get_var mcc mcc
	json_get_var mnc mnc

	case "$provider" in
		verizon)
			type="cdma"
			fw="1"
			mcc="310"
			mnc="12"
		;;
		sprint)
			type="cdma"
			fw="3"
			mcc="310"
			mnc="120"
		;;
		att)
			type="gsm"
			fw="2"
			mcc="310"
			mnc="410"
			apn="wap.cingular"
		;;
		tmobile)
			type="gsm"
			fw="4"
			mcc="310"
			mnc="260"
			apn="epc.tmobile.com"
		;;
		telcel)
			type="gsm"
			fw="6"
			mcc="334"
			mnc="20"
			apn="internet.itelcel.com"
			username="webgprs"
			password="webgprs2002"
		;;
		movistar)
			type="gsm"
			fw="6"
			mcc="334"
			mnc="3"
			apn="internet.movistar.mx"
			username="movistar"
			password="movistar"
		;;
		iusacell)
			type="gsm"
			fw="6"
			mcc="334"
			mnc="50"
			apn="web.iusacellgsm.mx"
			username="iusacellgsm"
			password="iusacellgsm"
		;;
		tigo_colombia)
			type="gsm"
			fw="6"
			mcc="732"
			mnc="103"
			apn="web.colombiamovil.com.co"
		;;
		movistar_colombia)
			type="gsm"
			fw="6"
			mcc="732"
			mnc="123"
			apn="internet.movistar.com.co"
			username="movistar"
			password="movistar"
		;;
		comcel)
			type="gsm"
			fw="6"
			mcc="732"
			mnc="101"
			apn="internet.comcel.com.co"
			username="comcel"
			password="comcel"
		;;
		vodafone)
			type="gsm"
			fw="0"
			mcc="222"
			mnc="10"
			apn="ppbundle.internet"
			username="web"
			password="web"
		;;
		telefonica)
			type="gsm"
			fw="7"
			mcc="234"
			mnc="15"
			apn="wap.vodafone.co.uk"
			username="wap"
			password="wap"
		;;
		telecom_italia)
			type="gsm"
			fw="8"
			mcc="222"
			mnc="1"
			apn="wap.tim.it"
			username="WAPTIM"
			password="WAPTIM"
		;;
		orange)
			type="gsm"
			fw="9"
			mcc="208"
			mnc="2"
			apn="orange.fr"
		;;
		gsm)
			fw="6"
		;;
		cdma)
			fw="1"
		;;
		*)
			$LOGGER "Unknown provider $provider"
			exit 1
		;;
	esac
	# Check firmware
	man3g check-firmware-loaded --firmware $fw
	if [ "$?" != 0 ] ; then
		ifconfig $ifname down
		$LOGGER "Loading firmware $fw"
        	let retries=5
        	while [ $((retries)) -ge 0 ] ; do
                	man3g load-firmware --firmware $fw
                	if [ "$?" = "0" ] ; then
                        	break
                	fi
                	$LOGGER "Retrying..."
                	sleep 1
                	let retries=retries-1
        	done
	fi
	ifconfig $ifname up
        if [ "$fw" != "1" ] && [ "$fw" != "3" ] ; then
		rat=""
		if [ "$service" = "2g" ] ; then
			rat="gsm"
		elif [ "$service" = "3g" ] ; then
			rat="umts"
		fi
		REGISTER_CMD="register"
		[ "$mcc" != "" ] || REGISTER_CMD="$REGISTER_CMD --mcc $mcc"
		[ "$mnc" != "" ] || REGISTER_CMD="$REGISTER_CMD --mnc $mnc"
		[ "$apn" != "" ] || REGISTER_CMD="$REGISTER_CMD --apn $apn"
		[ "$rat" != "" ] || REGISTER_CMD="$REGISTER_CMD --rat $rat"
		[ "$username" != "" ] || REGISTER_CMD="$REGISTER_CMD --username $username"
		[ "$password" != "" ] || REGISTER_CMD="$REGISTER_CMD --password $password"

        	let retries=1
        	while [ $((retries)) -ge 0 ] ; do
                	man3g $REGISTER_CMD
                	ret="$?"
                	if [ "$ret" = "0" ] ; then
                        	break;
                	fi
                	$LOGGER "man3g $REGISTER_CMD  returned $ret"
                	$LOGGER "Retrying..."
                	sleep 1
                	let retries=retries-1
        	done
	fi

	man3g start-session --auto-connect
	proto_dhcp_setup $interface $ifname
	return 0
}

proto_wwan_teardown() {
	proto_kill_command "$interface"
	man3g stop-session
}

add_protocol wwan
