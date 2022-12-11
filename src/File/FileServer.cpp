/**
 * @file FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 9/11/22.
 * @brief TODO
 */

#include "FileServer.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>

::grpc::Status FileServer::fileUpload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::FileUploadRequest *request, ::UsersBack_Maestro::FileUploadStatus *response)
{
    auto metadata = request->file().GetMetadata();
    auto builder = bsoncxx::builder::stream::document{};
    builder << "content" << bsoncxx::builder::stream::open_array /*is that needed for a bytes protobuf type ?? */
            << request->file().content() << bsoncxx::builder::stream::close_array;
    bsoncxx::document::value doc_value = builder.extract();
    _fileCollection.insert_one(doc_value.view());
    return ::grpc::Status::OK;
}