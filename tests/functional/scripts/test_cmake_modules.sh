#!/bin/bash

## This script tests every cmake target's build, including cmake modules target

SCRIPT_PATH=$0
CMAKE_LISTS_TXT=$1

DEFAULT_COLOR="\e[0;00m"
DFLT_CLR=$DEFAULT_COLOR
BOLD="\e[1m"
DIM="\e[2m"
UNDLN="\e[4m"
SHINE="\e[5;37m"
RODE="\e[9;37m"

U_BLACK="\e[7;30m"
U_RED="\e[7;31m" # Test failed
U_TEST_FAILURE_CLR=$U_RED # Test failed
U_GREEN="\e[7;32m" # Test succeed
U_TEST_SUCCESS_CLR=$U_GREEN # Test succeed
U_YELLOW="\e[7;33m" # Debug
U_BLUE="\e[7;34m" # Information
U_INFO_CLR=$U_BLUE # Information
U_MAGENTA="\e[7;35m" # Error
U_ERR_CLR=$U_MAGENTA # Error
U_CYAN="\e[7;36m" # Meta information
U_WHITE="\e[7;37m"
BLACK="\e[30m"
RED="\e[31m" # Test failed
TEST_FAILURE_CLR=$RED # Test failed
GREEN="\e[32m" # Test succeed
TEST_SUCCESS_CLR=$GREEN # Test succeed
YELLOW="\e[33m" # Debug
BLUE="\e[34m" # Information
INFO_CLR=$BLUE # Information
MAGENTA="\e[35m" # Error
ERR_CLR=$MAGENTA # Error
CYAN="\e[36m" # Meta information

function error()
{
  echo -e $ERR_CLR $2 $DFLT_CLR
  exit $1
}

function usage()
{
  error 1 "USAGE\n\t$1 ./path/to/CMakeLists.txt"
}

TEST_NUMBER=0
TEST_SUCCESS_NUMBER=0
TEST_FAILURE_NUMBER=0

function test_success()
{
  (( TEST_SUCCESS_NUMBER++ ))
  (( TEST_NUMBER++ ))
  echo -e $U_TEST_SUCCESS_CLR "TEST SUCCESS $DFLT_CLR\t" $TEST_SUCCESS_CLR $1 $DFLT_CLR
}

function test_failure()
{
  (( TEST_NUMBER++ ))
  (( TEST_FAILURE_NUMBER++ ))
  echo -e $U_TEST_FAILURE_CLR "TEST FAILURE $DFLT_CLR\t" $TEST_FAILURE_CLR $1 $DFLT_CLR
}

function test_results()
{
  echo -ne "\n"
  echo -e $U_INFO_CLR "TOTAL NUMBER OF TESTS $DFLT_CLR\t\t" $INFO_CLR $TEST_NUMBER $DFLT_CLR
  echo -e $U_TEST_SUCCESS_CLR "NUMBER OF TEST SUCCESS $DFLT_CLR\t" $TEST_SUCCESS_CLR "$TEST_SUCCESS_NUMBER" $DFLT_CLR
  if [ $TEST_NUMBER -ne $TEST_SUCCESS_NUMBER ]; then
    echo -e $U_TEST_FAILURE_CLR "NUMBER OF TEST FAILURE $DFLT_CLR\t" $TEST_FAILURE_CLR "$TEST_FAILURE_NUMBER" $DFLT_CLR
    exit 1
  fi
  exit 0
}

function reset_build_dir()
{
  rm -rf build
  mkdir "build"
}

if [ ! -f $CMAKE_LISTS_TXT ]; then
  usage $SCRIPT_PATH
fi
CMAKE_LISTS_TXT_DIR=$( ( dirname $CMAKE_LISTS_TXT ) )

if [ ! `command -v cmake` ]; then
  error 1 "cmake command does not exist"
fi

reset_build_dir

## Meta compilation
cmake -S $CMAKE_LISTS_TXT_DIR -B build
if [ $? -ne 0 ]; then
  test_failure "Meta compilation failed"
else
  unit_testsunit_teststest_success
fi
##

## Compilation
make -C build
if [ $? -ne 0 ]; then
  test_failure "Compilation failed"
else
  doxygendoxygentest_success
fi
##

## clang-format
make -C build clang-format
if [ $? -ne 0 ]; then
  test_failure "clang-format cmake module failed"
else
  test_success "clang-format cmake module"
fi
##

## clang-tidy
make -C build clang-tidy
if [ $? -ne 0 ]; then
  test_failure "clang-tidy cmake module failed"
else
  test_success "clang-tidy cmake module"
fi
##

## cmake-format
make -C build cmake-format
if [ $? -ne 0 ]; then
  test_failure "cmake-format cmake module failed"
else
  test_success "cmake-format cmake module"
fi
##

## functional tests
cmake -D functional_tests=true -S $CMAKE_LISTS_TXT_DIR -B build
if [ $? -ne 0 ]; then
  test_failure "functional_tests cmake module failed"
else
  test_success "functional_tests cmake module"
fi
make -C build functional_tests

if [ $? -ne 0 ]; then
  test_failure "functional_tests cmake module failed"
else
  test_success "functional_tests cmake module"
fi
##

## unit tests
cmake -D unit_tests=true -S $CMAKE_LISTS_TXT_DIR -B build
if [ $? -ne 0 ]; then
  test_failure "unit_tests cmake module failed"
else
  test_success "unit_tests cmake module"
fi

make -C build unit_tests
if [ $? -ne 0 ]; then
  test_failure "unit_tests cmake module failed"
else
  test_success "unit_tests cmake module"
fi
##

## doxygen
cmake -D doxygen=true -S $CMAKE_LISTS_TXT_DIR -B build
if [ $? -ne 0 ]; then
  test_failure "doxygen cmake module failed"
else
  test_success "doxygen cmake module"
fi

make -C build doxygen
if [ $? -ne 0 ]; then
  test_failure "doxygen cmake module failed"
else
  test_success "doxygen cmake module"
fi
##

test_results