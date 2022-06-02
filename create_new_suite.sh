#!/bin/bash

echo -e "You must execute command with dak user privileges!"
echo -e "

Step 0. Setup SSH connection for rsync in unchecked task

$ ssh-keygen
$ cat /home/dak/.ssh/id_rsa.pub
$ scp id_rsa.pub dak@tmax-uploadqueue-cont:/home/dak/.ssh/authorized_keys

Step 1. Create release key and import public key

$ gpg --homedir /srv/dak/keyrings/s3kr1t/dot-gnupg --batch --passphrase '' --quick-gen-key "NAME \<EMAIL\>" default default
$ gpg --no-default-keyring --keyring /srv/dak/keyrings/upload-keyring.gpg --import {Public Key}
$ dak import-keyring -U '%s' /srv/dak/keyrings/upload-keyring.gpg

Step 2. Add architecture

$ dak admin architecture add i386 "Intel x86 port" 
$ dak admin architecture add amd64 "AMD64 port" 

Step 3. Add a suite and components to the suite

$ dak admin suite add-all-arches {SUITE_NAME} {DOMAIN_NAME} origin={ORIGIN_NAME} label={LABEL_NAME} codename={CODE_NAME} signingkey={RELASE_KEY_ID}
$ dak admin s-c add {SUITE_NAME} main contrib non-free

Step 4. Re-run dak init-dirs

$ dak init-dirs

read dak/setup/README.rst
"



