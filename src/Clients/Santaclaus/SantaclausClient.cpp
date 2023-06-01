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
    grpc::Status status = this->_stub->getDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::MoveFileStatus
SantaclausClient::moveFile(const string &fileId, const std::optional<string> &name, const std::optional<string> &dirId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::MoveFileStatus response;
    Maestro_Santaclaus::MoveFileRequest request;

    request.set_fileid(fileId);
    if (name.has_value()) {
        request.set_newfilename(name.value());
    }
    if (dirId.has_value()) {
        request.set_dirid(dirId.value());
    }
    auto status = _stub->moveFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

Maestro_Santaclaus::RemoveFileStatus SantaclausClient::virtualRemoveFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFileStatus response;
    Maestro_Santaclaus::RemoveFileRequest request;

    request.set_fileid(fileId);
    auto status = _stub->virtualRemoveFile(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

// todo do same method for several files (present in Maestro_Santaclaus)
Maestro_Santaclaus::RemoveFileStatus SantaclausClient::physicalRemoveFile(const string &fileId) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFileStatus response;
    Maestro_Santaclaus::RemoveFileRequest request;

    request.set_fileid(fileId);
    auto status = _stub->physicalRemoveFile(&context, request, &response);

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
    auto status = _stub->removeDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}
Maestro_Santaclaus::MoveDirectoryStatus SantaclausClient::moveDirectory(
    const string &dirId, const std::optional<string> &name, const std::optional<string> &newLocationDirId
) const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::MoveDirectoryRequest request;
    Maestro_Santaclaus::MoveDirectoryStatus response;

    request.set_dirid(dirId);
    if (name.has_value())
        request.set_name(name.value());
    if (newLocationDirId.has_value())
        request.set_newlocationdirid(newLocationDirId.value());
    auto status = _stub->moveDirectory(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}