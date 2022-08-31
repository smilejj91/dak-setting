#!/bin/bash

while true;
do
dak import-repository --keyring=/usr/share/keyrings/debian-archive-keyring.gpg --architectures=all,i386,amd64,arm64 --components=main,contrib,non-free --target-suite=tmax-unstable --max-packages=1000 http://httpredir.debian.org/debian bullseye
done
