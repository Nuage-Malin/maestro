/**
 * @file NotFoundException.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_NOTFOUND_EXCEPTION_HPP
#define MAESTRO_NOTFOUND_EXCEPTION_HPP

#include <stdexcept>

#include <grpcpp/impl/status.h>

#include "Utils/GrpcStatus/GrpcStatus.hpp"

class NotFoundException : public std::runtime_error {
  public:
    NotFoundException(const string &message, string functionName = __FUNCTION__);
    ~NotFoundException() = default;
};

#endif