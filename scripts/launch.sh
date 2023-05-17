#!/bin/bash

set -o allexport
source ./env/maestro.env
set +o allexport

cmake -S . -B build

make -C build

