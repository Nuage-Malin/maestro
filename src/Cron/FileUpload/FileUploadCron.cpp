/**
 * @file FileUploadCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include <optional>
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
    //    std::unordered_set<string> disks = MongoCXX::Mongo(this->_events).getFilesSchemas().uploadQueue.getFilesDisk();
    Maestro_Vault::GetFilesMetaInfoStatus response_files =
        _clients.vaultcache.getFilesMetaInfo(std::nullopt, std::nullopt, Maestro_Vault::storage_type::UPLOAD_QUEUE);
    Maestro_Vault::GetFilesDisksRequest request;

    for (auto file : response_files.files()) {
        request.add_file_ids(file.fileid());
    }
    Maestro_Vault::GetFilesDisksStatus response_disks = _clients.vaultcache.getFilesDisks(request);

    for (const auto &disk_id : response_disks.disk_ids()) {
        this->_clients.bugle.setDiskState(disk_id, true);
    }
}

void FileUploadCron::_uploadFiles(const string &diskId)
{
    Maestro_Vault::GetFilesMetaInfoStatus response_files =
        _clients.vaultcache.getFilesMetaInfo(std::nullopt, diskId, Maestro_Vault::storage_type::UPLOAD_QUEUE);
    string content_to_transfer;

    for (auto file : response_files.files()) {
        // do it one by one because of max size of a request being 2 gigabytes
        content_to_transfer = _clients.vaultcache.downloadFile(file.fileid(), file.userid(), file.diskid());
        _clients.vault.uploadFile(file.fileid(), file.userid(), file.diskid(), content_to_transfer);
    }
}