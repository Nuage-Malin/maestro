/**
 * @file UsersBackClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 25/05/2023
 * @copyright Nuage Malin
 */

#ifndef TEST_USERSBACK_CLIENT_HPP
#define TEST_USERSBACK_CLIENT_HPP

#include <grpcpp/grpcpp.h>

#include <optional>

#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"

class UsersBackClient {
  public:
    UsersBackClient(const string &serverURI);
    ~UsersBackClient() = default;

    std::optional<UsersBack_Maestro::GetFilesIndexStatus> getFilesIndex(UsersBack_Maestro::GetFilesIndexRequest &request) const;
    std::optional<UsersBack_Maestro::FileUploadStatus> fileUpload(UsersBack_Maestro::FileUploadRequest &request) const;
    std::optional<UsersBack_Maestro::AskFileDownloadStatus> askFileDownload(UsersBack_Maestro::AskFileDownloadRequest &request
    ) const;

  private:
    void _manageStatusError(const string &funcName, const grpc::Status &status) const;

    std::unique_ptr<UsersBack_Maestro::UsersBack_Maestro_Service::Stub> _client;
};

#endif