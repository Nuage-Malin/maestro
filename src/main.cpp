/**
 * @file main.cpp
 * @author Arthur Jourdan
 * @date of creation 22/10/22.
 * @brief TODO
 */

#include <cstdlib>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <grpcpp/server_builder.h>
// #include <grpcpp/server.h>

#include "File/FileServer.hpp"

void RunServer(const std::string &db_path)
{
    // db instanciationn // todo put somewhere else
    mongocxx::instance inst{}; // This should be done only once.
    mongocxx::client conn{
        mongocxx::uri{"mongodb://maestro_admin:maestro_admin_password@mongo:27017/maestro?authSource=admin"}};
    mongocxx::database db = conn["Files"];
    mongocxx::collection coll = db["Uploaded"];
    FileServer service(coll);
    //
    std::string server_address("0.0.0.0:50051");

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

/**
 * @brief main function
 * @param ac Number of arguments
 * @param av Argument vector
 * @return EXIT_SUCCESS if success, EXIT_FAILURE otherwise
 */
int main(__attribute__((unused)) const int ac, __attribute__((unused)) const char *av[])
{

    return EXIT_SUCCESS;
}