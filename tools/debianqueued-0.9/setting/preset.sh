#!/bin/bash

# mail server
apt install -y libemail-sender-perl exim4
cp update-exim4.conf.conf /etc/exim4/update-exim4.conf.conf

# ftp server
apt install -y vsftpd
cp vsftpd.conf /etc/vsftpd.conf

# make root folder
mkdir -p /srv/upload.tmaxos.net

# for incoming file
mkdir -p /srv/upload.tmaxos.net/pub/UploadQueue
chown -R nobody:nogroup /srv/upload.tmaxos.net
chmod -R o+w /srv/upload.tmaxos.net/pub/UploadQueue

# for processed file
mkdir -p /srv/upload.tmaxos.net/for-ftpmaster

# for logging
mkdir -p ../run

# for key
mkdir -p /srv/upload.tmaxos.net/keyrings
gpg --no-default-keyring --keyring /srv/upload.tmaxos.net/keyrings/upload-keyring.gpg --fingerprint
