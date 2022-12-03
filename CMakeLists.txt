# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# cmake build file for C++ helloworld example. Assumes protobuf and gRPC have
# been installed using cmake. See cmake_externalproject/CMakeLists.txt for
# all-in-one cmake build that automatically builds all the dependencies before
# building helloworld.

cmake_minimum_required(VERSION 3.5.1)

set(PROJECT_NAME cmake-template)
set(EXECUTABLE_NAME maestro)

project(${PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

get_filename_component(THIRD_PARTIES_DIR
                       "${CMAKE_HOME_DIRECTORY}/third_parties/" ABSOLUTE)
get_filename_component(CMAKE_MODULES_DIR "${CMAKE_HOME_DIRECTORY}/cmake/"
                       ABSOLUTE)
get_filename_component(INCL_DIR "${CMAKE_HOME_DIRECTORY}/include/" ABSOLUTE)
get_filename_component(SRC_DIR "${CMAKE_HOME_DIRECTORY}/src/" ABSOLUTE)
get_filename_component(DOCS_DIR "${CMAKE_HOME_DIRECTORY}/docs/" ABSOLUTE)

set(SRCS ${SRC_DIR}/main.cpp
    ${SRC_DIR}/File/FileServer.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY})

#if(DEFINED gRPC)
  include("cmake/gRPC_cpp.cmake")
#endif()

add_executable(${EXECUTABLE_NAME} ${SRCS})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${INCL_DIR} ${SRC_DIR} ${my_protos_include_dir})

#if(DEFINED gRPC)
  target_link_libraries(${EXECUTABLE_NAME} ${my_grpc_proto} ${_REFLECTION}
                        ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
#endif()
include(${CMAKE_MODULES_DIR}/mongodb.cmake)

# custom cmake modules
include(${CMAKE_MODULES_DIR}/clang-format.cmake)
include(${CMAKE_MODULES_DIR}/clang-tidy.cmake)
include(${CMAKE_MODULES_DIR}/cmake-format.cmake)

if(DEFINED functional_tests)
  include(${CMAKE_MODULES_DIR}/functional_tests.cmake)
endif()

#if(DEFINED unit_tests)
  include(${CMAKE_MODULES_DIR}/gTest.cmake)
#endif()

if(DEFINED doxygen)
  include(${CMAKE_MODULES_DIR}/doxygen.cmake)
endif()
