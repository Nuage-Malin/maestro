/**
 * @file ExternalBugleClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 09/12/2023
 * @copyright Nuage Malin
 */

#include "ExternalBugleClient.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

ExternalBugleClient::ExternalBugleClient(const std::shared_ptr<grpc::ChannelInterface> &channel)
    : _santaclausStub(Santaclaus_HardwareMalin::Santaclaus_HardwareMalin_Service::NewStub(channel)) {}

Santaclaus_HardwareMalin::GetDisksStatus ExternalBugleClient::getDisks() const {
    Santaclaus_HardwareMalin::GetDisksRequest request;
    Santaclaus_HardwareMalin::GetDisksStatus response;
    grpc::ClientContext context;


    this->_callLogger("getDisks");
    grpc::Status status = this->_santaclausStub->getDisks(&context, request, &response);

    if (!status.ok()) {
        throw RequestFailureException(status, __FUNCTION__);
    }

    return response;
}

void ExternalBugleClient::_callLogger(const string &functionName) const
{
    std::cout << "[CLIENT] external bugle - " << functionName << std::endl;
}