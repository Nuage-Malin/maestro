/**
 * @file Mongo.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 10/04/2023
 * @copyright Nuage Malin
 */

#include <mongocxx/instance.hpp>

#include "Mongo.hpp"
#include "utils.hpp"

static const string fileDb{"maestro"};
static const string statsDb{"logs"};

MongoCXX::Mongo::Mongo(const EventsManager &events) : _events(events)
{
    mongocxx::instance inst{}; // This should be done only once.

    this->_client = mongocxx::uri{getEnv("MAESTRO_MONGO_URL")};
    if (!this->_client)
        throw std::runtime_error("Could not access mongo database");
    if (!this->_client[fileDb])
        throw std::runtime_error("Could not access '" + fileDb + "' database");
    if (!this->_client[statsDb])
        throw std::runtime_error("Could not access '" + statsDb + "' database");
}

FilesSchemas MongoCXX::Mongo::getFilesSchemas() const
{
    const mongocxx::database filesDatabase = this->_client[fileDb];

    return {
        .uploadQueue = FilesUploadQueueSchema(filesDatabase),
        .downloadedStack = FilesDownloadedStackSchema(filesDatabase, this->_events),
        .downloadQueue = FilesDownloadQueueSchema(filesDatabase),
        .removeQueue = FilesRemoveQueueSchema(filesDatabase)};
}

StatsSchemas MongoCXX::Mongo::getStatsSchemas() const
{
    const mongocxx::database statsDatabase = this->_client[statsDb];

    return {.userDiskInfo = StatsUserDiskInfoSchema(statsDatabase)};
}