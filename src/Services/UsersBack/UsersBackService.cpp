/**
 * @file UsersBackService.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "schemas.hpp"
#include "UsersBackService.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

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