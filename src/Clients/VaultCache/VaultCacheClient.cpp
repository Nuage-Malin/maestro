/**
 * @file VaultCacheClient.cpp
 * @author Arthur Jourdan
 * @date of creation 30/09/23.
 * @copyright Nuage Malin
 * @brief Implementation of client to request vault cache server, in other words, request temporary file storage
 */

#include <grpcpp/channel.h>

#include "VaultCacheClient.hpp"

#include "Exceptions/RequestFailure/RequestFailureException.hpp"

VaultCacheClient::VaultCacheClient(const std::shared_ptr<grpc::ChannelInterface> &channel)
    : _stub(Maestro_Vault::Maestro_Vault_Service::NewStub(channel))
{
    if (!this->_stub)
        throw std::runtime_error(STR_FUNCTION + " could not create gRPC stub");
}

void VaultCacheClient::uploadFile(
    const string &fileId, const string &userId, const string &diskId, const string &content,
    const Maestro_Vault::storage_type &store_type
) const
{
    grpc::ClientContext context;
    Maestro_Vault::UploadFileRequest request;
    Maestro_Vault::UploadFileStatus response;

    request.set_fileid(fileId);
    request.set_userid(userId);
    request.set_diskid(diskId);
    request.set_content(content);
    request.set_store_type(store_type);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->uploadFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
}

void VaultCacheClient::uploadFiles(const Maestro_Vault::UploadFilesRequest &files) const
{
    grpc::ClientContext context;
    Maestro_Vault::UploadFilesStatus response;
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->uploadFiles(&context, files, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
}

string VaultCacheClient::downloadFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Vault::DownloadFileRequest request;
    Maestro_Vault::DownloadFileStatus response;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->downloadFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response.content();
}

Maestro_Vault::DownloadFilesStatus VaultCacheClient::downloadFiles(const Maestro_Vault::DownloadFilesRequest &files) const
{
    grpc::ClientContext context;
    Maestro_Vault::DownloadFilesStatus response;
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->downloadFiles(&context, files, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Vault::DownloadFilesStatus VaultCacheClient::downloadStorageTypeFiles(const Maestro_Vault::storage_type &store_type) const
{
    grpc::ClientContext context;
    Maestro_Vault::DownloadStorageTypeFilesRequest request;
    Maestro_Vault::DownloadFilesStatus response;

    request.set_store_type(store_type);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->downloadStorageTypeFiles(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Vault::RemoveFileStatus VaultCacheClient::removeFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Vault::RemoveFileStatus response;
    Maestro_Vault::RemoveFileRequest request;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->removeFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);

    this->_events.emit<const string &>(
        Event::REMOVE_VAULT_CACHE_FILE, file.fileId
    );
    return response;
}

Maestro_Vault::RemoveFilesStatus VaultCacheClient::removeFiles(const Maestro_Vault::RemoveFilesRequest &files) const
{
    grpc::ClientContext context;
    Maestro_Vault::RemoveFilesStatus response;

    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->removeFiles(&context, files, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);

    for (const DownloadedStack &file : files.fileids()) {
        try {
            for (const string &failureFileId : response.fileidfailures())
                if (file.fileId == failureFileId)
                    throw std::runtime_error(STR_FUNCTION + ": [Failed to remove file " + file.fileId + " from vaultcache]");

            this->_events.emit<const string &>(
                Event::REMOVE_VAULT_CACHE_FILE, file.fileId
            );
        } catch (const std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
        }
    }
    return response;
}

void VaultCacheClient::removeUser(const string &userId) const
{
    grpc::ClientContext context;
    Maestro_Vault::RemoveUserRequest request;
    Maestro_Vault::RemoveUserStatus response;

    request.set_userid(userId);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->removeUser(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
}

Maestro_Vault::GetFileMetaInfoStatus VaultCacheClient::getFileMetaInfo(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Vault::GetFileMetaInfoRequest request;
    Maestro_Vault::GetFileMetaInfoStatus response;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->getFileMetaInfo(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Vault::GetFilesMetaInfoStatus VaultCacheClient::getFilesMetaInfo(
    std::optional<string> userId, std::optional<string> diskId, std::optional<Maestro_Vault::storage_type> store_type
) const
{
    grpc::ClientContext context;
    Maestro_Vault::GetFilesMetaInfoRequest request;
    Maestro_Vault::GetFilesMetaInfoStatus response;

    if (userId.has_value()) {
        request.set_userid(*userId);
    }
    if (diskId.has_value()) {
        request.set_diskid(*diskId);
    }
    if (store_type.has_value()) {
        request.set_store_type(*store_type);
    }
    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->getFilesMetaInfo(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Vault::GetFilesDisksStatus VaultCacheClient::getFilesDisks(const Maestro_Vault::GetFilesDisksRequest &request) const
{
    grpc::ClientContext context;
    Maestro_Vault::GetFilesDisksStatus response;

    this->_callLogger(__FUNCTION__);
    const grpc::Status status = this->_stub->getFilesDisks(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

void VaultCacheClient::_callLogger(const string &functionName) const
{
    std::cout << "[CLIENT] vault-cache - " << functionName << std::endl;
}