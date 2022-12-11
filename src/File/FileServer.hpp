/**
 * @file FileServer.hpp
 * @author Arthur Jourdan
 * @date of creation 9/11/22.
 * @brief TODO
 */

#ifndef CMAKE_TEMPLATE_FILESERVER_HPP
#define CMAKE_TEMPLATE_FILESERVER_HPP

#include "File.grpc.pb.h"
#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include <mongocxx/collection.hpp>

class FileServer : public UsersBack_Maestro::UsersBack_Maestro_Service::Service {
  public:
    FileServer(mongocxx::collection &fileCollection) : _fileCollection(fileCollection){};
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

    /*::grpc::Status askFileDownload(::grpc::ServerContext* context, const ::UsersBack_Maestro::AskFileDownloadRequest*
                                                                       request,
    ::UsersBack_Maestro::AskFileDownloadStatus* response) override;

    ::grpc::Status fileDownload(::grpc::ServerContext* context, const ::UsersBack_Maestro::FileDownloadRequest*
                                                                request, ::File::File* response) override;

    ::grpc::Status getFilesIndex(::grpc::ServerContext* context, const ::UsersBack_Maestro::GetFilesIndexRequest*
                                                                 request, ::File::FilesIndex* response) override;

*/
  private:
    mongocxx::collection _fileCollection;
};

#endif // CMAKE_TEMPLATE_FILESERVER_HPP