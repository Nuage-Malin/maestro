/**
 * @file DownloadedStackSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 * @brief Keep track of expiration date for downloadable files
 *        (in other words, knows until what datetime files are downloadable)
 */

#ifndef MAESTRO_FILES_DOWNLOADEDSTACK_SCHEMA_HPP
#define MAESTRO_FILES_DOWNLOADEDSTACK_SCHEMA_HPP

#include <tuple>

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "mongocxx.hpp"
#include "EventsManager.hpp"
#include "Utils/Date/Date.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

struct DownloadedStack
{
    string fileId;
    Date expirationDate;
};

class FilesDownloadedStackSchema : public TemplateSchema {
  public:
    FilesDownloadedStackSchema(const mongocxx::database &database, EventsManager &events);
    ~FilesDownloadedStackSchema() = default;

    void add(const string &fileId, const Date &expirationDate);
    void deleteFile(const string &fileId);
    NODISCARD std::vector<DownloadedStack> getExpiredFiles(const Date &expirationDate = Date());
    NODISCARD bool doesFileExist(const string &fileId);

  private:
    void _onFileExpiration(const string &fileId);
};

#endif