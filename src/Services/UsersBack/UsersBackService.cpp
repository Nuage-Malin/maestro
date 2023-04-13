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

grpc::Status UsersBackService::askFileDownload(
    grpc::ServerContext *context, const UsersBack_Maestro::AskFileDownloadRequest *request,
    UsersBack_Maestro::AskFileDownloadStatus *response
)
{
    return this->_procedureRunner([this, request, response]() {
        try {
            // Check if the file is already downloaded
            this->_filesSchemas.downloadedStack.getFileExpirationDate(request->fileid());

            response->set_allocated_waitingtime(new google::protobuf::Duration());
            return grpc::Status::OK;
        } catch (const NotFoundException &error) {
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
                        this->_clients.vault.downloadFile(request->fileid(), file.file().userid(), file.diskid());

                    this->_filesSchemas.downloadedStack.pushFile(request->fileid(), expirationDate, fileContent);
                    response->set_allocated_waitingtime(expirationDate.toAllocatedDuration()); // TODO: Edit waiting time
                } else {
                    // If the disk is offline, add the file to the queue database
                    this->_filesSchemas.downloadQueue.add(request->fileid(), file.file().userid(), file.diskid());

                    response->set_allocated_waitingtime(expirationDate.toAllocatedDuration());
                }
                return grpc::Status::OK;
            }
        }
    });
}