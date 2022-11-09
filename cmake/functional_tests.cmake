#
# EPITECH PROJECT, 2022 functional_tests.cmake File description: Compile example
#

# TODO make actual CMakeLists.txt, place it into examples folder ?

set(FUNC_TESTS functional_tests)
set(FUNC_TESTS_SRC_DIR ${CMAKE_HOME_DIRECTORY}/tests/functional/src/)
set(FUNC_TESTS_INC_DIR ${CMAKE_HOME_DIRECTORY}/tests/functional/include/)

set(FUNC_TESTS_SRCS ${FUNC_TESTS_SRC_DIR}main.cpp)

add_executable(${FUNC_TESTS} ${FUNC_TESTS_SRCS})

target_include_directories(${FUNC_TESTS} PUBLIC ${FUNC_TESTS_INC_DIR}
                                                ${INCL_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(${FUNC_TESTS} PRIVATE -Wall -Wextra)
endif()
