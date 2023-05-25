/**
 * @file UsersBackClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/02/2023
 * @copyright Nuage Malin
 */

#include <grpcpp/channel.h>

#include "UsersBackClient.hpp"

UsersBackClient::UsersBackClient(const string &serverURI)
    : _client(
        UsersBack_Maestro::UsersBack_Maestro_Service::NewStub(grpc::CreateChannel(serverURI, grpc::InsecureChannelCredentials()))
    )
{
    if (!this->_client)
        throw std::runtime_error("Could not create gRPC stub");
}