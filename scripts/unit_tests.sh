#!/bin/bash

set -o allexport
source ./env/unit_tests.env
source ./env/local_unit_tests.env
set +o allexport
make -C build unit_tests
RET=$?
if [ $RET -ne 0 ]; then
  echo "[Unit tests] compilation failed"
  exit $RET
fi

echo "$1"
if [ "$1" == "--docker" ] ; then
  docker compose --env-file ./env/unit_tests.env up --build --detach
  RET=$?
#  DOCKER_PID=$!
  if [ $RET -ne 0 ]; then
    echo "[Unit tests] docker launch failed"
    exit $RET
  fi
fi

./build/unit_tests

RET=$?
if [ $RET -ne 0 ]; then
  echo "[Unit tests] tests failed"
  exit $RET
fi

if [ "$1" == "--docker" ] ; then
#    kill $DOCKER_PID ## todo launch docker in background from docker arguments and stop with docker command
  docker compose --env-file ./env/unit_tests.env down
fi