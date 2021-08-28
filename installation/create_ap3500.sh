#!/bin/bash

# 10 units
# Done in pairs. Don't ask about the options. I really wanted to use getoptlong but ran out of time to figure it out
NUM_PAIR=1
SN=24803100
MAC_VAL=194137739821194
while [ $((NUM_PAIR)) -gt 0 ] ; do
	MAC=$(printf "%02X\n" $MAC_VAL | sed -e 's/[0-9A-F]\{2\}/&:/g' -e 's/:$//')
	RND=$(openssl rand -hex 4)
	SSID="imagestream_$RND"
	PW=$(openssl rand -hex 16)
	echo "./make_license -t 4 -m \"AP3500\" -r 2 -n \"Twist-Port Radio\" -s $SN -b \"$MAC\" -y $SSID -u $PW -h ap -j 1 > prod/AP3500-$SN"
	[ -f prod/AP3500-$SN ] && echo "File already exists!" && exit 1
	./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s $SN -b "$MAC" -y $SSID -u $PW -h ap -j 1 > prod/AP3500-$SN
	SN=$((SN+1))
	MAC_VAL=$((MAC_VAL+4))
	MAC=$(printf "%02X\n" $MAC_VAL | sed -e 's/[0-9A-F]\{2\}/&:/g' -e 's/:$//')
	echo "./make_license -t 4 -m \"AP3500\" -r 2 -n \"Twist-Port Radio\" -s $SN -b \"$MAC\" -y $SSID -u $PW -h sta -j 1 > prod/AP3500-$SN"
	[ -f prod/AP3500-$SN ] && echo "File already exists!" && exit 1 
	./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s $SN -b "$MAC" -y $SSID -u $PW -h sta -j 1 > prod/AP3500-$SN
	SN=$((SN+1))
	MAC_VAL=$((MAC_VAL+4))
	NUM_PAIR=$((NUM_PAIR-1))
done
#./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803062 -b "B0:91:37:05:00:26" -y imagestream_0a8198ec -u 26968dc44c877363f7e5142813e4f02f -h ap -j 1 > prod/AP3500-24803062
#./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803063 -b "B0:91:37:05:00:2A" -y imagestream_0a8198ec -u 26968dc44c877363f7e5142813e4f02f -h sta -j 1 > prod/AP3500-24803063
#./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803064 -b "B0:91:37:05:00:2E" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h ap -j 1 > prod/AP3500-24803064
#./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803065 -b "B0:91:37:05:00:32" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h sta -j 1 > prod/AP3500-24803065
#./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803066 -b "B0:91:37:05:00:36" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h ap -j 1 > prod/AP3500-24803066
