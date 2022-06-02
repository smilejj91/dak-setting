FROM debian:bullseye

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y gpg libemail-sender-perl exim4 vsftpd ssh rsync

ADD setting/vsftpd.conf /etc/vsftpd.conf
ADD setting/update-exim4.conf.conf /etc/exim4/update-exim4.conf.conf
RUN addgroup ftpmaster
RUN adduser dak --disabled-password --ingroup ftpmaster --shell /bin/bash

WORKDIR /home/dak

ADD config /home/dak/config
ADD debianqueued /home/dak/debianqueued
ADD setting/init.sh /home/dak/init.sh

EXPOSE 21 22

CMD ["/bin/bash", "init.sh"]

# How to make docker image
# $ docker build . --no-cache -t harbor.tmaxos.net/infra/tmax-uploadqueue:{tag number}
# $ docker push harbor.tmaxos.net/infra/tmax-uploadqueue:{tag number}
# http://redmine.tmaxos.net/projects/wiki/wiki/How_to_use_Harbor
