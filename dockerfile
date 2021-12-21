FROM ubuntu:latest

RUN apt -y update && apt install -y

RUN apt -y install gcc
RUN apt -y install g++
RUN apt -y install make
RUN apt -y install cmake
RUN apt -y install git
RUN apt -y install build-essential
RUN apt -y install qt5-default
RUN apt -y install libpthread-stubs0-dev

ADD . /usr/src/enki-s

WORKDIR /usr/src/enki-s

CMD /bin/bash
