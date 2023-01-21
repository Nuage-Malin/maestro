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

# Copy sources
COPY third_parties /app/third_parties
COPY cmake /app/cmake
COPY CMakeLists.txt /app
COPY include /app/include
COPY src /app/src

# Build
RUN cmake -S . -B build
RUN make -C build

# Run
CMD ./build/maestro

# todo stage "unit_tests"