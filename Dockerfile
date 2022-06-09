FROM debian:bullseye

#install packages

## install crucial packages
RUN apt -y update
RUN apt -y install procps sudo vim git postgresql postgresql-client postgresql-13-debversion locales ftpsync iproute2 iputils-ping jdupes rsync moreutils nginx ssh

#setting locale
RUN localedef -f UTF-8 -i en_US en_US.UTF-8
ENV LC_ALL en_US.UTF-8

RUN apt -y install python3-tabulate python3-voluptuous python3-psycopg2 python3-sqlalchemy python3-apt gnupg dpkg-dev lintian binutils-multiarch python3-yaml less python3-ldap python3-pyrss2gen python3-rrdtool symlinks python3-debian python3-debianbts

ADD tools/ftpmaster/default /etc/nginx/sites-available/default
ADD tools/ftpmaster/nginx.conf /etc/nginx/nginx.conf

RUN addgroup ftpmaster 
RUN adduser dak --disabled-password --ingroup ftpmaster --shell /bin/bash
RUN { echo "export LC_ALL=en_US.UTF-8"; echo "export PATH=\"/srv/dak/bin:\${PATH}\""; echo "export PGDATABASE=\"projectb\""; } >> /home/dak/.bashrc

ADD init_commands.sh /home/dak/init_commands.sh
ADD create_new_suite.sh /home/dak/create_new_suite.sh

RUN mkdir /etc/dak && mkdir /srv/dak && mkdir /srv/mirror

RUN sed -i 's=host    all             all             127.0.0.1/32            md5=host    all             all             127.0.0.1/32            trust=' /etc/postgresql/13/main/pg_hba.conf
RUN sed -i 's/data_directory =/#data_directory =/' /etc/postgresql/13/main/postgresql.conf
RUN sed -i 's/local/internet/' /etc/exim4/update-exim4.conf.conf

WORKDIR /home/dak

ENTRYPOINT ["/bin/bash", "init_commands.sh"]

# VERSION is 1.0.1
# $ docker build . --no-cache -t harbor.tmaxos.net/infra/tmax-dak:{VERSION}
# $ docker push harbor.tmaxos.net/infra/tmax-dak:{VERSION}
# http://redmine.tmaxos.net/projects/wiki/wiki/How_to_use_Harbor
