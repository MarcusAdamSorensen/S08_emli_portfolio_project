#!/bin/bash

transmit_before=$(awk '/'eth0'/ { print $10 }' /proc/net/dev)

sleep 1

transmit_after=$(awk '/'eth0'/ { print $10 }' /proc/net/dev)
transmit_rate=$(echo "scale=2; ($transmit_before - $transmit_after) / 1024" | bc)

echo "$transmit_rate"
