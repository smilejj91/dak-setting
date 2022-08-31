#!/bin/bash

set -x

ln -sf /usr/share/zoneinfo/Asia/Seoul /etc/localtime

if [ ! -e /var/lib/postgresql/data/base ]; then
  chown -R postgres:postgres /var/lib/postgresql/data
  sudo -E -u postgres /usr/lib/postgresql/13/bin/initdb -D /var/lib/postgresql/data
fi

sudo -E -u postgres /usr/lib/postgresql/13/bin/pg_ctl -D /var/lib/postgresql/data start "-o -c config_file=/etc/postgresql/13/main/postgresql.conf"


if [ ! -e /srv/dak/bin/dak ]; then
  su - 
  ln -s /srv/dak/etc/dak.conf /etc/dak/dak.conf
  ./dak/setup/dak-setup.sh

  su -
  cat /home/dak/dak/config/tos/crontab > /var/spool/cron/crontabs/dak
  chown dak:crontab /var/spool/cron/crontabs/dak

  su - dak -c "mkdir -p /srv/dak/dak/"
  cp -r /home/dak/dak/config /srv/dak/dak/config
  chown -R dak:ftpmaster /srv/dak/dak
  
  su - dak -c "mkdir -p /srv/dak/keyrings/s3kr1t/dot-gnupg"
  su - dak -c "mkdir -p /srv/dak/queue/byhand/COMMENTS"
  su - dak -c "mkdir -p /srv/dak/tiffani"
  su - dak -c "mkdir -p /srv/dak/ftp/indices/files/components"
  su - dak -c "mkdir -p /srv/dak/mirror/tos-ftp-master"
  su - dak -c "mkdir -p /srv/dak/backup"

  su - dak -c "ssh-keygen -t rsa -q -f '/home/dak/.ssh/id_rsa' -N ''"
  su - dak -c "cat /home/dak/.ssh/id_rsa.pub > /home/dak/.ssh/authorized_keys"
fi 

if [ ! -e /etc/dak/dak.conf ]; then
  su - 
  ln -s /srv/dak/etc/dak.conf /etc/dak/dak.conf
fi

if [ ! -e /var/spool/cron/crontabs/dak ]; then
  su -
  cat /home/dak/dak/config/tos/crontab > /var/spool/cron/crontabs/dak
  chown dak:crontab /var/spool/cron/crontabs/dak
fi

ln -s /srv/dak/mirror/tos-ftp-master /srv/mirror/tmax

/etc/init.d/cron start
/etc/init.d/exim4 start
service nginx start
service ssh start

if [ ! -e /srv/dak/log/current ]; then
  # refer to monthly.functions
  DATE=`date +%Y-%m`
  su - dak -c "ln -sf /srv/dak/log/${DATE} /srv/dak/log/current"
  su - dak -c "mkdir -p /srv/dak/log/cron"
fi

tail -f /srv/dak/log/current
