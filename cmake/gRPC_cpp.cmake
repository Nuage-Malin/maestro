# TODO ensure that installation succeed and variables are set
# include("${CMAKE_MODULES_DIR}/cmake/manually_install_gRPC.cmake")
include("${CMAKE_MODULES_DIR}/install_gRPC.cmake")

# Proto file TODO directory that contains protofiles as submodule of this repo
get_filename_component(my_protos_path
                       "${THIRD_PARTIES_DIR}/protobuf-interfaces/" ABSOLUTE)
if(NOT EXISTS ${my_protos_path})
  message(FATAL_ERROR "Could not find ${my_protos_path}, exiting")
endif()

set(my_protos "${my_protos_path}/PoC.proto")

get_filename_component(my_generated_path "${my_protos_path}/generated/"
                       ABSOLUTE)

# Generated sources
set(my_protos_srcs "${my_generated_path}/PoC.pb.cc")
set(my_protos_hdrs "${my_generated_path}/PoC.pb.h")
set(my_grpc_srcs "${my_generated_path}/PoC.grpc.pb.cc")
set(my_grpc_hdrs "${my_generated_path}/PoC.grpc.pb.h")
add_custom_command(
  OUTPUT "${my_protos_srcs}" "${my_protos_hdrs}" "${my_grpc_srcs}"
         "${my_grpc_hdrs}"
  COMMAND
    ${_PROTOBUF_PROTOC} ARGS --grpc_out "${my_generated_path}" --cpp_out
    "${my_generated_path}" -I "${my_protos_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}" "${my_protos}"
  DEPENDS "${my_protos}")

# Include generated *.pb.h files
include_directories("${my_generated_path}")

# ${my_grpc_proto}
set(my_grpc_proto my_grpc_proto)
add_library(${my_grpc_proto} ${my_grpc_srcs} ${my_grpc_hdrs} ${my_protos_srcs}
                             ${my_protos_hdrs})
target_link_libraries(${my_grpc_proto} ${_REFLECTION} ${_GRPC_GRPCPP}
                      ${_PROTOBUF_LIBPROTOBUF})
