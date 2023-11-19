/**
 * @file DownloadQueueSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 12/04/2023
 * @copyright Nuage Malin
 * @brief Stores (temporarly) file metadata of files to be made available for downlaod
 */

#ifndef MAESTRO_FILES_DOWNLOADQUEUE_SCHEMA_HPP
#define MAESTRO_FILES_DOWNLOADQUEUE_SCHEMA_HPP

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "mongocxx.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

class FilesDownloadQueueSchema : public TemplateSchema {
  public:
    FilesDownloadQueueSchema(const mongocxx::database &database);
    ~FilesDownloadQueueSchema() = default;

    void add(const string &fileId, const string &userId, const string &diskId);
    void deleteDiskFiles(const string &diskId);
    void deleteFile(const string &fileId);
    void deleteUser(const string &userId);
    Date getRequestedDate(const string &fileId, const string &diskId);
    NODISCARD Maestro_Vault::DownloadFilesRequest getDiskFiles(const string &diskId);
    NODISCARD std::unordered_set<string> getFilesDisk();
    NODISCARD bool doesFileExist(const string &fileId);
};

#endif