#!/bin/bash

ARG_BUILD_SERVICE=false # --build-service
ARG_BUILD_TESTS=false   # --build-tests
ARG_RUN_SERVICE=false   # --run-service
ARG_RUN_TESTS=false     # --run-tests

CURRENT_FILE_DIR="$(dirname $0)/"

usage()
{
    echo "Usage: $0 [--help] [--build-service] [--build-tests] [--run-service] [--run-tests]"
    echo "\t--help: Prints this message"
    echo "\t--build-service: Build maestro"
    echo "\t--build-tests: Build system tests"
    echo "\t--run-service: Run maestro"
    echo "\t--run-tests: Run system tests"
    exit 0
}

check_exit_failure()
{
    EXIT_STATUS=$?
    if [ $EXIT_STATUS -ne 0 ]; then
        echo "$1" 1>&2
        exit $EXIT_STATUS
    fi
}

for arg in "$@"; do
    case $arg in
        --help)
            usage
        ;;
        --build-service)
            ARG_BUILD_SERVICE=true
        ;;
        --build-tests)
            ARG_BUILD_TESTS=true
        ;;
        --run-service)
            ARG_RUN_SERVICE=true
        ;;
        --run-tests)
            ARG_RUN_TESTS=true
        ;;
        *)
            echo "Invalid option: $arg" >&2
            exit 1
        ;;
    esac
done

if $ARG_BUILD_SERVICE; then
    $CURRENT_FILE_DIR/launch.sh --build --dry-run
    check_exit_failure "Failed to build maestro"
fi

if $ARG_BUILD_TESTS; then
    cmake -D system_tests=true -S . -B build
    check_exit_failure "Failed to cmake system tests"

    make -C build system_tests
    check_exit_failure "Failed to make system tests"
fi

if $ARG_RUN_SERVICE; then
    $CURRENT_FILE_DIR/launch.sh --docker
    check_exit_failure "Failed to run maestro"
fi

if $ARG_RUN_TESTS; then
    set -o allexport
    source $CURRENT_FILE_DIR/../env/system_tests.env
    set +o allexport

    ./build/system_tests
    check_exit_failure "System tests failed"
fi

exit 0