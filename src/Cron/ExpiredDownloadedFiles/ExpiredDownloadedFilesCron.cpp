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
}

void ExpiredDownloadedFilesCron::run()
{
    this->removeExpiredDownloadedFiles();
}

void ExpiredDownloadedFilesCron::removeExpiredDownloadedFiles()
{
    std::vector<DownloadedStack> filesToRemove =
        MongoCXX::Mongo(this->_events).getFilesSchemas().downloadedStack.getExpiredFiles();

    if (filesToRemove.begin() == filesToRemove.end())
        throw std::logic_error("Calling remove expired files with no files to remove, in function " + STR_FUNCTION);
    Maestro_Vault::RemoveFilesRequest request{};

    for (const DownloadedStack &file : filesToRemove)
        request.add_fileids(file.fileId);
    Maestro_Vault::RemoveFilesStatus response = this->_clients.vaultcache.removeFiles(request);

    for (const DownloadedStack &file : filesToRemove) {
        try {
            for (const string &failureFileId : response.fileidfailures())
                if (file.fileId == failureFileId)
                    throw std::runtime_error(STR_FUNCTION + ": [Failed to remove file " + file.fileId + " from vaultcache]");

            this->_events.emit<const string &, const Date &>(
                Event::DOWNLOADEDSTACK_FILE_EXPIRATION, file.fileId, file.expirationDate
            );
        } catch (const std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
        }
    }
}