FROM ubuntu:22.04

# Requirements
RUN apt-get update && apt-get install -y \
    curl                \
    cmake               \
    g++                 \
    git                 \
    build-essential     \
    autoconf            \
    libtool             \
    pkg-config          \
    libssl-dev          \
    libsasl2-dev        \
    libmongoc-dev       \
    libmongoc-doc       \
    libstdc++-12-dev    \
    libmongoc-1.0-0     \
    libbson-1.0-0       \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app/third_parties

# CMake installations
COPY .git/modules/third_parties/grpc/HEAD /app/.git/modules/grpc
RUN mkdir grpc && cd grpc \
    && git init \
    && git remote add origin https://github.com/grpc/grpc.git \
    && git fetch origin $(head -n 1 /app/.git/modules/grpc | tr -d '\n') \
    && git checkout FETCH_HEAD \
    && git submodule update --init --recursive
COPY .git/modules/third_parties/mongo-cxx-driver/HEAD /app/.git/modules/mongo-cxx-driver
RUN mkdir mongo-cxx-driver && cd mongo-cxx-driver \
    && git init \
    && git remote add origin https://github.com/mongodb/mongo-cxx-driver.git \
    && git fetch origin $(head -n 1 /app/.git/modules/mongo-cxx-driver | tr -d '\n') \
    && git checkout FETCH_HEAD \
    && git submodule update --init --recursive
COPY .git/modules/third_parties/libcron/HEAD /app/.git/modules/libcron
RUN mkdir libcron && cd libcron \
    && git init \
    && git remote add origin https://github.com/PerMalmberg/libcron.git \
    && git fetch origin $(head -n 1 /app/.git/modules/libcron | tr -d '\n') \
    && git checkout FETCH_HEAD \
    && git submodule update --init --recursive

WORKDIR /app

COPY cmake /app/cmake
COPY CMakeLists.txt /app

RUN cmake -D install=true -S . -B build
RUN make -C build -j $((`nproc` - 1))

# Project build
COPY third_parties/protobuf-interfaces /app/third_parties/protobuf-interfaces
COPY include /app/include
COPY src /app/src

RUN cmake -D build=true -S . -B build
RUN make -C build -j $((`nproc` - 1))

# Run
CMD ./build/maestro