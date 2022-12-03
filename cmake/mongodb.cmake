
function(find_mongo_packages)
    find_package(libbsoncxx REQUIRED PATHS /opt/mongo)
    find_package(libmongocxx REQUIRED PATHS /opt/mongo)
endfunction()

find_mongo_packages()

## todo use install_mongocxx.sh when libraries are not found
if (NOT DEFINED LIBMONGOCXX_LIBRARIES OR NOT DEFINED LIBBSONCXX_LIBRARIES)
    EXECUTE_PROCESS(COMMAND ./install_mongocxx.sh)
    find_mongo_packages()
endif ()

include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
#include_directories(./third_parties/mongo-cxx-driver-r3.7.0/build/install/lib)

include_directories("/opt/mongo/include/mongocxx/v_noabi")
include_directories("/opt/mongo/include/bsoncxx/v_noabi")
include_directories("/opt/mongo/include/libmongoc-1.0")
include_directories("/opt/mongo/include/libbson-1.0")
include_directories("/opt/mongo/lib")

target_link_libraries(${EXECUTABLE_NAME} ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME} ${LIBBSONCXX_LIBRARIES})