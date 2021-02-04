#!/bin/bash

# 5 demo units
# Done in pairs. Don't ask about the options. I really wanted to use getoptlong but ran out of time to figure it out
./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803062 -b "B0:91:37:05:00:26" -y imagestream_0a8198ec -u 26968dc44c877363f7e5142813e4f02f -h ap -j 1 > demos/AP3500-24803062
./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803063 -b "B0:91:37:05:00:2A" -y imagestream_0a8198ec -u 26968dc44c877363f7e5142813e4f02f -h sta -j 1 > demos/AP3500-24803063
./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803064 -b "B0:91:37:05:00:2E" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h ap -j 1 > demos/AP3500-24803064
./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803065 -b "B0:91:37:05:00:32" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h sta -j 1 > demos/AP3500-24803065
./make_license -t 4 -m "AP3500" -r 2 -n "Twist-Port Radio" -s 24803066 -b "B0:91:37:05:00:36" -y imagestream_5d401ed6 -u 759b4bfda8b5d401ed67810850de1208 -h ap -j 1 > demos/AP3500-24803066
