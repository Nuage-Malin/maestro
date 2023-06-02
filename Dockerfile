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

WORKDIR /app

# CMake installations
ENV GRPC_FULL_INSTALL="true"

COPY third_parties/grpc /app/third_parties/grpc
COPY third_parties/mongo-cxx-driver /app/third_parties/mongo-cxx-driver
COPY third_parties/libcron /app/third_parties/libcron
COPY cmake /app/cmake
COPY CMakeLists.txt /app

RUN cmake -D install=true -S . -B build
RUN make -C build -j $((`nproc` - 1))

# Project build
COPY third_parties/protobuf-interfaces /app/third_parties/protobuf-interfaces
COPY include /app/include
COPY src /app/src

RUN cmake -D launch=true -S . -B build
RUN make -C build -j $((`nproc` - 1))

# Run
CMD ./build/maestro