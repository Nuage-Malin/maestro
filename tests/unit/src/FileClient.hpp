/**
 * @file FileClient.hpp
 * @author Arthur Jourdan
 * @date of creation 9/12/22.
 * @brief TODO
 */

#ifndef TEST_TESTING_FILECLIENT_HPP
#define TEST_TESTING_FILECLIENT_HPP

#include "common/File.grpc.pb.h"
#include "common/File.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include <grpcpp/grpcpp.h>

class FileClient {
  public:
    explicit FileClient(const std::string &serverIP);
    ~FileClient() = default;

    bool fileUpload(UsersBack_Maestro::FileUploadStatus &response, const std::string &name, const std::string &dirname,
        const std::string &userID, const std::string &content) const;
    bool askFileDownload(UsersBack_Maestro::AskFileDownloadStatus &response, const std::string &fileId) const;
    bool fileDownload(File::File &response, const std::string &fileId);
    bool getFilesIndex(File::FilesIndex &response, const std::string &userId, const std::string &dirpath) const;

  private:
    std::unique_ptr<UsersBack_Maestro::UsersBack_Maestro_Service::Stub> _stub;
};

#endif // TEST_TESTING_FILECLIENT_HPP