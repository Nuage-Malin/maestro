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

bool FileClient::fileUpload(
    const std::string &name, const std::string &dirname, const std::string &userID, const std::string &content) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::FileUploadRequest request;
    UsersBack_Maestro::FileUploadStatus response;
    grpc::Status status;
    auto *file{new File::NewFile()};
    auto *metadata{new File::FileApproxMetadata()};

    metadata->set_name(name);
    metadata->set_dirname(dirname);
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

bool FileClient::askFileDownload(const std::string &fileId) const
{
    grpc::ClientContext context;
    UsersBack_Maestro::AskFileDownloadRequest request;
    UsersBack_Maestro::AskFileDownloadStatus response;
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
