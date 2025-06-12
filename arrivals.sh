#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <stop_id>"
    exit 1
fi

STOP_ID="$1"

curl -s -A "Mozilla/5.0" "http://telematics.oasa.gr/api/?act=getStopArrivals&p1=$STOP_ID" |
jq -r '[.[] | select(.btime2 != null)] | .[] | "\(.route_code) \(.veh_code) \(.btime2)"' > arrivals.txt

if [ -s arrivals.txt ]; then
    ./encode < arrivals.txt > arrivals.bin
else
    > arrivals.bin
fi