#!/bin/bash
#
# Copyright (C) 2013-2014 ImageStream Internet Solutions, Inc.
#
# One-shot production info text file for Compex Production line
# File will disappear after reboot

set +e
. /lib/functions.sh
. /lib/functions/system.sh

power2() {
    local x=${1#-} n=1
    while ((n<x)); do ((n*=2)); done
    x=$((3*n>4*x?n/2:n))
    echo $(($1<0?-x:x))
}

ethports_st() {
	board=$(board_name)
	case "$board" in
		wpj563 |\
		wpj344)
			PORTNAMES="LAN0 LAN1"
			PORTNUMS="3 2"
			;;
		wpj558)
			PORTNAMES="LAN0 LAN1"
			PORTNUMS="5 1"
			;;
		wpj531)
			PORTNAMES="LAN0 LAN1"
			PORTNUMS="5 3"
			;;
		wpj342)
			PORTNAMES="LAN0 LAN1"
			PORTNUMS="2 1"
			;;
	esac
	ETHPORTS=
	for port in $PORTNAMES; do
		set -- $PORTNUMS
		portnum=$1
		shift
		PORTNUMS="$@"
		PORTST=$(swconfig dev switch0 port ${portnum} show | grep link)
		PORTST=${PORTST/*link/link}
		PORTST=${PORTST/duplex*/duplex}
		ETHPORTS="$ETHPORTS"$(printf "\n    $port: $PORTST")
	done
}

do_start() {
	let retries=30
	while [ $((retries)) -gt 0 ]; do
		ip link show dev wlan1 > /dev/null 2>&1 && break
		sleep 1
		let retries=retries-1
	done
	. /etc/opuntia_release
	e0_mac=$(ip link show dev eth0 | grep link | awk '{ print $2 }')
	w0_mac=$(ip link show dev wlan0 | grep link | awk '{ print $2 }')
	w1_mac=$(ip link show dev wlan1 | grep link | awk '{ print $2 }')
	PCIERADIOS="None"
	for pciedev in /sys/bus/pci/devices/*;do
		if [ -d $pciedev -a -e $pciedev/class ];then
			pcieclass=$(cat $pciedev/class)
			if [ "$(($pcieclass))" = "$((0x28000))" ];then
				DEVICEID="$(cat $pciedev/vendor),$(cat $pciedev/device)"
				case $DEVICEID in
					0x168c,0x0029) PCIERADIO="11n AR9220";;
					0x168c,0x002a) PCIERADIO="11n AR9280";;
					0x168c,0x0030) PCIERADIO="11n AR9380";;
					0x168c,0x0033) PCIERADIO="11n AR9580";;
					0x168c,0x003c) PCIERADIO="11ac QCA9880";;
					0x168c,0x0040) PCIERADIO="11ac QCA9980";;
					0x168c,0x0046) PCIERADIO="11ac QCA9984";;
					0x168c,0x0056) PCIERADIO="11ac QCA9888";;
				*) PCIERADIO="Unknown $DEVICEID";;
				esac
				if [ "$PCIERADIOS" = "None" ]; then
					PCIERADIOS=$PCIERADIO
				else
					PCIERADIOS="$PCIERADIOS, $PCIERADIO"
				fi
			fi
		fi
	done
	# Ethernet ports status
	ethports_st
	for i in 1 2 3 4 5 6 7 8; do
		st=$(echo $ETHPORTS | grep -o down)
		if [ -n "$st" ];then
			sleep 2
			ethports_st
		else
			break
		fi
	done

	echo "Firmware Version: $DISTRIB_RELEASE $DISTRIB_REVISION"
	echo "Loader Version: "$(strings /dev/mtd0 | grep U-Boot)
	echo "First Radio MAC address: ${w0_mac^^}"
	echo "Second Radio MAC address: ${w1_mac^^}"
	echo "Ethernet MAC address: ${e0_mac^^}"
	echo "Company Name: $DISTRIB_MANUFACTURER"
	echo "Model Name: $DISTRIB_PRODUCT"
	echo "Flash Size: "$(power2 $(($(dmesg | grep spi0 | grep 'Kbytes)' | cut -d '(' -f 2 | awk '{ print $1 }')/1000)) )" MB"
	echo "RAM Size: "$(power2 $(($(grep MemTotal: /proc/meminfo | awk '{ print $2 }')/1000)) )" MB"
	echo "PCIe Radio Detected: $PCIERADIOS"
	echo "USB STORAGE:"
	echo "USB LTE:"
	echo "LTE SIM:"
	echo "ETH PORTS: $ETHPORTS"
}

do_start > /tmp/info.txt 2>&1
mkdir /www/prod/
ln -s /tmp/info.txt /www/prod/info.txt
