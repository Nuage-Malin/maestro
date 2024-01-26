/**
 * @file ExternalBugleClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 09/12/2023
 * @copyright Nuage Malin
 */

#ifndef EXTERNAL_BUGLE_CLIENT_HPP
#define EXTERNAL_BUGLE_CLIENT_HPP

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Santaclaus_HardwareMalin/Santaclaus_HardwareMalin.grpc.pb.h"

#include "utils.hpp"
#include "EventsManager.hpp"

class ExternalBugleClient {
  public:
    ExternalBugleClient(const std::shared_ptr<grpc::ChannelInterface> &channel);
    ~ExternalBugleClient() = default;

    NODISCARD Santaclaus_HardwareMalin::GetDisksStatus getDisks() const;

  private:
    void _callLogger(const string &functionName) const;

  private:
    std::unique_ptr<Santaclaus_HardwareMalin::Santaclaus_HardwareMalin_Service::Stub> _santaclausStub;
};

#endif