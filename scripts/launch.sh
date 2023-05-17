#!/bin/bash

cmake -S . -B build

make -C build

if [ "$1" == "--docker" ] ; then
    docker compose --env-file ./env/unit_tests.env --profile mongo up --build &
    DOCKER_PID=$!
fi

set -o allexport
source ./env/local.env
set +o allexport

./build/maestro

if [ "$1" == "--docker" ] ; then
    kill $DOCKER_PID ## todo launch docker in background from docker arguments and stop with docker command
fi