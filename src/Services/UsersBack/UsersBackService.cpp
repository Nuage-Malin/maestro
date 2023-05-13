/**
 * @file UsersBackService.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "schemas.hpp"
#include "UsersBackService.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"
#include "Schemas/Files/DownloadedStack/DownloadedStackSchema.hpp"

UsersBackService::UsersBackService(FilesSchemas &filesSchemas, StatsSchemas &statsSchemas, const GrpcClients &clients)
    : _filesSchemas(filesSchemas), _statsSchemas(statsSchemas), _clients(clients)
{
}

grpc::Status UsersBackService::fileUpload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileUploadRequest *request,
    UsersBack_Maestro::FileUploadStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        // Index file into Santaclaus
        Maestro_Santaclaus::AddFileStatus addFileStatus =
            this->_clients.santaclaus.addFile(request->file().metadata(), request->file().content().size());

        response->set_fileid(addFileStatus.fileid());
        // Ask if the disk is online
        if (this->_clients.hardwareMalin.diskStatus(addFileStatus.diskid())) {
            try {
                // If the disk is online, upload the file on the filesystem
                this->_clients.vault.uploadFile(
                    addFileStatus.fileid(), request->file().metadata().userid(), addFileStatus.diskid(), request->file().content()
                );
            } catch (const RequestFailureException &error) {
                // If the upload failed, add it to the queue database
                this->_filesSchemas.uploadQueue.uploadFile(
                    addFileStatus.fileid(), request->file().metadata().userid(), addFileStatus.diskid(), request->file().content()
                );
            }
        } else {
            // If the disk is offline, add it to the queue database
            this->_filesSchemas.uploadQueue.uploadFile(
                addFileStatus.fileid(), request->file().metadata().userid(), addFileStatus.diskid(), request->file().content()
            );
        }
        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::getUserConsumption(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
    UsersBack_Maestro::GetUserConsumptionStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        const uint64 consumption = this->_statsSchemas.userDiskInfo.getUserConsumption(
            request->userid(), Date(request->startdate()), Date(request->enddate())
        );

        response->set_consumption(consumption);
        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::getUserDiskSpace(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetUserDiskSpaceRequest *request,
    UsersBack_Maestro::GetUserDiskSpaceStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        const uint64 &diskSpace = this->_statsSchemas.userDiskInfo.getUserDiskSpace(request->userid(), Date(request->date()));

        response->set_useddiskspace(diskSpace);
        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::askFileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::AskFileDownloadRequest *request,
    UsersBack_Maestro::AskFileDownloadStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        // Check if the file is already downloaded
        if (this->_filesSchemas.downloadedStack.doesFileExist(request->fileid())) {
            response->set_allocated_waitingtime(new google::protobuf::Duration());
            return grpc::Status::OK;
        }

        const Maestro_Santaclaus::GetFileStatus file = this->_clients.santaclaus.getFile(request->fileid());
        const std::chrono::days expirationLimit(2);

        try {
            // Check if the file is already in the database
            const Date &requestedDate = this->_filesSchemas.downloadQueue.getRequestedDate(request->fileid(), file.diskid());

            response->set_allocated_waitingtime((requestedDate + expirationLimit).toAllocatedDuration());
            return grpc::Status::OK;
        } catch (const NotFoundException &error) {
            const Date &expirationDate = Date() + expirationLimit;

            if (this->_clients.hardwareMalin.diskStatus(file.diskid())) {
                // If the disk is online, download the file from the filesystem and upload it to the database
                const string fileContent =
                    this->_clients.vault.downloadFile(request->fileid(), file.file().approxmetadata().userid(), file.diskid());

                this->_filesSchemas.downloadedStack.pushFile(request->fileid(), expirationDate, fileContent);
                response->set_allocated_waitingtime(expirationDate.toAllocatedDuration()); // TODO: Edit waiting time
            } else {
                // If the disk is offline, add the file to the queue database
                this->_filesSchemas.downloadQueue.add(request->fileid(), file.file().approxmetadata().userid(), file.diskid());

                response->set_allocated_waitingtime(expirationDate.toAllocatedDuration());
            }
            return grpc::Status::OK;
        }
    });
}

grpc::Status UsersBackService::fileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileDownloadRequest *request, File::File *response
)
{
    return this->_procedureRunner([this, request, response]() {
        File::FileMetadata metadata = this->_clients.santaclaus.getFile(request->fileid()).file();

        metadata.set_isdownloadable(true);

        File::File file;
        file.set_content(this->_filesSchemas.downloadedStack.downloadFile(request->fileid()));
        file.set_allocated_metadata(new File::FileMetadata(metadata));

        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::getFilesIndex(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetFilesIndexRequest *request,
    UsersBack_Maestro::GetFilesIndexStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        Maestro_Santaclaus::GetDirectoryStatus subFiles = this->_clients.santaclaus.getDirectory(
            request->userid(), request->has_dirid() ? std::optional(request->dirid()) : std::nullopt, request->isrecursive()
        );
        File::FilesIndex filesIndex;

        filesIndex.CopyFrom(subFiles.subfiles());
        filesIndex.clear_fileindex();
        for (const File::FileMetadata &file : subFiles.subfiles().fileindex()) {
            File::FileMetadata *fileIndex = filesIndex.add_fileindex();

            fileIndex->CopyFrom(file);
            fileIndex->set_isdownloadable(this->_filesSchemas.downloadedStack.doesFileExist(file.fileid()));
        }
        response->set_allocated_subfiles(new File::FilesIndex(filesIndex));
        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::fileMove(
    UNUSED grpc::ServerContext *, const ::UsersBack_Maestro::FileMoveRequest *request,
    ::UsersBack_Maestro::FileMoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        //        Maestro_Santaclaus::MoveFileRequest santaclausRequest;

        //        santaclausRequest.set_fileid(request->fileid());
        //        if (request->has_dirid())
        //            santaclausRequest.set_dirid(request->dirid());
        //        if (request->has_newfilename())
        //            santaclausRequest.set_name(request->newfilename());

        /* auto santaclausResponse = */ this->_clients.santaclaus.moveFile(
            request->fileid(),
            request->has_newfilename() ? std::optional(request->newfilename()) : std::nullopt,
            request->has_dirid() ? std::optional(request->dirid()) : std::nullopt
        );

        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::fileRemove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::FileRemoveRequest *request,
    UsersBack_Maestro::FileRemoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        auto file = this->_clients.santaclaus.getFile(request->fileid()); // get disk id from santaclaus

        this->_clients.santaclaus.virtualRemoveFile(request->fileid());
        if (this->_clients.hardwareMalin.diskStatus(file.diskid())) { // check if disk is turned on
            Maestro_Vault::RemoveFileRequest my_request;

            my_request.set_diskid(file.diskid());
            my_request.set_fileid(request->fileid());
            try {
                const auto my_response = this->_clients.vault.removeFile(my_request);

                this->_clients.santaclaus.physicalRemoveFile(request->fileid());
            } catch (const RequestFailureException &e) { // if deletion in vault didn't succeed
                this->_filesSchemas.removeQueue.add(file.diskid(), request->fileid());
            }
        } else { // if disk is turned off
            this->_filesSchemas.removeQueue.add(file.diskid(), request->fileid());
        }
        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::filesRemove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::FilesRemoveRequest *request,
    UsersBack_Maestro::FilesRemoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        std::unordered_map<string, std::unordered_set<string>> disksFiles;
        // unordered_map with string as key, for diskId, unordered_set as value, to store each fileId corresponding to this diskId
        string diskId;

        for (const auto &fileId : request->fileid()) {
            diskId = this->_clients.santaclaus.getFile(fileId).diskid();
            disksFiles.find(diskId);
            if (auto files = disksFiles.find(diskId); files != disksFiles.end()) {
                files->second.insert(fileId); // todo is this iterator affecting the disksFiles (is it a reference) ?
            } else {
                std::unordered_set<string> newFiles;

                newFiles.insert(fileId);
                disksFiles.insert(std::make_pair(diskId, newFiles));
            }
        }

        this->_clients.santaclaus.virtualRemoveFiles(request->fileid().begin(), request->fileid().end());
        for (const auto &diskFiles : disksFiles) {
            if (this->_clients.hardwareMalin.diskStatus(diskId)) { // check if disk is turned on
                Maestro_Vault::RemoveFilesRequest my_request;
                uint counter = 0;

                my_request.set_diskid(diskId);
                for (const auto &diskFile : diskFiles.second) {
                    my_request.set_fileid(counter++, diskFile);
                }
                const auto my_response = this->_clients.vault.removeFiles(my_request);
                try {
                    this->_clients.santaclaus.physicalRemoveFiles(diskFiles.second.begin(), diskFiles.second.end());
                } catch (const RequestFailureException &e) { // if deletion in vault didn't succeed
                    this->_filesSchemas.removeQueue.add(diskId, diskFiles.second.begin(), diskFiles.second.end());
                }
            } else { // if disk is turned off
                this->_filesSchemas.removeQueue.add(diskId, diskFiles.second.begin(), diskFiles.second.end());
            }
        }

        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::dirRemove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirRemoveRequest *request, UsersBack_Maestro::DirRemoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        auto response = this->_clients.santaclaus.removeDirectory(request->dirid());

        for (auto fileIdToRm : response.fileidstoremove()) {
            this->_clients.santaclaus.virtualRemoveFile(fileIdToRm);
        }
        return grpc::Status::OK;
    });
}
grpc::Status UsersBackService::dirMove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirMoveRequest *request, UsersBack_Maestro::DirMoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        return grpc::Status::OK;
    });
}