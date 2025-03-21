/**
 * @file DownloadedStackSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 * @brief Stores files (temporarly) available for download
 */

#ifndef MAESTRO_FILES_DOWNLOADEDSTACK_SCHEMA_HPP
#define MAESTRO_FILES_DOWNLOADEDSTACK_SCHEMA_HPP

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "mongocxx.hpp"
#include "EventsManager.hpp"
#include "Utils/Date/Date.hpp"
#include "Schemas/Templates/FileBucket/TemplateFileBucket.hpp"

class FilesDownloadedStackSchema : public TemplateFileBucket {
  public:
    FilesDownloadedStackSchema(const mongocxx::database &database, const EventsManager &events);
    ~FilesDownloadedStackSchema() = default;

    void pushFile(const string &fileId, const Date &expirationDate, const string &content);
    NODISCARD string downloadFile(const string &fileId);
    void deleteExpiredFiles(const Date &expirationDate = Date());
    NODISCARD Date getFileExpirationDate(const string &fileId);
    NODISCARD bool doesFileExist(const string &fileId);

  private:
    const EventsManager &_events;
};

#endif