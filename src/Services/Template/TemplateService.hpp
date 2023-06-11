/**
 * @file TemplateService.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Template service that should be used as a base for all services
 */

#ifndef MAESTRO_TEMPLATE_SERVICE_HPP
#define MAESTRO_TEMPLATE_SERVICE_HPP

#include <source_location>

#include <grpcpp/support/status.h>
#include <mongocxx/exception/query_exception.hpp>

#include "utils.hpp"
#include "Schemas/Mongo/Mongo.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

class TemplateService {
  public:
    TemplateService(const EventsManager &events);
    ~TemplateService() = default;

  protected:
    grpc::Status _procedureRunner(
        std::function<grpc::Status(FilesSchemas &&, StatsSchemas &&)> callback,
        const std::source_location &location = std::source_location::current()
    );
    grpc::Status _procedureRunner(
        std::function<grpc::Status(FilesSchemas &&, StatsSchemas &&)> callback, const string &functionName,
        const std::source_location &location = std::source_location::current()
    );

  private:
    grpc::Status
    _manageErrors(const RequestFailureException &error, const std::source_location &location = std::source_location::current());
    grpc::Status
    _manageErrors(const mongocxx::query_exception &error, const std::source_location &location = std::source_location::current());
    grpc::Status
    _manageErrors(const std::out_of_range &error, const std::source_location &location = std::source_location::current());
    grpc::Status
    _manageErrors(const std::invalid_argument &error, const std::source_location &location = std::source_location::current());
    grpc::Status
    _manageErrors(const std::exception &error, const std::source_location &location = std::source_location::current());

    NODISCARD string _getErrorCaller(const std::source_location &location);

    const EventsManager &_events;
};

#endif