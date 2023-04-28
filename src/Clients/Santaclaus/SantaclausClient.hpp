/**
 * @file SantaclausClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 14/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_SANTACLAUS_CLIENT_HPP
#define MAESTRO_SANTACLAUS_CLIENT_HPP

#include <optional>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_Santaclaus/Maestro_Santaclaus.grpc.pb.h"

#include "utils.hpp"

class SantaclausClient {
  public:
    SantaclausClient(const std::shared_ptr<grpc::ChannelInterface> &channel);
    ~SantaclausClient() = default;

    NODISCARD Maestro_Santaclaus::AddFileStatus addFile(const File::FileApproxMetadata &file, const uint64 &fileSize) const;
    NODISCARD Maestro_Santaclaus::GetFileStatus getFile(const string &fileId) const;
    NODISCARD Maestro_Santaclaus::GetDirectoryStatus
    getDirectory(const string &userId, const std::optional<string> &dirId = std::nullopt, bool isRecursive = false) const;

  private:
    std::unique_ptr<Maestro_Santaclaus::Maestro_Santaclaus_Service::Stub> _stub;
};

#endif