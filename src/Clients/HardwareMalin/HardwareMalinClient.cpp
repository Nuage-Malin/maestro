/**
 * @file HardwareMalinClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#include <string>

#include <grpcpp/channel.h>

#include "HardwareMalinClient.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

HardwareMalinClient::HardwareMalinClient(const std::shared_ptr<grpc::ChannelInterface> &channel)
    : _stub(Maestro_HardwareMalin::Maestro_HardwareMalin_Service::NewStub(channel))
{
    if (!this->_stub)
        throw std::runtime_error(string(__FUNCTION__) + " could not create gRPC stub");
}

bool HardwareMalinClient::diskStatus(const string &diskId) const
{
    grpc::ClientContext context;
    Maestro_HardwareMalin::DiskStatusRequest request;
    Maestro_HardwareMalin::DiskStatusStatus response;

    request.set_diskid(diskId);
    grpc::Status status = this->_stub->diskStatus(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status);
    return response.status();
}