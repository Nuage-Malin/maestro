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
#include "Services/Template/TemplateService.hpp"
#include "Schemas/Stats/UserDiskInfo/StatsUserDiskInfoSchema.hpp"

class UsersBackService : public TemplateService, public UsersBack_Maestro::UsersBack_Maestro_Service::Service {
  public:
    UsersBackService(const mongocxx::database &fileDatabase, const mongocxx::database &statsDatabase);
    ~UsersBackService() = default;

    grpc::Status getUserConsumption(
        grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
        UsersBack_Maestro::GetUserConsumptionStatus *response
    ) override;

  private:
    /**
     * @brief Set file bucket with if _fileDatabase is settled. Otherwise throw an error.
     *
     * @throw std::logic_error if _fileDatabase is not settled
     */
    // void _setFileBucket();
    // NODISCARD int _isDownloadable(const string &fileId);

  private:
    // mongocxx::gridfs::bucket _fileBucket;

    StatsUserDiskInfoSchema _statsUserDiskInfoSchema;
};

#endif /* MAESTRO_FILESERVER_HPP */