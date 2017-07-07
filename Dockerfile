FROM ubuntu:14.04 
MAINTAINER yxs

RUN cd /tmp/
RUN sudo apt-get update
RUN sudo apt-get install -y wget
RUN wget http://kernel.ubuntu.com/~kernel-ppa/mainline/v4.2-unstable/linux-headers-4.2.0-040200_4.2.0-040200.201508301530_all.deb
RUN wget http://kernel.ubuntu.com/~kernel-ppa/mainline/v4.2-unstable/linux-headers-4.2.0-040200-generic_4.2.0-040200.201508301530_amd64.deb
RUN wget http://kernel.ubuntu.com/~kernel-ppa/mainline/v4.2-unstable/linux-image-4.2.0-040200-generic_4.2.0-040200.201508301530_amd64.deb
RUN sudo dpkg -i linux-headers-4.2.0-*.deb linux-image-4.2.0-*.deb

RUN apt-get -y install gcc
RUN apt-get -y install make

COPY ./httpd/ /http/

RUN cd /http && make
ENTRYPOINT cd /http && ./start.sh


