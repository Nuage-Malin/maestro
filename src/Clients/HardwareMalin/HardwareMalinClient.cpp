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

HardwareMalinClient::HardwareMalinClient(const std::shared_ptr<grpc::ChannelInterface> &channel, const EventsManager &events)
    : _stub(Maestro_HardwareMalin::Maestro_HardwareMalin_Service::NewStub(channel)), _events(events)
{
    if (!this->_stub)
        throw std::runtime_error(STR_FUNCTION + " could not create gRPC stub");
}

bool HardwareMalinClient::diskStatus(const string &diskId) const
{
    grpc::ClientContext context;
    Maestro_HardwareMalin::DiskStatusRequest request;
    Maestro_HardwareMalin::DiskStatusStatus response;

    request.set_diskid(diskId);
    grpc::Status status = this->_stub->diskStatus(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response.status();
}

void HardwareMalinClient::setDiskState(const string &diskId, bool state) const
{
    if (!state)
        this->_events.emit<const string &>(Event::DISK_SHUTDOWN_BEFORE, diskId);
    grpc::ClientContext context;
    Maestro_HardwareMalin::SetDiskStateRequest request;
    Maestro_HardwareMalin::SetDiskStateStatus response;

    request.set_diskid(diskId);
    request.set_state(state);
    grpc::Status status = this->_stub->setDiskState(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    if (state)
        this->_events.emit<const string &>(Event::DISK_STARTUP, diskId);
    else
        this->_events.emit<const string &>(Event::DISK_SHUTDOWN, diskId);
}