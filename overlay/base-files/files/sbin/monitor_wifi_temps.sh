#!/bin/sh

#    50 0
#    53 10
#    56 20
#    59 30
#    62 40
#    65 50
#    68 60
#    71 70
#    74 80
#    77 90
#    80 100

# Check interval in seconds
SLEEP_INTERVAL=30
# Temp threshold in milli degrees C
TEMP_THRESH=50000
# Divide difference by this to get percentage
TEMP_DIV=300
# Max throttle percentage - don't go to 100% let it melt!
MAX_PCT=90

get_throttle_pct()
{
        temp=$1
        td=$((temp-TEMP_THRESH))
        pct=$((td/TEMP_DIV))
        if [ $pct -ge 0 ] ; then
                if [ $pct -lt $MAX_PCT ] ; then
                        echo $pct
                else
                        echo $MAC_PCT
                fi
        else
                echo 0
        fi
}


while : ; do
        for p in /sys/class/ieee80211/phy*; do
                [ -d $p ] || break
                t=$(cat $p/device/hwmon/hwmon*/temp1_input)
                phy=$(basename $p)
                tp=$(get_throttle_pct $t)
                cur=$(cat /sys/class/ieee80211/$phy/device/cooling_device/cur_state)
                if [ $cur -ne $tp ]; then
                        logger "Throttling $phy to $tp% due to temp $((t/1000)) C"
                        echo $tp > /sys/class/ieee80211/$phy/device/cooling_device/cur_state
                fi
        done
        sleep $SLEEP_INTERVAL
done
