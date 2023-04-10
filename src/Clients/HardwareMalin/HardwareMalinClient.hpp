/**
 * @file HardwareMalinClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_HARDWAREMALIN_CLIENT_HPP
#define MAESTRO_HARDWAREMALIN_CLIENT_HPP

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_HardwareMalin/Maestro_HardwareMalin.grpc.pb.h"

#include "utils.hpp"
#include "EventsManager.hpp"

class HardwareMalinClient {
  public:
    HardwareMalinClient(const std::shared_ptr<grpc::ChannelInterface> &channel, const EventsManager &events);
    ~HardwareMalinClient() = default;

    NODISCARD bool diskStatus(const string &diskId) const;
    void setDiskState(const string &diskId, bool state) const;

  private:
    std::unique_ptr<Maestro_HardwareMalin::Maestro_HardwareMalin_Service::Stub> _stub;
    const EventsManager &_events;
};

#endif