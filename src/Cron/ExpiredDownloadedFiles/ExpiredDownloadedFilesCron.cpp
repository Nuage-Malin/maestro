/**
 * @file ExpiredDownloadedFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "ExpiredDownloadedFilesCron.hpp"

ExpiredDownloadedFilesCron::ExpiredDownloadedFilesCron(GrpcClients &grpcClient, const EventsManager &events)
    : TemplateCron("ExpiredDownloadedFiles"), _clients(grpcClient), _events(events)
{
}

void ExpiredDownloadedFilesCron::run()
{
    this->removeExpiredDownloadedFiles();
}

// template <typename StrIterator>
//     requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
//  todo template
void ExpiredDownloadedFilesCron::removeExpiredDownloadedFiles()
{
    //    std::tuple<StrIterator, StrIterator> fileIdsToRemove =
    std::vector<string> fileIdsToRemove = MongoCXX::Mongo(this->_events).getFilesSchemas().downloadedStack.deleteExpiredFiles();

    if (fileIdsToRemove.begin() == fileIdsToRemove.end())
        throw std::logic_error("Calling remove expired files with no files to remove, in function " + STR_FUNCTION);
    Maestro_Vault::RemoveFileRequest req{};

    for (auto fileId : fileIdsToRemove) {
        req.set_fileid(fileId);
        this->_clients.vaultcache.removeFile(req);
    }
}