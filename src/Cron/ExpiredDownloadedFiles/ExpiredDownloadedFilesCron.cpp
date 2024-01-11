/**
 * @file ExpiredDownloadedFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "ExpiredDownloadedFilesCron.hpp"

ExpiredDownloadedFilesCron::ExpiredDownloadedFilesCron(GrpcClients &grpcClient, EventsManager &events)
    : TemplateCron("ExpiredDownloadedFiles"), _clients(grpcClient), _events(events)
{
    MongoCXX::Mongo mongo(this->_events);
    const std::function<void(const string &)> &callback =
        std::bind(&ExpiredDownloadedFilesCron::_onFileRemoved, this, std::placeholders::_1);

    events.on<const string &>(Event::REMOVE_VAULT_CACHE_FILE, std::move(callback));
}

void ExpiredDownloadedFilesCron::run()
{
    this->_removeExpiredDownloadedFiles();
}

void ExpiredDownloadedFilesCron::_removeExpiredDownloadedFiles()
{
    std::vector<DownloadedStack> filesToRemove =
        MongoCXX::Mongo(this->_events).getFilesSchemas().downloadedStack.getExpiredFiles();

    if (filesToRemove.begin() == filesToRemove.end())
        throw std::logic_error("Calling remove expired files with no files to remove, in function " + STR_FUNCTION);
    Maestro_Vault::RemoveFilesRequest request{};

    for (const DownloadedStack &file : filesToRemove)
        request.add_fileids(file.fileId);
    this->_clients.vaultcache.removeFiles(request);
}

void ExpiredDownloadedFilesCron::_onFileRemoved(const string &fileId)
{
    FilesSchemas &filesSchemas = MongoCXX::Mongo(this->_events).getFilesSchemas();

    filesSchemas.downloadedStack.deleteFile(fileId);
}