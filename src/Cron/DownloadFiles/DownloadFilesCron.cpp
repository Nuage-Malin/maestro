/**
 * @file DownloadFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "DownloadFilesCron.hpp"

DownloadFilesCron::DownloadFilesCron(GrpcClients &clients, EventsManager &events)
    : TemplateCron("DownloadFiles"), _events(events), _clients(clients)
{
    const std::function<void(const string &)> &callback =
        std::bind(&DownloadFilesCron::_downloadDiskFiles, this, std::placeholders::_1);

    events.on<const string &>(Event::DISK_STARTUP, std::move(callback));
}

void DownloadFilesCron::run()
{
    std::unordered_set<string> disks = MongoCXX::Mongo(this->_events).getFilesSchemas().downloadQueue.getFilesDisk();

    for (const auto &disk : disks)
        this->_clients.bugle.setDiskState(disk, true);
}

void DownloadFilesCron::_downloadDiskFiles(const string &diskId)
{
    FilesSchemas filesSchemas = MongoCXX::Mongo(this->_events).getFilesSchemas();
    const Maestro_Vault::DownloadFilesRequest &request = filesSchemas.downloadQueue.getDiskFiles(diskId);
    const Maestro_Vault::DownloadFilesStatus &files = this->_clients.vault.downloadFiles(request);

    for (const auto &file : files.files())
        filesSchemas.downloadedStack.pushFile(file.fileid(), Date() + std::chrono::days(2), file.content());

    filesSchemas.downloadQueue.deleteDiskFiles(diskId);
}