/**
 * @file UsersBackService.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 17/02/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_USERSBACK_SERVICE_HPP
#define MAESTRO_USERSBACK_SERVICE_HPP

#include <utility>
#include <mongocxx/database.hpp>

#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"
#include "clients.hpp"
#include "schemas.hpp"
#include "Services/Template/TemplateService.hpp"
#include "Schemas/Stats/UserDiskInfo/StatsUserDiskInfoSchema.hpp"

class UsersBackService : public TemplateService, public UsersBack_Maestro::UsersBack_Maestro_Service::Service {
  public:
    UsersBackService(const mongocxx::database &fileDatabase, const mongocxx::database &statsDatabase, const GrpcClients &clients);
    ~UsersBackService() = default;

    grpc::Status fileUpload(
        grpc::ServerContext *context, const UsersBack_Maestro::FileUploadRequest *request,
        UsersBack_Maestro::FileUploadStatus *response
    ) override;

    grpc::Status getUserConsumption(
        grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
        UsersBack_Maestro::GetUserConsumptionStatus *response
    ) override;

    grpc::Status getUserDiskSpace(
        grpc::ServerContext *context, const UsersBack_Maestro::GetUserDiskSpaceRequest *request,
        UsersBack_Maestro::GetUserDiskSpaceStatus *response
    ) override;

  private:
    FilesSchemas _filesSchemas;
    StatsUserDiskInfoSchema _statsUserDiskInfoSchema;
    const GrpcClients &_clients;
};

#endif