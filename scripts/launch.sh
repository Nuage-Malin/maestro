#!/bin/bash

ARG_BUILD=false     # --build (maestro)
ARG_DOCKER=false    # --docker
ARG_DRY_RUN=false   # --dry-run

usage()
{
    echo "Usage: $0 [--help] [--build] [--docker] [--dry-run]"
    echo -e "\t--help: Prints this message"
    echo -e "\t--build: Build maestro (cmake + make)"
    echo -e "\t--docker: Launches maestro and the database with docker"
    echo -e "\t--dry-run: Build maestro if --build is specified, but don't launch it"
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
        docker compose --env-file ./env/maestro.env --profile launch down
    else
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
    if $ARG_DOCKER; then
        docker compose --env-file ./env/maestro.env --profile launch build
        check_exit_failure "Failed to build with docker"
    else
        cmake -D install=true -S . -B build
        check_exit_failure "Failed to install dependencies"

        cmake -D build=true -S . -B build
        check_exit_failure "Failed to cmake"

        make -C build -j $((`nproc` - 1))
        check_exit_failure "Failed to make"

        docker compose --env-file ./env/local.env --profile mongo build
        check_exit_failure "Failed to build database"
    fi
fi

trap "exit_gracefully 1" SIGINT

if ! $ARG_DRY_RUN; then
    if $ARG_DOCKER; then
        docker compose --env-file ./env/maestro.env --profile launch up
        check_exit_failure "Failed to start maestro with docker"
    else
        docker compose --env-file ./env/local.env --profile mongo up -d
        check_exit_failure "Failed to start maestro database"

        set -o allexport
        source ./env/local.env
        set +o allexport

        exec ./build/maestro
        check_exit_failure "Failed to start maestro locally"
    fi
fi