/**
 * @file UploadQueueSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_FILES_UPLOADQUEUE_SCHEMA_HPP
#define MAESTRO_FILES_UPLOADQUEUE_SCHEMA_HPP

#include <mongocxx/database.hpp>

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "Schemas/Templates/FileBucket/TemplateFileBucket.hpp"

class UploadQueueSchema : public TemplateFileBucket {
  public:
    UploadQueueSchema(const mongocxx::database &database);
    ~UploadQueueSchema() = default;

    void uploadFile(const string &fileId, const string &userId, const string &diskId, const string &content);
    Maestro_Vault::UploadFilesRequest getDiskFiles(const string &diskId);
    std::unordered_set<string> getFilesDisk();
};

#endif