/**
 * @file UsersBackService.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "UsersBackService.hpp"

UsersBackService::UsersBackService(UNUSED const mongocxx::database &fileDatabase, const mongocxx::database &statsDatabase)
    : _statsUserDiskInfoSchema(StatsUserDiskInfoSchema(statsDatabase))
{
}

grpc::Status UsersBackService::getUserConsumption(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
    UsersBack_Maestro::GetUserConsumptionStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        const uint64 consumption = this->_statsUserDiskInfoSchema.getUserConsumption(
            request->userid(), Date(request->startdate()), Date(request->enddate())
        );

        response->set_consumption(consumption);
        return grpc::Status::OK;
    });
}