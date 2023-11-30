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

UsersBackService::UsersBackService(const GrpcClients &clients, EventsManager &events) : TemplateService(events), _clients(clients)
{
}

grpc::Status UsersBackService::fileUpload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileUploadRequest *request,
    UsersBack_Maestro::FileUploadStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
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
                        this->_fileUploadFailure(request->file(), addFileStatus);
                    }
                } else {
                    // If the disk is offline, add it to the queue database
                    this->_fileUploadFailure(request->file(), addFileStatus);
                }
            } catch (const RequestFailureException &error) {
                // If fail to call bugle, upload it on the DB
                std::cerr << "[WARNING] Fail to get disk status (" << addFileStatus.diskid()
                          << "), upload file on the DB : " << error.what() << std::endl;
                this->_fileUploadFailure(request->file(), addFileStatus);
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
        [this, request, response](FilesSchemas &&, StatsSchemas &&statsSchemas) {
            const Date &date = request->has_date() ? Date(request->date()) : Date();
            const uint64 &totalDisksSpace = statsSchemas.diskInfo.getTotalDisksSpace(date);
            const uint64 &diskSpace = statsSchemas.userDiskInfo.getUserDiskSpace(request->userid(), date);
            //                + filesSchemas.uploadQueue.getUserQueueSpace(request->userid(), date); // todo

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

            const std::chrono::minutes expirationLimit(5);
            const Date &expirationDate = Date() + expirationLimit;

            // Check if file is in UploadQueue (in vaultCache)
            try {
                Maestro_Vault::GetFileMetaInfoStatus fileMetaInfo = this->_clients.vaultcache.getFileMetaInfo(request->fileid());

                for (auto store_type : fileMetaInfo.file().store_types()) {
                    // Array of storage_types, check within it if is uploadQueue
                    if (store_type == Maestro_Vault::storage_type::UPLOAD_QUEUE) {
                        auto file = _clients.vaultcache.downloadFile(request->fileid());

                        filesSchemas.downloadedStack.add(request->fileid(), expirationDate);
                        response->set_allocated_waitingtime(new google::protobuf::Duration());
                        return grpc::Status::OK;
                    }
                }
            } catch (const RequestFailureException &error) {}

            // If file is not in uploadQueue :
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
                        const string fileContent = this->_clients.vault.downloadFile(request->fileid());

                        filesSchemas.downloadedStack.add(request->fileid(), expirationDate);
                        response->set_allocated_waitingtime(new google::protobuf::Duration()); // TODO: Edit waiting time
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
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::cancelFileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::CancelFileDownloadRequest *request,
    UsersBack_Maestro::CancelFileDownloadStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&FilesSchemas, StatsSchemas &&) {
            FilesSchemas.downloadQueue.deleteFile(request->fileid());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::fileDownload(
    UNUSED grpc::ServerContext *context, const UsersBack_Maestro::FileDownloadRequest *request, File::File *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            Maestro_Santaclaus::GetFileStatus fileStatus = this->_clients.santaclaus.getFile(request->fileid());
            File::FileMetadata metadata = fileStatus.file();
            string file;

            try {
                // Call vaultcache and get content of file
                file = this->_clients.vaultcache.downloadFile(request->fileid());
            } catch (const RequestFailureException &error) {
                if (this->_clients.bugle.diskStatus(fileStatus.diskid())) {
                    // If the disk is online, download the file from vault
                    file = this->_clients.vault.downloadFile(request->fileid());
                } else {
                    // If the disk is offline, throw the error
                    std::cerr << "[WARNING] File not found in vault cache and the file disk is offline : " << error.what()
                              << std::endl;
                    throw error;
                }
            }

            metadata.set_state(File::FileState::DOWNLOADABLE);
            response->set_content(file);
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

            // Files
            filesIndex.clear_fileindex();
            for (const File::FileMetadata &file : subFiles.subfiles().fileindex()) {
                File::FileMetadata *fileIndex = filesIndex.add_fileindex();

                fileIndex->CopyFrom(file);
                try {
                    Maestro_Vault::GetFileMetaInfoStatus fileMetaInfo = this->_clients.vaultcache.getFileMetaInfo(file.fileid());

                    if (fileMetaInfo.file().store_types().size()) {
                        for (auto &store_type : fileMetaInfo.file().store_types()) {
                            if (store_type == Maestro_Vault::storage_type::UPLOAD_QUEUE) { // TODO
                                fileIndex->set_state(File::FileState::UPLOADING);
                            } else if (store_type == Maestro_Vault::storage_type::DOWNLOAD_QUEUE) {
                                fileIndex->set_state(File::FileState::DOWNLOADABLE);
                            }
                        }
                    } else {
                        if (filesSchemas.downloadQueue.doesFileExist(file.fileid())) {
                            fileIndex->set_state(File::FileState::ASKED);
                        } else {
                            fileIndex->set_state(File::FileState::STORED);
                        }
                    }
                } catch (const RequestFailureException &error) {
                    if (filesSchemas.downloadQueue.doesFileExist(file.fileid())) {
                        fileIndex->set_state(File::FileState::ASKED);
                    } else {
                        fileIndex->set_state(File::FileState::STORED);
                    }
                }
            }

            // Directories
            filesIndex.clear_dirindex();
            for (const File::DirMetadata &dir : subFiles.subfiles().dirindex()) {
                File::DirMetadata *dirIndex = filesIndex.add_dirindex();

                dirIndex->CopyFrom(dir);
                if (dir.approxmetadata().name() != "/"
                    && (!request->has_dirid() || dir.approxmetadata().dirid() == request->dirid())) {
                    try {
                        dirIndex->set_state(
                            this->_getDirectoryState(request->userid(), dir.dirid(), filesIndex, request->isrecursive())
                        );
                    } catch (const RequestFailureException &error) {
                        std::cerr << "[WARNING] Fail to get directory " << dir.dirid()
                                  << ") state, set it to UNKNOWN : " << error.what() << std::endl;
                        dirIndex->set_state(File::FileState::UNKNOWN);
                    }
                }
            }

            response->set_allocated_subfiles(new File::FilesIndex(filesIndex));

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    moveFile(UNUSED grpc::ServerContext *, const UsersBack_Maestro::MoveFileRequest *request, UNUSED UsersBack_Maestro::MoveFileStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.moveFile(request->fileid(), request->newdirid());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    renameFile(UNUSED grpc::ServerContext *, const UsersBack_Maestro::RenameFileRequest *request, UNUSED UsersBack_Maestro::RenameFileStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.renameFile(request->fileid(), request->newfilename());

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
                this->_clients.vaultcache.removeFile(request->fileid());
            } catch (const RequestFailureException &error) {
                std::cerr << "[WARNING] Fail to remove file into vault cache : " << error.what() << std::endl;
            }
            try {
                if (!this->_clients.bugle.diskStatus(file.diskid())) { // check if disk is turned off
                    std::cout << "[INFO] Disk " << file.diskid() << " is turned off, add the file to the remove queue DB"
                              << std::endl;
                    this->_fileRemoveFailure(filesSchemas, file.diskid(), request->fileid());
                    return grpc::Status::OK;
                }
            } catch (const RequestFailureException &error) {
                std::cerr << "[WARNING] Fail to get disk status, try to remove file from vault cache : " << error.what()
                          << std::endl;
                this->_fileRemoveFailure(filesSchemas, file.diskid(), request->fileid());
                return grpc::Status::OK;
            }

            // if disk is turned on
            try {
                this->_clients.vault.removeFile(request->fileid());

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
    UNUSED grpc::ServerContext *, const UsersBack_Maestro::DirMakeRequest *request, UsersBack_Maestro::DirMakeStatus *response
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
    moveDir(UNUSED grpc::ServerContext *, const UsersBack_Maestro::MoveDirectoryRequest *request, UNUSED UsersBack_Maestro::MoveDirectoryStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.moveDirectory(request->dirid(), request->newdirid());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    renameDir(UNUSED grpc::ServerContext *, const UsersBack_Maestro::RenameDirectoryRequest *request, UNUSED UsersBack_Maestro::RenameDirectoryStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&, StatsSchemas &&) {
            this->_clients.santaclaus.renameDirectory(request->dirid(), request->newdirname());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status UsersBackService::
    removeUser(UNUSED grpc::ServerContext *, const UsersBack_Maestro::RemoveUserRequest *request, UNUSED UsersBack_Maestro::RemoveUserStatus *)
{
    return this->_procedureRunner(
        [this, request](FilesSchemas &&filesSchemas, StatsSchemas &&) {
            const auto &filesToRemove = this->_clients.santaclaus.removeUser(request->userid()).fileidstoremove();

            this->_clients.vaultcache.removeUser(request->userid());
            for (const string &fileId : filesToRemove)
                filesSchemas.downloadedStack.deleteFile(fileId);
            filesSchemas.downloadQueue.deleteUser(request->userid());
            try {
                this->_clients.vault.removeUser(request->userid());
            } catch (const RequestFailureException &error) {
                std::cerr << "[WARNING] Fail to remove user in vault, push them into removeQueue : " << error.what() << std::endl;
                filesSchemas.removeQueue.add(filesToRemove.begin(), filesToRemove.end());
            }

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

File::FileState UsersBackService::_getDirectoryState(
    const string &userId, const string &directoryId, File::FilesIndex filesIndex, const bool &isRecursive
)
{
    File::FileState state = File::FileState::UNKNOWN;

    if (!isRecursive) {
        grpc::ServerContext context;
        UsersBack_Maestro::GetFilesIndexRequest request;
        UsersBack_Maestro::GetFilesIndexStatus response;

        request.set_dirid(directoryId);
        request.set_userid(userId);
        request.set_isrecursive(false);
        std::cout << "[CLIENT] UsersBack_Maestro::getFilesIndex" << std::endl;
        auto status = this->getFilesIndex(&context, &request, &response);

        if (!status.ok())
            throw RequestFailureException(status, __FUNCTION__);
        filesIndex.CopyFrom(response.subfiles());
    }

    for (const File::FileMetadata &fileMetadata : filesIndex.fileindex()) {
        if (directoryId != fileMetadata.approxmetadata().dirid())
            continue;

        state = this->_getFileState(fileMetadata.state(), state);
        if (state == File::FileState::DOWNLOADABLE)
            return state;
    }

    for (const File::DirMetadata &dirMetadata : filesIndex.dirindex()) {
        if (dirMetadata.approxmetadata().dirid() == directoryId) {
            state = this->_getFileState(this->_getDirectoryState(userId, dirMetadata.dirid(), filesIndex, false), state);
            if (state == File::FileState::DOWNLOADABLE)
                return state;
        }
    }

    return state;
}

File::FileState UsersBackService::_getFileState(const File::FileState &fileState, const File::FileState &currentState) const
{
    if (fileState == File::FileState::DOWNLOADABLE) {
        return File::FileState::DOWNLOADABLE;
    } else if (fileState == File::FileState::ASKED) {
        return File::FileState::ASKED;
    } else if (fileState == File::FileState::UPLOADING && currentState != File::FileState::ASKED) {
        return File::FileState::UPLOADING;
    } else if (fileState == File::FileState::STORED && currentState == File::FileState::UNKNOWN) {
        return File::FileState::STORED;
    }
    if (currentState == File::FileState::UNKNOWN)
        return fileState;
    return currentState;
}

void UsersBackService::_fileUploadFailure(const File::NewFile &file, const Maestro_Santaclaus::AddFileStatus &addFileStatus)
{
    _clients.vaultcache.uploadFile(
        addFileStatus.fileid(),
        file.metadata().userid(),
        addFileStatus.diskid(),
        file.content(),
        Maestro_Vault::storage_type::UPLOAD_QUEUE
    );
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