/**
 * @file TemplateService.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include <sstream>

#include "TemplateService.hpp"

grpc::Status TemplateService::_procedureRunner(std::function<grpc::Status()> callback)
{
    try {
        return callback();
    } catch (const mongocxx::query_exception &error) {
        return _manageErrors(error);
    } catch (const std::out_of_range &error) {
        return _manageErrors(error);
    } catch (const std::invalid_argument &error) {
        return _manageErrors(error);
    } catch (const std::exception &error) {
        return _manageErrors(error);
    }
}

grpc::Status TemplateService::_manageErrors(const mongocxx::query_exception &error, const std::source_location &location)
{
    std::cerr << "[" << this->_getCaller(location) << "] mongocxx::query_exception: " << error.what() << std::endl;

    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Query exception", error.what());
}

grpc::Status TemplateService::_manageErrors(const std::out_of_range &error, const std::source_location &location)
{
    std::cerr << "[" << this->_getCaller(location) << "] std::out_of_range: " << error.what() << std::endl;

    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Out of range arguments", error.what());
}

grpc::Status TemplateService::_manageErrors(const std::invalid_argument &error, const std::source_location &location)
{
    std::cerr << "[" << this->_getCaller(location) << "] std::invalid_argument: " << error.what() << std::endl;

    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument", error.what());
}

grpc::Status TemplateService::_manageErrors(const std::exception &error, const std::source_location &location)
{
    std::cerr << "[" << this->_getCaller(location) << "] std::exception: " << error.what() << std::endl;

    return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error", error.what());
}

string TemplateService::_getCaller(const std::source_location &location)
{
    std::ostringstream message;

    message << location.file_name() << ":" << location.line() << ":" << location.column() << " => " << location.function_name();
    return message.str();
}
