#!/bin/sh
# 'qmi4g' network protocol handling
#
# Copyright (C) 2012 Lanedo GmbH <aleksander@lanedo.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#
# The 'qmi4g' protocol helps to manage QMI-enabled devices. The expected
# configuration in /etc/config/network is something like this:
#
# config 'interface' 'broadband'
#         option 'ifname' 'wwan0'
#         option 'proto' 'dhcp'
#         option 'auto' '0'
#
# config 'interface' 'wdm0'
#         option 'proto' 'qmi4g'
#         option 'wdmif' '/dev/cdc-wdm0'
#         option 'wwanif' 'broadband'
#
# So, first we have the configuration for a WWAN interface, setup with DHCP
# and NOT automatically brought up during boot.
#
# Then, we have the 'qmi4g' interface, with the specific cdc-wdm port to be used
# set in the 'device' option. This is the interface to be broght up/down, which
# will internall control when the WWAN interface is to be enabled/disabled.
#

stop_interface_qmi4g() {
    local config="$1"

    local device
    config_get wdmif "$config" wdmif
    local wwanif
    config_get wwanif "$config" wwanif

    uci_set_state network "$config" up 0

    logger -t qmi4g "Stopping QMI network on device '$wdmif'..."
    qmi-network "$wdmif" stop
    logger -t qmi4g "Stopping QMI WWAN interface '$wwanif'..."
    /sbin/ifdown "$wwanif"
}

setup_interface_qmi4g() {
    local iface="$1"
    local config="$2"

    /sbin/insmod cdc_wdm 2>&- >&-
    /sbin/insmod qmi_wwan 2>&- >&-

    local device
    config_get wdmif "$config" wdmif
    local wwanif
    config_get wwanif "$config" wwanif

    logger -t qmi4g "Starting QMI network on device '$wdmif'..."
    qmi-network "$wdmif" start
    [ $? -eq 0 ] && {
        uci_set_state network "$config" up 1

        logger -t qmi4g "Starting QMI WWAN interface '$wwanif'..."
        /sbin/ifup "$wwanif"
    }
}
