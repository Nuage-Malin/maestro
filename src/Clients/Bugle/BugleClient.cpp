/**
 * @file BugleClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#include <string>

#include <grpcpp/channel.h>

#include "BugleClient.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

BugleClient::BugleClient(const std::shared_ptr<grpc::ChannelInterface> &channel, const EventsManager &events)
    : _stub(Maestro_HardwareMalin::Maestro_HardwareMalin_Service::NewStub(channel)), _events(events)
{
    if (!this->_stub)
        throw std::runtime_error(STR_FUNCTION + " could not create gRPC stub");
}

bool BugleClient::diskStatus(const string &diskId) const
{
    grpc::ClientContext context;
    Maestro_HardwareMalin::DiskStatusRequest request;
    Maestro_HardwareMalin::DiskStatusStatus response;

    request.set_diskid(diskId);
    this->_callLogger(__FUNCTION__);
    grpc::Status status = this->_stub->diskStatus(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response.status();
}

void BugleClient::setDiskState(const string &diskId, bool state) const
{
    if (!state)
        this->_events.emit<const string &>(Event::DISK_SHUTDOWN_BEFORE, diskId);
    grpc::ClientContext context;
    Maestro_HardwareMalin::SetDiskStateRequest request;
    Maestro_HardwareMalin::SetDiskStateStatus response;

    request.set_diskid(diskId);
    request.set_state(state);
    this->_callLogger(__FUNCTION__);
    grpc::Status status = this->_stub->setDiskState(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    if (state)
        this->_events.emit<const string &>(Event::DISK_STARTUP, diskId);
    else
        this->_events.emit<const string &>(Event::DISK_SHUTDOWN, diskId);
}

void BugleClient::_callLogger(const string &functionName) const
{
    std::cout << "[CLIENT] bugle - " << functionName << std::endl;
}