#
# EPITECH PROJECT, 2022 gTest.cmake File description: Install googletest,
# build and run test suite
#
# todo rename as unit_tests

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True) set(CMAKE_CXX_COMPILER g++)

set(TEST test_testing)
set(TEST_EXEC_NAME unit_tests)
project(
        ${TEST}
        VERSION 0.1
        LANGUAGES CXX)

get_filename_component(UNIT_TEST_DIR "${TEST_DIR}/unit/" ABSOLUTE)
if (NOT EXISTS "${UNIT_TEST_DIR}")
    message(FATAL_ERROR "Could not find unit test directory")
endif ()
get_filename_component(TEST_SRC_DIR "${UNIT_TEST_DIR}/src/" ABSOLUTE)
if (NOT EXISTS ${TEST_SRC_DIR})
    message(FATAL_ERROR "Could not find unit test src directory")
endif ()

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
FetchContent_MakeAvailable(googletest) # TODO make this non-fatal so that the entire build doesn't fail when no connection

include(GoogleTest)
# gRPC
include("${CMAKE_MODULES_DIR}/gRPC_cpp.cmake")
#
add_executable(${TEST_EXEC_NAME}
               ${TEST_SRC_DIR}/Utils/Date/Date.cpp
               ${TEST_SRC_DIR}/UsersBack/UsersBackClient.cpp
               ${TEST_SRC_DIR}/UsersBack/UsersBackTest.cpp)

target_include_directories(${TEST_EXEC_NAME} PUBLIC ${INCL_DIR} ${TEST_SRC_DIR})
target_link_libraries(${TEST_EXEC_NAME} gtest_main)


# gRPC
target_link_libraries(${TEST_EXEC_NAME} ${my_grpc_proto} ${_REFLECTION}
                      ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
#
gtest_discover_tests(${TEST_EXEC_NAME})

# same GNU '_Safe_iterator' error when using this method :
# include_directories(third_party/googletest-release-1.11.0)
# target_include_directories(${TEST_EXEC_NAME} PUBLIC
# third_party/googletest-release-1.11.0/
# third_party/googletest-release-1.11.0/googletest/include)