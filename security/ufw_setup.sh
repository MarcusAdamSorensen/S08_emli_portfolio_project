#!/bin/bash

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
ufw logging on

#print added rules
ufw show added

#enable ufw firewall
ufw enable
