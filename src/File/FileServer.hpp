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
     * @brief Put a file in the cloud
     * @param context TODO
     * @param request TODO
     * @param response TODO
     * @return TODO
     */
    ::grpc::Status fileUpload(::grpc::ServerContext *context, const ::UsersBack_Maestro::FileUploadRequest *request,
        ::UsersBack_Maestro::FileUploadStatus *response) override;

    ::grpc::Status askFileDownload(::grpc::ServerContext *context,
        const ::UsersBack_Maestro::AskFileDownloadRequest *request,
        ::UsersBack_Maestro::AskFileDownloadStatus *response) override;

    ::grpc::Status fileDownload(::grpc::ServerContext *context, const ::UsersBack_Maestro::FileDownloadRequest *request,
        ::File::File *response) override;

    ::grpc::Status getFilesIndex(::grpc::ServerContext *context,
        const ::UsersBack_Maestro::GetFilesIndexRequest *request, ::File::FilesIndex *response) override;

  private:
    mongocxx::database _fileDatabase;
    mongocxx::gridfs::bucket _fileBucket;

    const string _fileBucketName{"fileBucket"};

    static const int DEFAULT_WAITING_TIME = 60 /* seconds */ * 60 /* minutes */ * 24 /* hours */;

  private:
    void _setFileBucket();
};

#endif