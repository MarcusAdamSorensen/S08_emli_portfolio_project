#!/bin/bash

sudo sysctl -w net.ipv4.ip_forward=1
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE

python3 /home/pi/bin/mqtt.py
