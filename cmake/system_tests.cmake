set(SYSTEM_TESTS system_tests)
set(SYSTEM_TESTS_SRC_DIR ${CMAKE_HOME_DIRECTORY}/tests/system/src)
set(SYSTEM_TESTS_INC_DIR ${CMAKE_HOME_DIRECTORY}/tests/system/include/)

set(SYSTEM_TESTS_SRCS
    ${SYSTEM_TESTS_SRC_DIR}/main.cpp
    ${SYSTEM_TESTS_SRC_DIR}/UsersBack/client/UsersBackClient.cpp
    ${SYSTEM_TESTS_SRC_DIR}/UsersBack/tests/UsersBackTests.cpp
)

add_executable(${SYSTEM_TESTS} ${SYSTEM_TESTS_SRCS})

set_property(TARGET ${SYSTEM_TESTS} PROPERTY CXX_STANDARD 20)
target_include_directories(${SYSTEM_TESTS} PUBLIC ${SYSTEM_TESTS_SRC_DIR} ${SYSTEM_TESTS_INC_DIR} ${INCL_DIR})

# gRPC
target_link_libraries(${SYSTEM_TESTS} ${my_grpc_proto} ${_REFLECTION}
    ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(${SYSTEM_TESTS} PRIVATE -Wall -Wextra)
endif()
