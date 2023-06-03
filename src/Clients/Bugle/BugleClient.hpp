/**
 * @file BugleClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_BUGLE_CLIENT_HPP
#define MAESTRO_BUGLE_CLIENT_HPP

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_HardwareMalin/Maestro_HardwareMalin.grpc.pb.h"

#include "utils.hpp"
#include "EventsManager.hpp"

class BugleClient {
  public:
    BugleClient(const std::shared_ptr<grpc::ChannelInterface> &channel, const EventsManager &events);
    ~BugleClient() = default;

    NODISCARD bool diskStatus(const string &diskId) const;
    void setDiskState(const string &diskId, bool state) const;

  private:
    void _callLogger(const string &functionName) const;

  private:
    std::unique_ptr<Maestro_HardwareMalin::Maestro_HardwareMalin_Service::Stub> _stub;
    const EventsManager &_events;
};

#endif