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

UsersBackService::UsersBackService(const GrpcClients &clients, const EventsManager &events)
    : TemplateService(events), _clients(clients)
{
}

grpc::Status UsersBackService::fileUpload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileUploadRequest *request,
    UsersBack_Maestro::FileUploadStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            // Index file into Santaclaus
            Maestro_Santaclaus::AddFileStatus addFileStatus =
                this->_clients.santaclaus.addFile(request->file().metadata(), request->file().content().size());

            response->set_fileid(addFileStatus.fileid());
            // Ask if the disk is online
            try {
                if (this->_clients.bugle.diskStatus(addFileStatus.diskid())) {
                    try {
                        // If the disk is online, upload the file on the filesystem
                        this->_clients.vault.uploadFile(
                            addFileStatus.fileid(),
                            request->file().metadata().userid(),
                            addFileStatus.diskid(),
                            request->file().content()
                        );
                    } catch (const RequestFailureException &error) {
                        // If the upload failed, add it to the queue database
                        std::cerr << "[WARNING] Fail to upload file (" << request->file().metadata().name()
                                  << ") on vault, upload it on the DB : " << error.what() << std::endl;
                        this->_fileUploadFailure(filesSchemas, request->file(), addFileStatus);
                    }
                } else {
                    // If the disk is offline, add it to the queue database
                    this->_fileUploadFailure(filesSchemas, request->file(), addFileStatus);
                }
            } catch (const RequestFailureException &error) {
                // If fail to call bugle, upload it on the DB
                std::cerr << "[WARNING] Fail to get disk status (" << addFileStatus.diskid()
                          << "), upload file on the DB : " << error.what() << std::endl;
                this->_fileUploadFailure(filesSchemas, request->file(), addFileStatus);
            }
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::getUserConsumption(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
    UsersBack_Maestro::GetUserConsumptionStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&statsSchemas) {
            const uint64 consumption = statsSchemas.userDiskInfo.getUserConsumption(
                request->userid(),
                request->has_startdate() ? std::optional(Date(request->startdate())) : std::nullopt,
                request->has_enddate() ? Date(request->enddate()) : Date()
            );

            response->set_consumption(consumption);
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::getUserDiskSpace(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetUserDiskSpaceRequest *request,
    UsersBack_Maestro::GetUserDiskSpaceStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&statsSchemas) {
            const Date &date = request->has_date() ? Date(request->date()) : Date();
            const uint64 &totalDisksSpace = statsSchemas.diskInfo.getTotalDisksSpace(date);
            const uint64 &diskSpace = statsSchemas.userDiskInfo.getUserDiskSpace(
                request->userid(), date
            ) + filesSchemas.uploadQueue.getUserQueueSpace(request->userid(), date);

            response->set_totaldiskspace(totalDisksSpace);
            response->set_useddiskspace(diskSpace);
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::askFileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::AskFileDownloadRequest *request,
    UsersBack_Maestro::AskFileDownloadStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            // Check if the file is already downloaded
            if (filesSchemas.downloadedStack.doesFileExist(request->fileid())) {
                response->set_allocated_waitingtime(new google::protobuf::Duration());
                return grpc::Status::OK;
            }

            const std::chrono::days expirationLimit(2);
            const Date &expirationDate = Date() + expirationLimit;

            try {
                // Get file from UploadQueue if exist
                const string &uploadQueueFile = filesSchemas.uploadQueue.getFile(request->fileid());

                filesSchemas.downloadedStack.pushFile(request->fileid(), expirationDate, uploadQueueFile);
                response->set_allocated_waitingtime(new google::protobuf::Duration());
                return grpc::Status::OK;
            } catch (const NotFoundException &error) {
                const Maestro_Santaclaus::GetFileStatus file = this->_clients.santaclaus.getFile(request->fileid());

                try {
                    // Check if the file is already in the database
                    const Date &requestedDate = filesSchemas.downloadQueue.getRequestedDate(request->fileid(), file.diskid());

                    response->set_allocated_waitingtime((requestedDate + expirationLimit).toAllocatedDuration());
                    return grpc::Status::OK;
                } catch (const NotFoundException &error) {
                    try {
                        if (this->_clients.bugle.diskStatus(file.diskid())) {
                            // If the disk is online, download the file from the filesystem and upload it to the database
                            const string fileContent = this->_clients.vault.downloadFile(
                                request->fileid(), file.file().approxmetadata().userid(), file.diskid()
                            );

                            filesSchemas.downloadedStack.pushFile(request->fileid(), expirationDate, fileContent);
                            response->set_allocated_waitingtime(expirationDate.toAllocatedDuration()); // TODO: Edit waiting time
                        } else {
                            // If the disk is offline, add the file to the queue database
                            this->_askFileDownloadFailure(filesSchemas, request->fileid(), file, expirationDate, *response);
                        }
                    } catch (const RequestFailureException &error) {
                        // If fail to call bugle or vault, add the file to the queue database
                        std::cerr << "[WARNING] Fail to call bugle or vault, add the file to the queue DB : " << error.what()
                                  << std::endl;
                        this->_askFileDownloadFailure(filesSchemas, request->fileid(), file, expirationDate, *response);
                    }
                    return grpc::Status::OK;
                }
            }
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::fileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileDownloadRequest *request, File::File *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            File::FileMetadata metadata = this->_clients.santaclaus.getFile(request->fileid()).file();

            metadata.set_isdownloadable(true);

            response->set_content(filesSchemas.downloadedStack.downloadFile(request->fileid()));
            response->set_allocated_metadata(new File::FileMetadata(metadata));

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::getFilesIndex(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::GetFilesIndexRequest *request,
    UsersBack_Maestro::GetFilesIndexStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            Maestro_Santaclaus::GetDirectoryStatus subFiles = this->_clients.santaclaus.getDirectory(
                request->userid(), request->has_dirid() ? std::optional(request->dirid()) : std::nullopt, request->isrecursive()
            );
            File::FilesIndex filesIndex;

            filesIndex.CopyFrom(subFiles.subfiles());
            filesIndex.clear_fileindex();
            for (const File::FileMetadata &file : subFiles.subfiles().fileindex()) {
                File::FileMetadata *fileIndex = filesIndex.add_fileindex();

                fileIndex->CopyFrom(file);
                fileIndex->set_isdownloadable(filesSchemas.downloadedStack.doesFileExist(file.fileid()));
            }
            response->set_allocated_subfiles(new File::FilesIndex(filesIndex));
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    fileMove(UNUSED grpc::ServerContext *, const ::UsersBack_Maestro::FileMoveRequest *request, UNUSED UsersBack_Maestro::FileMoveStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            /* auto santaclausResponse = */ this->_clients.santaclaus.moveFile(
                request->fileid(),
                request->has_newfilename() ? std::optional(request->newfilename()) : std::nullopt,
                request->has_dirid() ? std::optional(request->dirid()) : std::nullopt
            );

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

// todo test
grpc::Status UsersBackService::
    fileRemove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::FileRemoveRequest *request, UNUSED UsersBack_Maestro::FileRemoveStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            const auto &file = this->_clients.santaclaus.getFile(request->fileid()); // get disk id from santaclaus

            this->_clients.santaclaus.virtualRemoveFile(request->fileid());
            try {
                if (!this->_clients.bugle.diskStatus(file.diskid())) { // check if disk is turned off
                    std::cout << "[INFO] Disk " << file.diskid() << " is turned off, add the file to the remove queue DB"
                              << std::endl;
                    this->_fileRemoveFailure(filesSchemas, file.diskid(), request->fileid());
                    return grpc::Status::OK;
                }
            } catch (const RequestFailureException &error) {
                std::cerr << "[WARNING] Fail to get disk status, add the file to the remove queue DB : " << error.what()
                          << std::endl;
                this->_fileRemoveFailure(filesSchemas, file.diskid(), request->fileid());
                return grpc::Status::OK;
            }

            // if disk is turned on
            Maestro_Vault::RemoveFileRequest removeFileRequest;

            removeFileRequest.set_diskid(file.diskid());
            removeFileRequest.set_userid(file.file().approxmetadata().userid());
            removeFileRequest.set_fileid(request->fileid());
            try {
                this->_clients.vault.removeFile(removeFileRequest);

                this->_clients.santaclaus.physicalRemoveFile(request->fileid());
            } catch (const RequestFailureException &error) { // if deletion in vault didn't succeed
                std::cerr << "[WARNING] Fail to remove file in vault, add the file to the remove queue DB : " << error.what()
                          << std::endl;
                this->_fileRemoveFailure(filesSchemas, file.diskid(), request->fileid());
            }
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

// todo test
grpc::Status UsersBackService::filesRemove(
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::FilesRemoveRequest *request,
    UsersBack_Maestro::FilesRemoveStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            *response = this->actFilesRemove(filesSchemas, request->fileid().begin(), request->fileid().end());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}
grpc::Status UsersBackService::dirMake(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::DirMakeRequest *request,
    UsersBack_Maestro::DirMakeStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.addDirectory(request->directory());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    dirRemove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirRemoveRequest *request, UNUSED UsersBack_Maestro::DirRemoveStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            auto my_response = this->_clients.santaclaus.removeDirectory(request->dirid());

            if (my_response.fileidstoremove().size())
                this->actFilesRemove(filesSchemas, my_response.fileidstoremove().begin(), my_response.fileidstoremove().end());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}
grpc::Status UsersBackService::
    dirMove(UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirMoveRequest *request, UNUSED UsersBack_Maestro::DirMoveStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.moveDirectory(
                request->dirid(),
                request->has_name() ? std::optional(request->name()) : std::nullopt,
                request->has_newlocationdirid() ? std::optional(request->newlocationdirid()) : std::nullopt
            );

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

void UsersBackService::_fileUploadFailure(
    FilesSchemas &filesSchemas, const File::NewFile &file, const Maestro_Santaclaus::AddFileStatus &addFileStatus
)
{
    filesSchemas.uploadQueue.uploadFile(addFileStatus.fileid(), file.metadata().userid(), addFileStatus.diskid(), file.content());
}

void UsersBackService::_askFileDownloadFailure(
    FilesSchemas &filesSchemas, const string &fileId, const Maestro_Santaclaus::GetFileStatus &file, const Date &expirationDate,
    UsersBack_Maestro::AskFileDownloadStatus &response
)
{
    filesSchemas.downloadQueue.add(fileId, file.file().approxmetadata().userid(), file.diskid());

    response.set_allocated_waitingtime(expirationDate.toAllocatedDuration());
}

void UsersBackService::_fileRemoveFailure(FilesSchemas &filesSchemas, const string &diskId, const string &fileId)
{
    filesSchemas.removeQueue.add(diskId, fileId);
}