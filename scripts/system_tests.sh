#!/bin/bash

ARG_BUILD_SERVICE=false # --build-service
ARG_BUILD_TESTS=false   # --build-tests
ARG_RUN_SERVICE=false   # --run-service
ARG_RUN_TESTS=false     # --run-tests
ARG_STOP=false          # --stop

CURRENT_FILE_DIR="$(dirname $0)/"

usage()
{
    echo "Usage: $0 [--help] [--build-service] [--build-tests] [--run-service] [--run-tests] [--stop]"
    echo "\t--help: Prints this message"
    echo "\t--build-service: Build maestro"
    echo "\t--build-tests: Build system tests"
    echo "\t--run-service: Run maestro"
    echo "\t--run-tests: Run system tests"
    echo "\t--stop: Stop service"
    exit 0
}

check_exit_failure()
{
    EXIT_STATUS=$?
    if [ $EXIT_STATUS -ne 0 ]; then
        echo -e "\033[31m$1\033[0m" 1>&2
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
        --stop)
            ARG_STOP=true
        ;;
        *)
            echo "Invalid option: $arg" >&2
            exit 1
        ;;
    esac
done

if $ARG_BUILD_SERVICE; then
    $CURRENT_FILE_DIR/launch.sh --build --docker --dry-run
    check_exit_failure "Failed to build maestro"
fi

if $ARG_BUILD_TESTS; then
    docker build -t maestro-system-tests -f Dockerfile.system-tests $CURRENT_FILE_DIR/..
    check_exit_failure "Failed to build system tests"
fi

if $ARG_RUN_SERVICE; then
    exec $CURRENT_FILE_DIR/launch.sh --docker
    check_exit_failure "Failed to run maestro"
fi

if $ARG_RUN_TESTS; then
    docker run --rm --network users-back:maestro --env-file $CURRENT_FILE_DIR/../env/system_tests.env --name maestro-system-tests maestro-system-tests
    check_exit_failure "System tests failed"
fi

if $ARG_STOP; then
    docker stop maestro-system-tests
    docker compose --env-file ./env/maestro.env --profile launch down
    check_exit_failure "Failed to stop maestro"
fi

exit 0