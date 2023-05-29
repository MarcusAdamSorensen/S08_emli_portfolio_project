#!/bin/bash

if [ "$EUID" != 0 ]
  then echo "Please run as root"
  exit
fi

name="fail2ban"
dpkg -s $name &> /dev/null  

if [ $? -ne 0 ]
    then
        echo "$name not installed"  
        sudo apt-get update
        sudo apt-get install $name
    else
        echo "$name already installed"
fi

# Configuration parameters
config="[sshd]
enabled = true
port = ssh
filter = sshd
logpath = /var/log/auth.log
maxretry = 3
bantime = 24h
findtime = 1m
ignoreip = whitelist-IP"

# Path to sshd.conf file
sshd_conf="/etc/fail2ban/jail.d/sshd.conf"

# Create the sshd.conf file and set config parameters (permissions)
echo "$config" | tee "$sshd_conf" > /dev/null

echo "sshd.conf file has been created and configured."

echo "enable fail2ban with new config"
systemctl restart fail2ban.service 
