/**
 * @file FileClient.hpp
 * @author Arthur Jourdan
 * @date of creation 9/12/22.
 * @brief TODO
 */

#ifndef TEST_TESTING_FILECLIENT_HPP
#define TEST_TESTING_FILECLIENT_HPP

#include "File.grpc.pb.h"
#include "File.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include <grpcpp/grpcpp.h>

class FileClient {
  public:
    explicit FileClient(const std::string &ServerIP);
    ~FileClient() = default;

    bool fileUpload(
        const std::string &name, const std::string &dirname, const std::string &userID, const std::string &content);
    bool askFileDownload();
    bool fileDownload();
    bool getFilesIndex();

  private:
    std::unique_ptr<UsersBack_Maestro::UsersBack_Maestro_Service::Stub> _stub;
};

#endif // TEST_TESTING_FILECLIENT_HPP