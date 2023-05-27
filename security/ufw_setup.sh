#!/bin/bash

if [ "$EUID" != 0 ]
  then echo "Please run as root"
  exit
fi

name="ufw"
dpkg -s $name &> /dev/null  

if [ $? -ne 0 ]
    then
        echo "not installed"  
        sudo apt-get update
        sudo apt-get install $name
    else
        echo "$name already installed"
fi

ufw disable

#clear/remove all existing rules (clean slate)
ufw --force reset

#ufw rules
ufw default deny incoming
ufw default deny outgoing
ufw allow 22
ufw allow svn
ufw allow git
ufw allow out http
ufw allow in http
ufw allow out https
ufw allow in https
ufw allow out 53
ufw allow 3000
ufw allow 1883
ufw allow 8088
ufw allow 8086
ufw logging on

#print added rules
ufw show added

#enable ufw firewall
ufw enable
