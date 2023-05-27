#!/bin/bash

ARG_BUILD=false     # --build (maestro)
ARG_DOCKER=false    # --docker
ARG_DRY_RUN=false   # --dry-run

usage()
{
    echo "Usage: $0 [--help] [--build] [--docker] [--dry-run]"
    echo -e "\t--help: Prints this message"
    echo -e "\t--build: Build maestro (cmake + make)"
    echo -e "\t--docker: Launches the database in a docker container but the still runs maestro locally"
    echo -e "\t--dry-run: Builds maestro if --build is specified and start the database if --docker is specified but does not run maestro. It's useful to build without running maestro."
    exit 0
}

exit_gracefully()
{
    if [ $1 -ne 0 ]; then
        echo -e "\033[31mExiting gracefully...\033[0m" 1>&2
    else
        echo "Exiting gracefully..."
    fi

    if $ARG_DOCKER; then
        docker compose --env-file ./env/local.env --profile mongo down
    fi

    exit $1
}

check_exit_failure()
{
    EXIT_STATUS=$?
    if [ $EXIT_STATUS -ne 0 ]; then
        echo -e "\033[31m$1\033[0m" 1>&2
        exit_gracefully $EXIT_STATUS
    fi
}

for arg in "$@"; do
    case $arg in
        --help)
            usage
        ;;
        --build)
            ARG_BUILD=true
        ;;
        --docker)
            ARG_DOCKER=true
        ;;
        --dry-run)
            ARG_DRY_RUN=true
        ;;
        *)
            echo "Invalid option: $arg" >&2
            exit 1
        ;;
    esac
done

if $ARG_BUILD; then
    cmake -S . -B build
    check_exit_failure "Failed to cmake"

    make -C build
    check_exit_failure "Failed to make"
fi

if $ARG_DOCKER; then
    docker compose --env-file ./env/local.env --profile mongo up --build -d
fi

trap "exit_gracefully 1" SIGINT

set -o allexport
source ./env/local.env
set +o allexport

if ! $ARG_DRY_RUN; then
    exec ./build/maestro
fi