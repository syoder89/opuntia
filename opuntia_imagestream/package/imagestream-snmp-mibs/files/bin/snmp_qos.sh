#!/bin/bash

CACHE_TIME=25
ISIS_OID=".1.3.6.1.4.1.15425"
BASE_OID=".1.3.6.1.4.1.15425.1.1.2.1"
QOS_TABLE_OID="1"
#1.$offset.$snmpid

declare -a mib_types
declare -a mib_values

NUM_VARS=32

function find_next_oid()
{
	next_oid=`cat $oidfile | oidsort -n "$1" 2>/dev/null`
	ret="$?"
	return $?
}

function set_mib()
{
	local type=$1
	local val=$2

	echo "$BASE_OID.$QOS_TABLE_OID.$offset.$snmpid" >> $oidfile

	if [ "$type" = "counter" ] || [ "$type" = "integer" ] ; then
		val=$((val))
	fi
	let idx=(snmpid*NUM_VARS)+offset

	mib_types[$idx]="${type}"
	mib_values[$idx]="${val}"

	let offset=offset+1
}

#1.1.1 = 1
#1.2.1 = eth0
#1.3.1 = 3
#1.4.1 = default
#1.5.1 = LLQ

function get_mib()
{
	local QOS_OID=`echo $OID | cut -d '.' -f 14-`
	local offset=`echo $QOS_OID | cut -d '.' -f 1`
	local snmpid=`echo $QOS_OID | cut -d '.' -f 2`

	let idx=(snmpid*NUM_VARS)+offset

	type=${mib_types[$idx]}
	val=${mib_values[$idx]}
	if [ "$type" != "" ] ; then
		return 0
	fi
	return 1
}

#Interface Serial2.592, Policy default, Service ack, Type CBQ, Priority 3
#  Rx limits: 25 Kbps min, 102 Kbps max
#  Rx 0 packets 0 bytes (dropped 0 overlimits 0)
#    Rx backlog 0 packets 0 bytes 0 ms
#  Tx limits: 25 Kbps min, 102 Kbps max
#  Tx 0 packets 0 bytes (dropped 0 overlimits 0)
#    Tx backlog 0 packets 0 bytes 0 ms

function create_mib()
{
	local timestamp=`date +%s`

	if [ $((last_mib_update+CACHE_TIME)) -gt $((timestamp)) ] ; then
		return
	fi

	last_mib_update=$timestamp
	tmpfile="/tmp/snmp_qos.$$"
	oidfile="/tmp/snmp_qos_oids.$$"
	qos_stats --machine > $tmpfile
	let snmpid=1
	rm -f $oidfile
	while read iface
	do
		ifnum=`ip link show dev $iface | head -n 1 | cut -d ':' -f 1`
		read policy
		read service
		read type
		read priority
		read rx_min
		read rx_max
		read rx_packets
		read rx_bytes
		read rx_dropped
		read rx_overlimits
		read rx_backlog_packets
		read rx_backlog_bytes
		read rx_backlog_ms
		read tx_min
		read tx_max
		read tx_packets
		read tx_bytes
		read tx_dropped
		read tx_overlimits
		read tx_backlog_packets
		read tx_backlog_bytes
		read tx_backlog_ms

		let offset=1
		set_mib "integer" $snmpid
		set_mib "string" $iface
		set_mib "integer" $ifnum
		set_mib "string" $policy
		set_mib "string" $service
		set_mib "string" $type
		set_mib "integer" $priority
		set_mib "integer" $rx_min
		set_mib "integer" $rx_max
		set_mib "counter" $rx_packets
		set_mib "counter" $rx_bytes
		set_mib "counter" $rx_dropped
		set_mib "counter" $rx_overlimits
		set_mib "integer" $rx_backlog_packets
		set_mib "integer" $rx_backlog_bytes
		set_mib "integer" $rx_backlog_ms
		set_mib "integer" $tx_min
		set_mib "integer" $tx_max
		set_mib "counter" $tx_packets
		set_mib "counter" $tx_bytes
		set_mib "counter" $tx_dropped
		set_mib "counter" $tx_overlimits
		set_mib "integer" $tx_backlog_packets
		set_mib "integer" $tx_backlog_bytes
		set_mib "integer" $tx_backlog_ms
		let snmpid=snmpid+1
	done < $tmpfile
	rm -f $tmpfile
}

function do_get()
{
	get_mib
	if [ "$?" = "0" ] ; then
		echo $OID
		echo $type
		echo $val
	else
		echo "NONE"
	fi
}

function check_oid() {
	local tmp=`echo $1 | cut -d '.' -f 1-12`
	if [ "$tmp" != "$BASE_OID" ] ; then
		echo "NONE"
		return 1
	fi
	return 0
}

while read cmd ; do
	case "$cmd" in
		PING)
			echo "PONG"
		;;
		get)
			read OID
			check_oid $OID
			if [ "$?" = "0" ] ; then
				create_mib
				do_get
			fi
		;;
		getnext)
			read OID
			check_oid $OID
			if [ "$?" = "0" ] ; then
				create_mib
				find_next_oid $OID
				if [ "$?" = "0" ] ; then
					OID="${next_oid}"
					do_get
				else
					echo "NONE"
				fi
			fi
		;;
		*)
		# Unknown command
		;;
	esac
done

rm -f $oidfile
