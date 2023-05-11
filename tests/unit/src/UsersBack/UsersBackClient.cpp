/**
 * @file UsersBackClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/02/2023
 * @copyright Nuage Malin
 */

#include <string>

#include <grpcpp/channel.h>

#include "UsersBackClient.hpp"

UsersBackClient::UsersBackClient(const string &serverIP)
    : _client(
        UsersBack_Maestro::UsersBack_Maestro_Service::NewStub(grpc::CreateChannel(serverIP, grpc::InsecureChannelCredentials()))
    )
{
    if (!this->_client)
        throw std::runtime_error("Could not create gRPC stub");
}

bool UsersBackClient::getUserConsumption(
    UsersBack_Maestro::GetUserConsumptionStatus &response, const UsersBack_Maestro::GetUserConsumptionRequest &request
) const
{
    grpc::ClientContext context;
    grpc::Status status = this->_client->getUserConsumption(&context, request, &response);

    if (!status.ok()) {
        std::cerr << __FUNCTION__ << " request failed: " << status.error_message() << std::endl;
        return false;
    }
    return true;
}