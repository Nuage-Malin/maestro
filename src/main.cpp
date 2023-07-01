/**
 * @file main.cpp
 * @author Arthur Jourdan
 * @date of creation 22/10/22.
 * @brief TODO
 */

#include <exception>

#include <mongocxx/instance.hpp>
#include <grpcpp/server_builder.h>

#include "utils.hpp"
#include "clients.hpp"
#include "Schemas/Mongo/Mongo.hpp"
#include "Services/UsersBack/UsersBackService.hpp"
#include "Services/Chouf/ChoufService.hpp"
#include "Cron/FileUpload/FileUploadCron.hpp"
#include "Cron/ExpiredDownloadedFiles/ExpiredDownloadedFilesCron.hpp"
#include "Cron/DownloadFiles/DownloadFilesCron.hpp"
#include "Cron/RemoveFiles/RemoveFilesCron.hpp"

/**
 * @brief Run the server
 * @throw std::invalid_argument if MAESTRO_MONGO_URL environment variable not found
 * @throw std::runtime_error if could not access mongo database
 * @throw std::runtime_error if could not access database or collection
 * @return void
 */
void RunServer()
{
    // Mongo
    mongocxx::instance mongoInstance{}; // This should be done only once.

    // Events
    EventsManager events;

    // Clients
    GrpcClients clients = {
        .santaclaus = SantaclausClient(grpc::CreateChannel(getEnv("MAESTRO_SANTACLAUS_URI"), grpc::InsecureChannelCredentials())),
        .bugle = BugleClient(grpc::CreateChannel(getEnv("MAESTRO_BUGLE_URI"), grpc::InsecureChannelCredentials()), events),
        .vault = VaultClient(grpc::CreateChannel(getEnv("MAESTRO_VAULT_URI"), grpc::InsecureChannelCredentials()))};

    // CRON
    ManagerCron managerCron;

    managerCron.add("0 30 3 * * ?", ExpiredDownloadedFilesCron(events));
    managerCron.add("0 0 3 * * ?", FileUploadCron(clients, events));
    managerCron.add("0 0 3 * * ?", DownloadFilesCron(clients, events));
    managerCron.add("0 0 3 * * ?", RemoveFilesCron(clients, events));

    // Services
    UsersBackService usersBackService(clients, events);
    ChoufService choufService(clients, events, managerCron);

    // gRPC
    const char *address = getenv("MAESTRO_ADDRESS");
    const string serverAddress(address);
    grpc::ServerBuilder builder;

    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&usersBackService);
    builder.RegisterService(&choufService);
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
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}