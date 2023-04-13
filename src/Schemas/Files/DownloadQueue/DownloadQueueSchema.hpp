/**
 * @file DownloadQueueSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 12/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_FILES_DOWNLOADQUEUE_SCHEMA_HPP
#define MAESTRO_FILES_DOWNLOADQUEUE_SCHEMA_HPP

#include "mongocxx.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

class FilesDownloadQueueSchema : public TemplateSchema {
  public:
    FilesDownloadQueueSchema(const mongocxx::database &database);
    ~FilesDownloadQueueSchema() = default;

    void add(const string &fileId, const string &userId, const string &diskId);
    void remove(const MongoCXX::ObjectId &id);
    Date getRequestedDate(const string &fileId, const string &diskId);
};

#endif