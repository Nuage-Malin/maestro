/**
 * @file VaultCacheClient.hpp
 * @author Arthur Jourdan
 * @date of creation 30/09/23.
 * @copyright Nuage Malin
 * @brief Description of client to request vault cache server, in other words, request temporary file storage
 */

#ifndef MAESTRO_VAULTCACHE_CLIENT_HPP
#define MAESTRO_VAULTCACHE_CLIENT_HPP

#include <optional>
#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "utils.hpp"

class VaultCacheClient {
  public:
    VaultCacheClient(const std::shared_ptr<grpc::ChannelInterface> &channel);
    ~VaultCacheClient() = default;

    void uploadFile(
        const string &fileId, const string &userId, const string &diskId, const string &content,
        const Maestro_Vault::storage_type &store_type
    ) const;
    /// Upload
    void uploadFiles(const Maestro_Vault::UploadFilesRequest &files) const;

    /// Download
    string downloadFile(const string &fileId) const;
    Maestro_Vault::DownloadFilesStatus downloadFiles(const Maestro_Vault::DownloadFilesRequest &files) const;
    NODISCARD Maestro_Vault::DownloadFilesStatus downloadStorageTypeFiles(const Maestro_Vault::storage_type &store_type) const;

    /// Remove
    Maestro_Vault::RemoveFileStatus removeFile(const string &fileId) const;
    Maestro_Vault::RemoveFilesStatus removeFiles(const Maestro_Vault::RemoveFilesRequest &files) const;

    /// Get
    NODISCARD Maestro_Vault::GetFileMetaInfoStatus getFileMetaInfo(const string &fileId) const;
    NODISCARD Maestro_Vault::GetFilesMetaInfoStatus getFilesMetaInfo(
        std::optional<string> userId = std::nullopt, std::optional<string> diskId = std::nullopt,
        std::optional<Maestro_Vault::storage_type> store_type = std::nullopt
    ) const;
    NODISCARD Maestro_Vault::GetFilesDisksStatus getFilesDisks(const Maestro_Vault::GetFilesDisksRequest &files) const;

  private:
    void _callLogger(const string &functionName) const;

  private:
    std::unique_ptr<Maestro_Vault::Maestro_Vault_Service::Stub> _stub;
};

#endif // MAESTRO_VAULTCACHE_CLIENT_HPP