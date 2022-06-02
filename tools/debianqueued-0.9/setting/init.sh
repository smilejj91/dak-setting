#!/bin/bash

# incoming
mkdir -p /srv/upload.tmaxos.net/pub/UploadQueue
chown -R nobody:nogroup /srv/upload.tmaxos.net
chmod -R o+w /srv/upload.tmaxos.net/pub/UploadQueue

mkdir -p /srv/upload.tmaxos.net/for-ftpmaster
mkdir -p /srv/upload.tmaxos.net/keyrings
chown -R dak:ftpmaster /srv/upload.tmaxos.net/for-ftpmaster
chown -R dak:ftpmaster /srv/upload.tmaxos.net/keyrings

su - dak -c "mkdir -p /home/dak/run"
su - dak -c "gpg --no-default-keyring --keyring /srv/upload.tmaxos.net/keyrings/upload-keyring.gpg --fingerprint"

su -

service vsftpd start
service ssh start

su - dak -c "/usr/bin/perl -w debianqueued"

tail -f /home/dak/run/log
