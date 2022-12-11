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
}

bool FileClient::fileUpload(
    const std::string &name, const std::string &dirname, const std::string &userID, const std::string &content)
{
    grpc::ClientContext my_context;
    UsersBack_Maestro::FileUploadRequest my_request;
    UsersBack_Maestro::FileUploadStatus my_response;
    grpc::Status my_status;
    auto *my_file{new File::NewFile()};
    auto *my_metadata{new File::FileApproxMetadata()};

    my_metadata->set_name(name);
    my_metadata->set_dirname(dirname);
    my_metadata->set_userid(userID);
    my_file->set_allocated_metadata(my_metadata);
    my_file->set_content(content);
    my_request.set_allocated_file(my_file);

    my_status = _stub->fileUpload(&my_context, my_request, &my_response);
    //    delete my_file;     // Causes sesgfault idk why
    //    delete my_metadata; // Causes sesgfault idk why
    if (my_status.ok()) {
        std::cout << "Sent request to " /*<< TODO insert server IP*/ << std::endl;
        return true;
    }
    std::cerr << "Could not send to " /*<< todo insert server IP*/ << std::endl;
    return false;
}