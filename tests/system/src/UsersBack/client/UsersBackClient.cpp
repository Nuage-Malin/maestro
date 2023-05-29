/**
 * @file UsersBackClient.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/02/2023
 * @copyright Nuage Malin
 */

#include <grpcpp/channel.h>

#include "UsersBackClient.hpp"

UsersBackClient::UsersBackClient(const string &serverURI)
    : _client(
        UsersBack_Maestro::UsersBack_Maestro_Service::NewStub(grpc::CreateChannel(serverURI, grpc::InsecureChannelCredentials()))
    )
{
    if (!this->_client)
        throw std::runtime_error("Could not create gRPC stub");
}

void UsersBackClient::_manageStatusError(const string &funcName, const grpc::Status &status) const
{
    std::cerr << funcName << " request failed: " << status.error_message() << std::endl;
    std::cerr << "Error details: " << status.error_details() << std::endl;
}

std::optional<UsersBack_Maestro::GetFilesIndexStatus>
UsersBackClient::getFilesIndex(UsersBack_Maestro::GetFilesIndexRequest &request) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::GetFilesIndexStatus response;
    grpc::Status status = this->_client->getFilesIndex(&context, request, &response);

    if (!status.ok()) {
        this->_manageStatusError(__FUNCTION__, status);
        return std::nullopt;
    }
    return response;
}

std::optional<UsersBack_Maestro::FileUploadStatus> UsersBackClient::fileUpload(UsersBack_Maestro::FileUploadRequest &request
) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::FileUploadStatus response;
    grpc::Status status = this->_client->fileUpload(&context, request, &response);

    if (!status.ok()) {
        this->_manageStatusError(__FUNCTION__, status);
        return std::nullopt;
    }
    return response;
}

std::optional<UsersBack_Maestro::AskFileDownloadStatus>
UsersBackClient::askFileDownload(UsersBack_Maestro::AskFileDownloadRequest &request) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::AskFileDownloadStatus response;
    grpc::Status status = this->_client->askFileDownload(&context, request, &response);

    if (!status.ok()) {
        this->_manageStatusError(__FUNCTION__, status);
        return std::nullopt;
    }
    return response;
}