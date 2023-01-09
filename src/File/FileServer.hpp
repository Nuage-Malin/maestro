/**
 * @file FileServer.hpp
 * @author Arthur Jourdan
 * @date of creation 9/11/22.
 * @brief TODO
 */

#ifndef MAESTRO_FILESERVER_HPP
#define MAESTRO_FILESERVER_HPP

#include <utility>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>

#include "common/File.grpc.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"

class FileServer : public UsersBack_Maestro::UsersBack_Maestro_Service::Service {
  public:
    FileServer(mongocxx::database &&fileDatabase) : _fileDatabase(std::move(fileDatabase))
    {
    }
    explicit FileServer(const mongocxx::database &file_database);
    ;
    ~FileServer() override = default;

    /**
     * @brief Upload a file to the database
     *
     * @param context gRPC related
     * @param request gRPC related, see protobuf and related documentation
     * @param response gRPC related, see protobuf and related documentation
     * @return ::grpc::Status OK if uploaded successfully,
     *          INVALID_ARGUMENT if bad query or invalid param
     *          INTERNAL if any other exception was thrown
     */
    ::grpc::Status fileUpload(::grpc::ServerContext *context, const ::UsersBack_Maestro::FileUploadRequest *request,
        ::UsersBack_Maestro::FileUploadStatus *response) override;

    /**
     * @brief Ask to download a file to download it later
     *
     * @param context gRPC related
     * @param request gRPC related, see protobuf and related documentation
     * @param response gRPC related, see protobuf and related documentation
     * @return ::grpc::Status OK if file is waiting before being downloadable,
     *          INVALID_ARGUMENT if bad query
     *          INTERNAL if any other exception was thrown
     *
     * @throw std::invalid_argument if the environment variable DOWNLOAD_WAITING_TIME isn't found
     */
    ::grpc::Status askFileDownload(::grpc::ServerContext *context,
        const ::UsersBack_Maestro::AskFileDownloadRequest *request,
        ::UsersBack_Maestro::AskFileDownloadStatus *response) override;

    /**
     * @brief Download an asked and available file
     *
     * @param context gRPC related
     * @param request gRPC related, see protobuf and related documentation
     * @param response gRPC related, see protobuf and related documentation
     * @return ::grpc::Status OK if uploaded successfully,
     *          NOT_FOUND if the file is not downloadable at the moment
     *          CANCELLED if miscellaneous behaviour occurred
     */
    ::grpc::Status fileDownload(::grpc::ServerContext *context, const ::UsersBack_Maestro::FileDownloadRequest *request,
        ::File::File *response) override;

    /**
     * @brief Get user files list
     *
     * @param context gRPC related
     * @param request gRPC related, see protobuf and related documentation
     * @param response gRPC related, see protobuf and related documentation
     * @return ::grpc::Status OK if got file index successfully,
     *          INVALID_ARGUMENT if bad query or invalid param
     *          INTERNAL if any other exception was thrown
     */
    ::grpc::Status getFilesIndex(::grpc::ServerContext *context,
        const ::UsersBack_Maestro::GetFilesIndexRequest *request, ::File::FilesIndex *response) override;

  private:
    mongocxx::database _fileDatabase;
    mongocxx::gridfs::bucket _fileBucket;
    std::unordered_map<string,
        std::tuple<google::protobuf::Duration, std::chrono::time_point<std::chrono::high_resolution_clock>>>
        _availabilityCountdown;
    std::unordered_map<string,
        std::tuple<google::protobuf::Duration, std::chrono::time_point<std::chrono::high_resolution_clock>>>
        _downloadCountdown;

    const string _fileBucketName{"fileBucket"};
    static const int DEFAULT_WAITING_TIME = 60 /* seconds */;

  private:
    void _setFileBucket();
    int isDownloadable(string fileId);
};

#endif /* MAESTRO_FILESERVER_HPP */