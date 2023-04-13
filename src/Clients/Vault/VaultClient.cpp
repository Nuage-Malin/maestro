/**
 * @file VaultClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#include <string>

#include <grpcpp/channel.h>

#include "VaultClient.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

VaultClient::VaultClient(const std::shared_ptr<grpc::ChannelInterface> &channel)
    : _stub(Maestro_Vault::Maestro_Vault_Service::NewStub(channel))
{
    if (!this->_stub)
        throw std::runtime_error(STR_FUNCTION + " could not create gRPC stub");
}

void VaultClient::uploadFile(const string &fileId, const string &userId, const string &diskId, const string &content) const
{
    grpc::ClientContext context;
    Maestro_Vault::UploadFileRequest request;
    Maestro_Vault::UploadFileStatus response;

    request.set_fileid(fileId);
    request.set_userid(userId);
    request.set_diskid(diskId);
    request.set_content(content);
    grpc::Status status = this->_stub->uploadFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status);
}

void VaultClient::uploadFiles(const Maestro_Vault::UploadFilesRequest &files) const
{
    grpc::ClientContext context;
    Maestro_Vault::UploadFilesStatus response;
    grpc::Status status = this->_stub->uploadFiles(&context, files, &response);

    if (!status.ok())
        throw RequestFailureException(status);
}

string VaultClient::downloadFile(const string &fileId, const string &userId, const string &diskId) const
{
    grpc::ClientContext context;
    Maestro_Vault::DownloadFileRequest request;
    Maestro_Vault::DownloadFileStatus response;

    request.set_fileid(fileId);
    request.set_userid(userId);
    request.set_diskid(diskId);
    grpc::Status status = this->_stub->downloadFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status);
    return response.content();
}

Maestro_Vault::DownloadFilesStatus VaultClient::downloadFiles(const Maestro_Vault::DownloadFilesRequest &files) const
{
    grpc::ClientContext context;
    Maestro_Vault::DownloadFilesStatus response;
    grpc::Status status = this->_stub->downloadFiles(&context, files, &response);

    if (!status.ok())
        throw RequestFailureException(status);
    return response;
}