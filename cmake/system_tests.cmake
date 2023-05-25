set(SYSTEM_TESTS system_tests)
set(SYSTEM_TESTS_SRC_DIR ${CMAKE_HOME_DIRECTORY}/tests/system/src/)
set(SYSTEM_TESTS_INC_DIR ${CMAKE_HOME_DIRECTORY}/tests/system/include/)

set(SYSTEM_TESTS_SRCS ${SYSTEM_TESTS_SRC_DIR}main.cpp)

add_executable(${SYSTEM_TESTS} ${SYSTEM_TESTS_SRCS})

target_include_directories(${SYSTEM_TESTS} PUBLIC ${SYSTEM_TESTS_INC_DIR}
                                                ${INCL_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(${SYSTEM_TESTS} PRIVATE -Wall -Wextra)
endif()
