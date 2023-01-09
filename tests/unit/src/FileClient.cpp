/**
 * @file FileClient.cpp
 * @author Arthur Jourdan
 * @date of creation 9/12/22.
 * @brief TODO
 */

#include <string>

#include <grpcpp/channel.h>

#include "FileClient.hpp"

FileClient::FileClient(const std::string &ServerIP)
    : _stub(UsersBack_Maestro::UsersBack_Maestro_Service::NewStub(
        grpc::CreateChannel(ServerIP, grpc::InsecureChannelCredentials())))
{
    if (!_stub)
        throw std::runtime_error("Could not create gRPC stub");
}

bool FileClient::fileUpload(UsersBack_Maestro::FileUploadStatus &response, const std::string &name,
    const std::string &dirpath, const std::string &userID, const std::string &content) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::FileUploadRequest request;
    grpc::Status status;
    auto *file{new File::NewFile()};
    auto *metadata{new File::FileApproxMetadata()};

    metadata->set_name(name);
    metadata->set_dirpath(dirpath);
    metadata->set_userid(userID);
    file->set_allocated_metadata(metadata);
    file->set_content(content);
    request.set_allocated_file(file);

    status = this->_stub->fileUpload(&context, request, &response);
    if (!status.ok()) {
        std::cerr << "Could not send upload request to server" << std::endl;
        std::cerr << "Error code : " << status.error_code() << ", error detail : " << status.error_details()
                  << ", error message : " << status.error_message() << std::endl;
        return false;
    }
    std::cout << "Sent upload request to server" << std::endl;
    return true;
}

bool FileClient::askFileDownload(UsersBack_Maestro::AskFileDownloadStatus &response, const std::string &fileId) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::AskFileDownloadRequest request;
    grpc::Status status;

    request.set_fileid(fileId);

    status = this->_stub->askFileDownload(&context, request, &response);
    if (!status.ok()) {
        std::cerr << "Could not send download request to server" << std::endl;
        std::cerr << "Error code : " << status.error_code() << ", error message : " << status.error_message()
                  << ", error detail : " << status.error_details() << std::endl;
        return false;
    }
    std::cout << "Sent download request to server" << std::endl;
    std::cout << "Waiting time: " << response.waitingtime().seconds() << std::endl;
    return true;
}

bool FileClient::fileDownload(File::File &response, const std::string &fileId)
{
    grpc::ClientContext context;
    UsersBack_Maestro::FileDownloadRequest request;
    grpc::Status status;

    request.set_fileid(fileId);

    status = this->_stub->fileDownload(&context, request, &response);
    if (!status.ok()) {
        std::cerr << "Could not download from server" << std::endl;
        std::cerr << "Error code : " << status.error_code() << ", error message : " << status.error_message()
                  << ", error detail : " << status.error_details() << std::endl;
        return false;
    }
    std::cout << "Downloaded file with id '" << fileId << "'" << std::endl;
    return true;
}

bool FileClient::getFilesIndex(File::FilesIndex &response, const std::string &userId, const std::string &dirpath) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::GetFilesIndexRequest request;
    grpc::Status status;

    request.set_userid(userId);
    request.set_dirpath(dirpath);

    status = this->_stub->getFilesIndex(&context, request, &response);
    if (!status.ok()) {
        std::cerr << "Could not get files index from server" << std::endl;
        std::cerr << "Error code : " << status.error_code() << ", error message : " << status.error_message()
                  << ", error detail : " << status.error_details() << std::endl;
        return false;
    }
    std::cout << "Got files index from server" << std::endl;
    return true;
}