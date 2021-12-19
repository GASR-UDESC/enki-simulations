FROM ubuntu:latest

RUN apt -y update && apt install -y

RUN apt -y install gcc
RUN apt -y install make
RUN apt -y install cmake

COPY . /usr/src/enki-s

WORKDIR /usr/src/enki-s
