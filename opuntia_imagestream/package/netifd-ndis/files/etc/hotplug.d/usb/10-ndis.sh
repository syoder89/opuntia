. /lib/functions.sh
[ "$ACTION" = add -a "$DEVTYPE" = usb_device ] || exit 0

find_ndis_iface() {
	local cfg="$1"
	local proto
	local modemdev
	config_get proto "$cfg" proto
	# This doesn't work for some reason! Returns notfound!
#	config_get device "$cfg" device "notfound"
	# Shell out uci to get the device path!
	device=$(uci get network.$cfg.device)
	[ "$proto" = ndis ] || return 0
	[ -n "$device" ] || {
		usb_idx=$(uci get network.$cfg.usb_idx)
		let retries=5
		while [ $retries -gt 0 ]; do
			devpath=$(echo /sys/devices/pci*/*/usb${usb_idx}/*-*/*/*/usbmisc/cdc-wdm*)
			device="$(readlink -f "$devpath/../../..")"
			[ -e "$device" ] && break
			let retries=retries-1
			sleep 1
		done
	}
	[ "$device" = "/sys$DEVPATH" ] || return 0
	logger "Found reconnected device $cfg and setting up"
	proto_set_available "$cfg" 1
	ifup $cfg
	exit 0
}

config_load network
config_foreach find_ndis_iface interface
