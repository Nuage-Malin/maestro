/**
 * @file main.cpp
 * @author Arthur Jourdan
 * @date of creation 22/10/22.
 * @brief TODO
 */

#include <exception>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <grpcpp/server_builder.h>
// #include <grpcpp/server.h>

#include "utils.hpp"
#include "File/FileServer.hpp"

static const string dbName{"Files"};
static const string collName{"Uploaded"};

/**
 * @brief Run the server
 * @throw std::invalid_argument if MAESTRO_MONGO_URL environment variable not found
 * @throw std::runtime_error if could not access mongo database
 * @throw std::runtime_error if could not access database or collection
 * @return void
 */
void RunServer()
{
    // mongo db instanciationn // todo put somewhere else
    mongocxx::instance inst{}; // This should be done only once.
    const char *mongoUrl = getenv("MAESTRO_MONGO_URL");
    if (!mongoUrl)
        throw std::invalid_argument("MAESTRO_MONGO_URL environment variable not found");
    mongocxx::client conn{mongocxx::uri{mongoUrl}};
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
    const char *address = getenv("MAESTRO_ADDRESS");
    const string serverAddress(address);
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
int main(UNUSED const int ac, UNUSED const char *av[])
{
    try {
        RunServer();
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}