/**
 * @file FileUploadCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "FileUploadCron.hpp"
#include "Cron/Manager/ManagerCron.hpp"

FileUploadCron::FileUploadCron(GrpcClients &grpcClient, EventsManager &events)
    : TemplateCron("FileUpload"), _events(events), _clients(grpcClient)
{
    const std::function<void(const string &)> &callback = std::bind(&FileUploadCron::_uploadFiles, this, std::placeholders::_1);

    events.on<const string &>(Event::DISK_STARTUP, std::move(callback));
}

void FileUploadCron::run()
{
    std::unordered_set<string> disks = MongoCXX::Mongo(this->_events).getFilesSchemas().uploadQueue.getFilesDisk();

    for (const auto &disk : disks)
        this->_clients.bugle.setDiskState(disk, true);
}

void FileUploadCron::_uploadFiles(const string &diskId)
{
    FilesUploadQueueSchema uploadQueue = MongoCXX::Mongo(this->_events).getFilesSchemas().uploadQueue;
    const std::pair<std::vector<MongoCXX::ValueView>, Maestro_Vault::UploadFilesRequest> &files =
        uploadQueue.getDiskFiles(diskId);

    this->_clients.vault.uploadFiles(files.second);
    uploadQueue.deleteFiles(files.first);
}