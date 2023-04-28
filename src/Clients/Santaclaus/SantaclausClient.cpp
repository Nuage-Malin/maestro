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
        throw std::runtime_error(string(__FUNCTION__) + " could not create gRPC stub");
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
        throw RequestFailureException(status);
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
        throw RequestFailureException(status);
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
        throw RequestFailureException(status);
    return response;
}
