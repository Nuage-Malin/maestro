/**
 * @file RemoveFilesCron.cpp
 * @author Arthur Jourdan (arthur.jourdan@epitech.eu)
 * @date 09/05/2023
 * @copyright Nuage Malin
 */

#include "Schemas/Mongo/Mongo.hpp"

#include "RemoveFilesCron.hpp"

RemoveFilesCron::RemoveFilesCron(GrpcClients &clients, EventsManager &events)
    : TemplateCron("RemoveFiles"), _events(events), _clients(clients)
{
    const std::function<void(const string &)> &callback =
        std::bind(&RemoveFilesCron::_removeDiskFiles, this, std::placeholders::_1);

    events.on<const string &>(Event::DISK_STARTUP, std::move(callback));
}

void RemoveFilesCron::run()
{
    std::unordered_set<string> disks = MongoCXX::Mongo(this->_events).getFilesSchemas().removeQueue.getFilesDisk();

    for (const auto &disk : disks)
        this->_clients.bugle.setDiskState(disk, true);
}

void RemoveFilesCron::_removeDiskFiles(const string &diskId)
{
    FilesSchemas filesSchemas = MongoCXX::Mongo(this->_events).getFilesSchemas();
    const Maestro_Vault::RemoveFilesRequest &request = filesSchemas.removeQueue.getDiskFiles(diskId);
    const Maestro_Vault::RemoveFilesStatus &response = this->_clients.vault.removeFiles(request);
    std::unordered_set<string> fileRemoved;
    string fileId;

    filesSchemas.removeQueue.deleteDiskFiles(diskId);
    filesSchemas.removeQueue.add(diskId, response.fileidfailures().begin(), response.fileidfailures().end());
    for (const auto &fileIdFailure : response.fileidfailures()) {
        for (auto i = 0; i <= request.fileid_size(); i++) {
            fileId = request.fileid(i);
            if (i == request.fileid_size()) {
                if (!fileRemoved.contains(fileId)) {
                    fileRemoved.insert(fileId);
                }
                break;
            }
            if (fileIdFailure == fileId) {
                break;
            }
        }
    }
    this->_clients.santaclaus.physicalRemoveFiles(fileRemoved.begin(), fileRemoved.end());
}