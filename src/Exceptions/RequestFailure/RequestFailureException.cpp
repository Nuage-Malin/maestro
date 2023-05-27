/**
 * @file RequestFailureException.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#include "RequestFailureException.hpp"

RequestFailureException::RequestFailureException(const GrpcStatus &status, string functionName)
    : std::runtime_error(functionName + ": [" + status.error_name() + "] " + status.error_message()),
      _details(status.error_details())
{
}

NODISCARD const string &RequestFailureException::getDetails() const noexcept
{
    return this->_details;
}