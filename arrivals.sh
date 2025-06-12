#!/bin/bash

gcc encode.c -o encode

if [ -z "$1" ]; then
    echo "Usage: $0 <stop_id>"
    exit 1
fi

STOP_ID="$1"

# Correct variable assignment: no spaces around '=' and use $(...) for command substitution
RESPONSE=$(curl -s -A "Mozilla/5.0" "http://telematics.oasa.gr/api/?act=getStopArrivals&p1=$STOP_ID")

if [ "$RESPONSE" != "null" ]; then
    echo "$RESPONSE" | jq -r '[.[] | select(.btime2 != null)] | .[] | "\(.route_code) \(.veh_code) \(.btime2)"' > arrivals.txt
else
    > arrivals.txt  # create or empty arrivals.txt if response is null
fi

if [ -s arrivals.txt ]; then
    ./encode < arrivals.txt > arrivals.bin
else
    > arrivals.bin  # create or empty arrivals.bin if no data
fi