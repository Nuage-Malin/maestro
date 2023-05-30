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
        const uint64 &diskSpace = this->_statsSchemas.userDiskInfo.getUserDiskSpace(
            request->userid(), request->has_date() ? Date(request->date()) : Date()
        );

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

        response->set_content(this->_filesSchemas.downloadedStack.downloadFile(request->fileid()));
        response->set_allocated_metadata(new File::FileMetadata(metadata));

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

grpc::Status UsersBackService::
    fileMove(UNUSED grpc::ServerContext *, const ::UsersBack_Maestro::FileMoveRequest *request, UNUSED UsersBack_Maestro::FileMoveStatus *)
{
    return this->_procedureRunner([this, request]() {
        /* auto santaclausResponse = */ this->_clients.santaclaus.moveFile(
            request->fileid(),
            request->has_newfilename() ? std::optional(request->newfilename()) : std::nullopt,
            request->has_dirid() ? std::optional(request->dirid()) : std::nullopt
        );

        return grpc::Status::OK;
    });
}

// todo test
grpc::Status UsersBackService::
    fileRemove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::FileRemoveRequest *request, UNUSED UsersBack_Maestro::FileRemoveStatus *)
{
    return this->_procedureRunner([this, request]() {
        auto file = this->_clients.santaclaus.getFile(request->fileid()); // get disk id from santaclaus

        this->_clients.santaclaus.virtualRemoveFile(request->fileid());
        if (!this->_clients.hardwareMalin.diskStatus(file.diskid())) { // check if disk is turned off
            this->_filesSchemas.removeQueue.add(file.diskid(), request->fileid());
        } else {                                                       // if disk is turned on
            Maestro_Vault::RemoveFileRequest my_request;

            my_request.set_diskid(file.diskid());
            my_request.set_fileid(request->fileid());
            try {
                const auto my_response = this->_clients.vault.removeFile(my_request);

                this->_clients.santaclaus.physicalRemoveFile(request->fileid());
            } catch (const RequestFailureException &e) { // if deletion in vault didn't succeed
                this->_filesSchemas.removeQueue.add(file.diskid(), request->fileid());
            }
        }
        return grpc::Status::OK;
    });
}

// todo test
grpc::Status UsersBackService::filesRemove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::FilesRemoveRequest *request,
    UsersBack_Maestro::FilesRemoveStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        *response = this->actFilesRemove(request->fileid().begin(), request->fileid().end());

        return grpc::Status::OK;
    });
}

grpc::Status UsersBackService::
    dirRemove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirRemoveRequest *request, UNUSED UsersBack_Maestro::DirRemoveStatus *)
{
    return this->_procedureRunner([this, request]() {
        auto my_response = this->_clients.santaclaus.removeDirectory(request->dirid());

        this->actFilesRemove(my_response.fileidstoremove().begin(), my_response.fileidstoremove().end());

        return grpc::Status::OK;
    });
}
grpc::Status UsersBackService::
    dirMove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirMoveRequest *request, UNUSED UsersBack_Maestro::DirMoveStatus *)
{
    return this->_procedureRunner([this, request]() {
        this->_clients.santaclaus.moveDirectory(
            request->dirid(),
            request->has_name() ? std::optional(request->name()) : std::nullopt,
            request->has_newlocationdirid() ? std::optional(request->newlocationdirid()) : std::nullopt
        );

        return grpc::Status::OK;
    });
}