#!/bin/bash

ARG_CMAKE=false # --cmake
ARG_BUILD=false # --build (maestro)
ARG_LOCAL=false # --local
ARG_MONGO=false # --mongo
ARG_DOCKER=false # --docker

LOCAL_PID=0

usage()
{
    echo "Usage: $0 [--cmake] [--build] [--local] [--docker] [--mongo]" 1>&2
    exit 1
}

exit_gracefully()
{
    echo "Exiting gracefully..."

    if [ $LOCAL_PID -ne 0 ] && ps -p $LOCAL_PID > /dev/null; then
        kill $LOCAL_PID
    fi

    if $ARG_DOCKER; then
        docker compose --env-file ./env/unit_tests.env --profile launch down
    fi
    if $ARG_MONGO; then
        docker compose --env-file ./env/unit_tests.env --profile mongo down
    fi

    exit $1
}

check_exit_failure()
{
    EXIT_STATUS=$?
    if [ $EXIT_STATUS -ne 0 ]; then
        echo "$1" 1>&2
        exit_gracefully $EXIT_STATUS
    fi
}

for arg in "$@"; do
    case $arg in
        --help)
            usage
        ;;
        --cmake)
            ARG_CMAKE=true
        ;;
        --build)
            ARG_BUILD=true
        ;;
        --local)
            ARG_LOCAL=true
        ;;
        --mongo)
            ARG_MONGO=true
        ;;
        --docker)
            ARG_DOCKER=true
        ;;
        *)
            echo "Invalid option: $arg" >&2
            exit 1
        ;;
    esac
done

check_maestro_environment()
{
    if $ARG_LOCAL && $ARG_DOCKER; then
        echo "Unit tests will be run in both local and docker environments. Do you want to continue? [y/n]"
        read -r answer

        if [[ $answer =~ ^[Yy]$ ]]; then
            echo "Run with local and docker environments..."
        elif [[ $answer =~ ^[Nn]$ ]]; then
            exit_gracefully 0
        else
            check_maestro_environment
        fi
    fi
}
check_mongo_environment()
{
    if $ARG_MONGO && $ARG_DOCKER; then
        echo "Unit tests will be run in both mongo and docker environments. Do you want to continue? [y/n]"
        read -r answer

        if [[ $answer =~ ^[Yy]$ ]]; then
            echo "Run with mongo and docker environments..."
        elif [[ $answer =~ ^[Nn]$ ]]; then
            exit_gracefully 0
        else
            check_environments
        fi
    fi
}
check_environments()
{
    check_maestro_environment
    check_mongo_environment
}
check_environments

set -o allexport
source ./env/unit_tests.env
set +o allexport

if $ARG_CMAKE; then
    cmake -D unit_tests=true -S . -B build
    check_exit_failure "[Unit tests] cmake failed"
fi
if $ARG_BUILD; then
    make -C build maestro
    check_exit_failure "[Unit tests] compilation failed"
fi
make -C build unit_tests
check_exit_failure "[Unit tests] compilation failed"

if $ARG_DOCKER; then
    docker compose --env-file ./env/unit_tests.env --profile launch build
    check_exit_failure "[Unit tests] docker build failed"
fi
if $ARG_MONGO; then
    docker volume rm $MONGO_VOLUME
    docker compose --env-file ./env/unit_tests.env --profile mongo build
    check_exit_failure "[Unit tests] mongo build failed"
fi
if $ARG_DOCKER; then
    docker compose --env-file ./env/unit_tests.env --profile launch up -d
    check_exit_failure "[Unit tests] docker build failed"
fi
if $ARG_MONGO; then
    docker compose --env-file ./env/unit_tests.env --profile mongo up -d
    check_exit_failure "[Unit tests] mongo build failed"
    sleep 3
fi

if $ARG_LOCAL; then
    set -e
    echo "Starting local maestro. The program will automatically exit without printing errors if maestro fails."
    ./build/maestro &
    LOCAL_PID=$!
    set +e
fi

echo "Waiting for MongoDB to be ready..."

until $(curl --output /dev/null --silent --fail http://localhost:$MONGO_PORT); do
    if $ARG_DOCKER && [ "$( docker container inspect -f '{{.State.Status}}' maestro-mongo )" != "running" ]; then
        echo "MongoDB exited unexpectedly" >&2
        exit_gracefully 1
    fi
    printf '.'
    sleep 1
done
docker logs maestro-mongo-loader -f
echo -e "\nMongoDB is ready"

if $ARG_LOCAL ; then
  echo "Waiting for maestro to be ready..."
  if ! ps -p $LOCAL_PID > /dev/null; then
      echo "Maestro exited unexpectedly"
      exit_gracefully 1
  fi
  while ! nc -z localhost 8080; do
      if ! ps -p $LOCAL_PID > /dev/null; then
          echo "Maestro exited unexpectedly"
          exit_gracefully 1
      fi
      printf '.'
      sleep 1
  done
fi

echo -e "\nMaestro is ready"

./build/unit_tests
check_exit_failure "[Unit tests] tests failed"
exit_gracefully 0