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
#include <exception>

static const std::string dbName{"Files"};
static const std::string collName{"Uploaded"};

void RunServer()
{
    // mongo db instanciationn // todo put somewhere else
    mongocxx::instance inst{}; // This should be done only once.
    mongocxx::client conn{
        mongocxx::uri{"mongodb://maestro_admin:maestro_admin_password@mongo:27017/maestro?authSource=admin"}};
    if (!conn)
        throw std::runtime_error("Could not access mongo database");
    mongocxx::database db = conn[dbName];
    if (!db)
        throw std::runtime_error("Could not access database '" + dbName + "'");
    mongocxx::collection coll = db[collName];
    if (!db)
        throw std::runtime_error("Could not access collection '" + collName + "'");

    // Backend
    FileServer service(db);

    // gRPC
    std::string serverAddress(std::string("127.0.0.1:") + std::getenv("MAESTRO_PORT"));
    grpc::ServerBuilder builder;

    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;
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
    RunServer();
    return EXIT_SUCCESS;
}