#!/bin/bash

if [ "$EUID" != 0 ]
  then echo "Please run as root"
  exit
fi

# Update at 01:00 every week on Wednesday:
cron_schedule="00 01 * * 3"

#Update & upgrade command
cron_update_command="/usr/bin/apt-get update && /usr/bin/apt-get -y upgrade >> /var/log/apt/myupdates.log"

# Update the cron configuration with the new cron job
(echo "$cron_schedule $cron_update_command") | sudo crontab -

echo "Cron has been configured with the following schedule $cron_schedule"