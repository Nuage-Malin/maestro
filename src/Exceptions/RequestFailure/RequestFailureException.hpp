/**
 * @file RequestFailureException.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 * @brief Exception thrown by a client when a request to a gRPC service fails
 */

#ifndef MAESTRO_REQUESTFAILURE_EXCEPTION_HPP
#define MAESTRO_REQUESTFAILURE_EXCEPTION_HPP

#include <stdexcept>

#include <grpcpp/impl/status.h>

#include "Utils/GrpcStatus/GrpcStatus.hpp"

class RequestFailureException : public std::runtime_error {
  public:
    RequestFailureException(const GrpcStatus &status, string functionName = __FUNCTION__);
    ~RequestFailureException() = default;

    NODISCARD const string &getDetails() const noexcept;

  private:
    const string _details;
};

#endif