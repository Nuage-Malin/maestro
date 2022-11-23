FROM ubuntu:22.04

# Requirements
RUN apt-get update && apt-get install -y \
    cmake   \
    g++     \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy sources
COPY cmake /app/cmake
COPY CMakeLists.txt /app
COPY include /app/include
COPY src /app/src

# Build
RUN cmake -B build
RUN make -C build

# Run
CMD ./maestro