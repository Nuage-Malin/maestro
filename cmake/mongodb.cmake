
set(MONGOCXX_DIR ${THIRD_PARTIES_DIR}/mongo-cxx-driver)
set(MONGO_INSTALL_DIR ${MONGOCXX_DIR}/install)

find_package(libbsoncxx PATHS ${MONGO_INSTALL_DIR})
find_package(libmongocxx PATHS ${MONGO_INSTALL_DIR})

## todo use install_mongocxx.sh when libraries are not found
if (NOT DEFINED LIBMONGOCXX_LIBRARIES OR NOT DEFINED LIBBSONCXX_LIBRARIES)
    set(ENV{MONGOCXX_DIR} ${MONGOCXX_DIR})
    set(ENV{MONGO_INSTALL_DIR} ${MONGO_INSTALL_DIR})
    execute_process(COMMAND chmod +x ${CMAKE_MODULES_DIR}/install_mongocxx.sh)
    execute_process(COMMAND ${CMAKE_MODULES_DIR}/install_mongocxx.sh
                    RESULT_VARIABLE ret)
    if (NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "Could not install mongo C++")
    endif ()

    find_package(libbsoncxx REQUIRED PATHS ${MONGO_INSTALL_DIR})
    find_package(libmongocxx REQUIRED PATHS ${MONGO_INSTALL_DIR})
endif ()

include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories(${LIBMONGOCXX_INCLUDE_DIR})

include_directories("${MONGO_INSTALL_DIR}/include/bsoncxx/v_noabi")
include_directories("${MONGO_INSTALL_DIR}/include/mongocxx/v_noabi")
include_directories("${MONGO_INSTALL_DIR}/include/libbson-1.0")
include_directories("${MONGO_INSTALL_DIR}/include/libmongoc-1.0")
include_directories("${MONGO_INSTALL_DIR}/lib")