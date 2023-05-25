#!/bin/bash

receive_before=$(awk '/'eth0'/ { print $2 }' /proc/net/dev)

sleep 1

receive_after=$(awk '/'eth0'/ { print $2 }' /proc/net/dev)
receive_rate=$(echo "scale=2; ($receive_before - $receive_after) / 1024" | bc)

echo "$receive_rate"
