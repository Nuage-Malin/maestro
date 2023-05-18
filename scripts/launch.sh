#!/bin/bash

cmake -S . -B build

make -C build

if [ "$1" == "--docker" ] ; then
    docker compose --env-file ./env/unit_tests.env --profile mongo up --build &
    DOCKER_PID=$!
fi

function stop_docker() {
  if [ "$1" == "--docker" ] ; then
    kill $DOCKER_PID ## todo launch docker in background from docker arguments and stop with docker command
  fi
}

trap "echo \"Stopping docker container...\"; stop_docker $1; sleep 3; exit" SIGINT

set -o allexport
source ./env/local.env
set +o allexport

./build/maestro