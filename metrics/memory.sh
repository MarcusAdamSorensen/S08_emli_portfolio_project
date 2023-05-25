#!/bin/bash

total=$(free -m | awk '/Mem:/ { print $2 }')
used=$(free -m | awk '/Mem:/ { print $3 }')

percentage=$(echo "scale=2; ($used / $total) * 100" | bc)

echo "$percentage%"
