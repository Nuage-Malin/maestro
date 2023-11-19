/**
 * @file SantaclausClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 14/03/2023
 * @copyright Nuage Malin
 */

#include <string>

#include <grpcpp/channel.h>

#include "SantaclausClient.hpp"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

SantaclausClient::SantaclausClient(const std::shared_ptr<grpc::ChannelInterface> &channel)
    : _stub(Maestro_Santaclaus::Maestro_Santaclaus_Service::NewStub(channel))
{
    if (!this->_stub)
        throw std::runtime_error(STR_FUNCTION + " could not create gRPC stub");
}

Maestro_Santaclaus::AddFileStatus SantaclausClient::addFile(const File::FileApproxMetadata &file, const uint64 &fileSize) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::AddFileRequest request;
    Maestro_Santaclaus::AddFileStatus response;

    request.set_filesize(fileSize);
    request.set_allocated_file(new File::FileApproxMetadata(file));
    this->_callLogger(__FUNCTION__);
    grpc::Status status = this->_stub->addFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

NODISCARD Maestro_Santaclaus::GetFileStatus SantaclausClient::getFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::GetFileRequest request;
    Maestro_Santaclaus::GetFileStatus response;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    grpc::Status status = this->_stub->getFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

NODISCARD Maestro_Santaclaus::GetDirectoryStatus
SantaclausClient::getDirectory(const string &userId, const std::optional<string> &dirId, bool isRecursive) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::GetDirectoryRequest request;
    Maestro_Santaclaus::GetDirectoryStatus response;

    request.set_userid(userId);
    if (dirId.has_value())
        request.set_dirid(dirId.value());
    request.set_isrecursive(isRecursive);
    this->_callLogger(__FUNCTION__);
    grpc::Status status = this->_stub->getDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::MoveFileStatus SantaclausClient::moveFile(const string &fileId, const string &dirId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::MoveFileStatus response;
    Maestro_Santaclaus::MoveFileRequest request;

    request.set_fileid(fileId);
    request.set_newdirid(dirId);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->moveFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RenameFileStatus SantaclausClient::renameFile(const string &fileId, const string &name) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RenameFileStatus response;
    Maestro_Santaclaus::RenameFileRequest request;

    request.set_fileid(fileId);
    request.set_newfilename(name);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->renameFile(&context, request, &response);

    // todo test
    return response;
}

Maestro_Santaclaus::RemoveFileStatus SantaclausClient::virtualRemoveFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFileStatus response;
    Maestro_Santaclaus::RemoveFileRequest request;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->virtualRemoveFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RemoveFileStatus SantaclausClient::physicalRemoveFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFileStatus response;
    Maestro_Santaclaus::RemoveFileRequest request;

    request.set_fileid(fileId);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->physicalRemoveFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RemoveFilesStatus SantaclausClient::physicalRemoveFiles(const Maestro_Santaclaus::RemoveFilesRequest &request
) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFilesStatus response;

    this->_callLogger(__FUNCTION__);
    auto status = _stub->physicalRemoveFiles(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::AddDirectoryStatus SantaclausClient::addDirectory(const File::FileApproxMetadata &directory) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::AddDirectoryRequest request;
    Maestro_Santaclaus::AddDirectoryStatus response;

    request.set_allocated_directory(new File::FileApproxMetadata(directory));
    this->_callLogger(__FUNCTION__);
    auto status = this->_stub->addDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RemoveDirectoryStatus SantaclausClient::removeDirectory(const string &dirId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveDirectoryRequest request;
    Maestro_Santaclaus::RemoveDirectoryStatus response;

    request.set_dirid(dirId);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->removeDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RemoveDirectoryStatus SantaclausClient::removeUser(const string &userId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveUserRequest request;
    Maestro_Santaclaus::RemoveDirectoryStatus response;

    request.set_userid(userId);
    this->_callLogger(__FUNCTION__);
    auto status = this->_stub->removeUser(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::MoveDirectoryStatus SantaclausClient::moveDirectory(const string &dirId, const string &newDirId) const
{
    // todo test
    grpc::ClientContext context;
    Maestro_Santaclaus::MoveDirectoryRequest request;
    Maestro_Santaclaus::MoveDirectoryStatus response;

    request.set_dirid(dirId);
    request.set_newdirid(newDirId);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->moveDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RenameDirectoryStatus SantaclausClient::renameDirectory(const string &dirId, const string &name) const
{
    // todo test
    grpc::ClientContext context;
    Maestro_Santaclaus::RenameDirectoryRequest request;
    Maestro_Santaclaus::RenameDirectoryStatus response;

    request.set_dirid(dirId);
    request.set_newdirname(name);
    this->_callLogger(__FUNCTION__);
    auto status = _stub->renameDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

void SantaclausClient::_callLogger(const string &functionName) const
{
    std::cout << "[CLIENT] " << functionName << std::endl;
}