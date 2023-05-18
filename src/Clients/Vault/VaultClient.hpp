/**
 * @file VaultClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_VAULT_CLIENT_HPP
#define MAESTRO_VAULT_CLIENT_HPP

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "utils.hpp"

class VaultClient {
  public:
    VaultClient(const std::shared_ptr<grpc::ChannelInterface> &channel);
    ~VaultClient() = default;

    void uploadFile(const string &fileId, const string &userId, const string &diskId, const string &content) const;
    void uploadFiles(const Maestro_Vault::UploadFilesRequest &files) const;
    string downloadFile(const string &fileId, const string &userId, const string &diskId) const;
    Maestro_Vault::DownloadFilesStatus downloadFiles(const Maestro_Vault::DownloadFilesRequest &files) const;
    Maestro_Vault::RemoveFileStatus removeFile(const Maestro_Vault::RemoveFileRequest &file) const;
    Maestro_Vault::RemoveFilesStatus removeFiles(const Maestro_Vault::RemoveFilesRequest &files) const;

  private:
    std::unique_ptr<Maestro_Vault::Maestro_Vault_Service::Stub> _stub;
};

#endif