/**
 * @file GrpcStatus.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 * @brief grpc::Status wrapper
 */

#ifndef MAESTRO_UTILS_GRPC_STATUS_HPP
#define MAESTRO_UTILS_GRPC_STATUS_HPP

#include <grpcpp/impl/status.h>

#include "utils.hpp"

class GrpcStatus : public grpc::Status {
  public:
    GrpcStatus() = default;
    GrpcStatus(const grpc::Status &status);
    ~GrpcStatus() = default;

    string error_name() const;
};

#endif