/**
 * @file FileUploadCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#include "FileUploadCron.hpp"
#include "Cron/Manager/ManagerCron.hpp"

FileUploadCron::FileUploadCron(FilesSchemas &filesSchemas, GrpcClients &grpcClient, EventsManager &events)
    : TemplateCron("FileUpload"), _filesSchemas(filesSchemas), _clients(grpcClient)
{
    const std::function<void(const string &)> &callback = std::bind(&FileUploadCron::_uploadFiles, this, std::placeholders::_1);

    events.on<const string &>(Event::DISK_STARTUP, std::move(callback));
}

void FileUploadCron::run()
{
    std::unordered_set<string> disks = this->_filesSchemas.uploadQueue.getFilesDisk();

    for (const auto &disk : disks)
        this->_clients.hardwareMalin.setDiskState(disk, true);
}

void FileUploadCron::_uploadFiles(const string &diskId)
{
    const Maestro_Vault::UploadFilesRequest &files = this->_filesSchemas.uploadQueue.getDiskFiles(diskId);

    this->_clients.vault.uploadFiles(files);
}