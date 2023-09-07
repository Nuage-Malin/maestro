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

set(PROJECT_NAME maestro)
set(EXECUTABLE_NAME maestro)

project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(THIRD_PARTIES_DIR
                       "${CMAKE_HOME_DIRECTORY}/third_parties/" ABSOLUTE)
get_filename_component(CMAKE_MODULES_DIR "${CMAKE_HOME_DIRECTORY}/cmake/"
                       ABSOLUTE)
get_filename_component(INCL_DIR "${CMAKE_HOME_DIRECTORY}/include/" ABSOLUTE)
get_filename_component(SRC_DIR "${CMAKE_HOME_DIRECTORY}/src/" ABSOLUTE)
get_filename_component(DOCS_DIR "${CMAKE_HOME_DIRECTORY}/docs/" ABSOLUTE)
get_filename_component(TEST_DIR "${CMAKE_HOME_DIRECTORY}/tests/" ABSOLUTE)

set(SRCS
    ${SRC_DIR}/Utils/Date/Date.cpp
    ${SRC_DIR}/Utils/GrpcStatus/GrpcStatus.cpp
    ${SRC_DIR}/Cron/Manager/ManagerCron.cpp
    ${SRC_DIR}/Cron/Template/TemplateCron.cpp
    ${SRC_DIR}/Cron/FileUpload/FileUploadCron.cpp
    ${SRC_DIR}/Cron/ExpiredDownloadedFiles/ExpiredDownloadedFilesCron.cpp
    ${SRC_DIR}/Cron/DownloadFiles/DownloadFilesCron.cpp
    ${SRC_DIR}/Cron/RemoveFiles/RemoveFilesCron.cpp
    ${SRC_DIR}/Exceptions/RequestFailure/RequestFailureException.cpp
    ${SRC_DIR}/Exceptions/NotFound/NotFoundException.cpp
    ${SRC_DIR}/Schemas/Mongo/Mongo.cpp
    ${SRC_DIR}/Schemas/Templates/Schema/TemplateSchema.cpp
    ${SRC_DIR}/Schemas/Templates/FileBucket/TemplateFileBucket.cpp
    ${SRC_DIR}/Schemas/Files/UploadQueue/UploadQueueSchema.cpp
    ${SRC_DIR}/Schemas/Files/DownloadedStack/DownloadedStackSchema.cpp
    ${SRC_DIR}/Schemas/Files/DownloadQueue/DownloadQueueSchema.cpp
    ${SRC_DIR}/Schemas/Files/RemoveQueue/RemoveQueueSchema.cpp
    ${SRC_DIR}/Schemas/Stats/DiskWakeup/DiskWakeupSchema.cpp
    ${SRC_DIR}/Schemas/Stats/UserDiskInfo/UserDiskInfoSchema.cpp
    ${SRC_DIR}/Services/Template/TemplateService.cpp
    ${SRC_DIR}/Services/UsersBack/UsersBackService.cpp
    ${SRC_DIR}/Services/Chouf/ChoufService.cpp
    ${SRC_DIR}/Clients/Santaclaus/SantaclausClient.cpp
    ${SRC_DIR}/Clients/Bugle/BugleClient.cpp
    ${SRC_DIR}/Clients/Vault/VaultClient.cpp)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY})

if (DEFINED install AND install STREQUAL "true")
    # gRPC
    include("${CMAKE_MODULES_DIR}/install_gRPC.cmake")

    # MongoDB
    include(${CMAKE_MODULES_DIR}/mongodb.cmake)

    # LibCron
    include(${CMAKE_MODULES_DIR}/libcron.cmake)
endif ()

if (DEFINED build AND build STREQUAL "true")
    include("${CMAKE_MODULES_DIR}/gRPC_cpp.cmake")

    add_executable(${EXECUTABLE_NAME} ${SRC_DIR}/main.cpp ${SRCS})
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 20)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${INCL_DIR} ${SRC_DIR} ${my_protos_include_dir})

    # gRPC
    target_link_libraries(${EXECUTABLE_NAME} ${my_grpc_proto} ${_REFLECTION}
                        ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

    # MongoDB
    target_link_libraries(${EXECUTABLE_NAME} ${LIBBSONCXX_LIBRARIES})
    target_link_libraries(${EXECUTABLE_NAME} ${LIBMONGOCXX_LIBRARIES})

    # LibCron
    target_link_libraries(${EXECUTABLE_NAME} ${LIBCRON_LIBRARIES})

    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif ()

# custom cmake modules
include(${CMAKE_MODULES_DIR}/clang-format.cmake)
include(${CMAKE_MODULES_DIR}/clang-tidy.cmake)
include(${CMAKE_MODULES_DIR}/cmake-format.cmake)

if (DEFINED functional_tests AND functional_tests STREQUAL "true")
    include(${CMAKE_MODULES_DIR}/functional_tests.cmake)
endif ()

if (DEFINED unit_tests AND unit_tests STREQUAL "true") ## todo build only unit_tests, not maestro ? for docker compose unit_tests
    include(${CMAKE_MODULES_DIR}/gTest.cmake)
endif ()

if (DEFINED system_tests AND system_tests STREQUAL "true") ## todo build only system_tests, not maestro ? for docker compose unit_tests
    include("${CMAKE_MODULES_DIR}/gRPC_cpp.cmake")

    include(${CMAKE_MODULES_DIR}/system_tests.cmake)
endif ()

if (DEFINED doxygen AND doxygen STREQUAL "true")
    include(${CMAKE_MODULES_DIR}/doxygen.cmake)
endif ()