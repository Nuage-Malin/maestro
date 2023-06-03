/**
 * @file DownloadFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "DownloadFilesCron.hpp"

DownloadFilesCron::DownloadFilesCron(FilesSchemas &filesSchemas, GrpcClients &clients, EventsManager &events)
    : TemplateCron("DownloadFiles"), _filesSchemas(filesSchemas), _clients(clients)
{
    const std::function<void(const string &)> &callback =
        std::bind(&DownloadFilesCron::_downloadDiskFiles, this, std::placeholders::_1);

    events.on<const string &>(Event::DISK_STARTUP, std::move(callback));
}

void DownloadFilesCron::run()
{
    std::unordered_set<string> disks = this->_filesSchemas.downloadQueue.getFilesDisk();

    for (const auto &disk : disks)
        this->_clients.bugle.setDiskState(disk, true);
}

void DownloadFilesCron::_downloadDiskFiles(const string &diskId)
{
    const Maestro_Vault::DownloadFilesRequest &request = this->_filesSchemas.downloadQueue.getDiskFiles(diskId);
    const Maestro_Vault::DownloadFilesStatus &files = this->_clients.vault.downloadFiles(request);

    for (const auto &file : files.files())
        this->_filesSchemas.downloadedStack.pushFile(file.fileid(), Date() + std::chrono::days(2), file.content());

    this->_filesSchemas.downloadQueue.deleteDiskFiles(diskId);
}