if (NOT DEFINED GRPC_CPP_ONCE)
    set(GRPC_CPP_ONCE true)

    get_filename_component(GRPC_SUBMODULE_DIR "${THIRD_PARTIES_DIR}/grpc/" ABSOLUTE)
    if (NOT EXISTS ${GRPC_SUBMODULE_DIR})
        set(GRPC_CLONE_SUBMODULE true)
    endif ()
    set(GRPC_FULL_INSTALL true)

    include("${CMAKE_MODULES_DIR}/install_gRPC.cmake")

    # Proto file
    get_filename_component(my_protos_path
                           "${THIRD_PARTIES_DIR}/protobuf-interfaces/" ABSOLUTE)
    if (NOT EXISTS ${my_protos_path})
        message(FATAL_ERROR "Could not find ${my_protos_path}, exiting")
    endif ()

    get_filename_component(my_protos_path_src
                           "${my_protos_path}/src/" ABSOLUTE)
    if (NOT EXISTS ${my_protos_path_src})
        message(FATAL_ERROR "Could not find ${my_protos_path_src}, exiting")
    endif ()

    get_filename_component(my_generated_path "${my_protos_path}/generated/"
                           ABSOLUTE)
    if (NOT EXISTS ${my_generated_path})
        make_directory(${my_generated_path})
    endif ()

    # Protobuf inpout file
    set(my_protos "${my_protos_path_src}/common/File.proto"
        "${my_protos_path_src}/UsersBack_Maestro/UsersBack_Maestro.proto"
        "${my_protos_path_src}/Maestro_Santaclaus/Maestro_Santaclaus.proto"
        "${my_protos_path_src}/Maestro_HardwareMalin/Maestro_HardwareMalin.proto"
        "${my_protos_path_src}/Maestro_Vault/Maestro_Vault.proto")


    # Generated sources
    set(my_protos_srcs "${my_generated_path}/common/File.pb.cc"
        "${my_generated_path}/UsersBack_Maestro/UsersBack_Maestro.pb.cc"
        "${my_generated_path}/Maestro_Santaclaus/Maestro_Santaclaus.pb.cc"
        "${my_generated_path}/Maestro_HardwareMalin/Maestro_HardwareMalin.pb.cc"
        "${my_generated_path}/Maestro_Vault/Maestro_Vault.pb.cc")

    set(my_protos_hdrs "${my_generated_path}/common/File.pb.h"
        "${my_generated_path}/UsersBack_Maestro/UsersBack_Maestro.pb.h"
        "${my_generated_path}/Maestro_Santaclaus/Maestro_Santaclaus.pb.h"
        "${my_generated_path}/Maestro_HardwareMalin/Maestro_HardwareMalin.pb.h"
        "${my_generated_path}/Maestro_Vault/Maestro_Vault.pb.h")

    set(my_grpc_srcs "${my_generated_path}/common/File.grpc.pb.cc"
        "${my_generated_path}/UsersBack_Maestro/UsersBack_Maestro.grpc.pb.cc"
        "${my_generated_path}/Maestro_Santaclaus/Maestro_Santaclaus.grpc.pb.cc"
        "${my_generated_path}/Maestro_HardwareMalin/Maestro_HardwareMalin.grpc.pb.cc"
        "${my_generated_path}/Maestro_Vault/Maestro_Vault.grpc.pb.cc")

    set(my_grpc_hdrs "${my_generated_path}/common/File.grpc.pb.h"
        "${my_generated_path}/UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"
        "${my_generated_path}/Maestro_Santaclaus/Maestro_Santaclaus.grpc.pb.h"
        "${my_generated_path}/Maestro_HardwareMalin/Maestro_HardwareMalin.grpc.pb.h"
        "${my_generated_path}/Maestro_Vault/Maestro_Vault.grpc.pb.h")

    set(my_protos_include_dir ${my_generated_path}/)

    add_custom_command(
            OUTPUT ${my_protos_srcs} ${my_protos_hdrs} ${my_grpc_srcs} ${my_grpc_hdrs}
            COMMAND
            ${_PROTOBUF_PROTOC} ARGS --grpc_out ${my_generated_path} --cpp_out
            ${my_generated_path} -I ${my_protos_path_src} -I ${my_protos_path} -I /usr/include
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE} ${my_protos}
            DEPENDS ${my_protos})

    # Include generated *.pb.h files
    include_directories("${my_generated_path}")

    # ${my_grpc_proto}
    set(my_grpc_proto my_grpc_proto)
    add_library(${my_grpc_proto} ${my_grpc_srcs} ${my_grpc_hdrs} ${my_protos_srcs}
                ${my_protos_hdrs})
    target_link_libraries(${my_grpc_proto} ${_REFLECTION} ${_GRPC_GRPCPP}
                          ${_PROTOBUF_LIBPROTOBUF})
endif ()