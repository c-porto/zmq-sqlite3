FROM ubuntu:22.04 as builder

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update \
    && apt-get install -y build-essential \
    gcc \
    g++ \
    make \
    cmake \
    git \
    libsqlite3-dev 

WORKDIR /server

COPY . .

RUN cmake -S . -Bbuild 
RUN cmake --build build
ENTRYPOINT [ "build/zmqserver" ]
EXPOSE 42068

