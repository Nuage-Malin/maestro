/**
 * @file ExpiredDownloadedFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "ExpiredDownloadedFilesCron.hpp"

ExpiredDownloadedFilesCron::ExpiredDownloadedFilesCron(const EventsManager &events)
    : TemplateCron("ExpiredDownloadedFiles"), _events(events)
{
}

void ExpiredDownloadedFilesCron::run()
{
    MongoCXX::Mongo(this->_events).getFilesSchemas().downloadedStack.deleteExpiredFiles();
}