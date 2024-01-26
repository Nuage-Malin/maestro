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
    std::vector<std::pair<string, string>> requestedFiles = filesSchemas.downloadQueue.getDiskFiles(diskId);
    Maestro_Vault::DownloadFilesRequest vaultDownloadRequest;

    for (const std::pair<string, string> &file : requestedFiles) {
        Maestro_Vault::DownloadFileRequest *downloadFileRequest = vaultDownloadRequest.add_files();

        downloadFileRequest->set_fileid(file.first);
    }

    const Maestro_Vault::DownloadFilesStatus &files = this->_clients.vault.downloadFiles(vaultDownloadRequest);
    Maestro_Vault::UploadFilesRequest vaultCacheUploadRequest;

    for (const auto &file : requestedFiles) {
        Maestro_Vault::UploadFileRequest *uploadFileRequest = vaultCacheUploadRequest.add_files();

        uploadFileRequest->set_fileid(file.first);
        uploadFileRequest->set_userid(file.second);
        uploadFileRequest->set_diskid(diskId);
        for (const auto &vaultFile : files.files())
            if (vaultFile.fileid() == file.first) {
                uploadFileRequest->set_content(vaultFile.content());
                break;
            }
        uploadFileRequest->set_store_type(Maestro_Vault::storage_type::DOWNLOAD_QUEUE);

        filesSchemas.downloadedStack.add(file.first, Date() + std::chrono::days(1));
    }

    filesSchemas.downloadQueue.deleteDiskFiles(diskId);
}