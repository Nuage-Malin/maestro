/**
 * @file FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 9/11/22.
 * @brief TODO
 */

#include <sstream>
#include <iostream>

#include "FileServer.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <mongocxx/gridfs/bucket.hpp>

FileServer::FileServer(const mongocxx::database &file_database) : _fileDatabase(file_database)
{
    this->setFileBucket();
}

::grpc::Status FileServer::fileUpload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::FileUploadRequest *request, ::UsersBack_Maestro::FileUploadStatus *response)
{
    const File::FileApproxMetadata &metadata{request->file().metadata()};
    const string &content{request->file().content()};
    // auto builder = bsoncxx::builder::stream::document{};

    //    builder << "content" << bsoncxx::builder::stream::open_array /*is that
    //    needed for a bytes protobuf type ?? */
    //            << request->file().content() <<
    //            bsoncxx::builder::stream::close_array;
    //    bsoncxx::document::value doc_value = builder.extract();
    //    _fileCollection.insert_one(doc_value.view()); // TODO gridFS

    std::istream fileStream{std::istringstream(content).rdbuf()};
    bsoncxx::stdx::string_view filename{metadata.name()};

    std::cout << "Uploading file" << std::endl;
    const auto result = this->_fileBucket.upload_from_stream(filename, &fileStream);
    std::cout << "Uploaded file ID: " << result.id().get_string().value.to_string() << std::endl;
    return ::grpc::Status::OK;
}
::grpc::Status FileServer::askFileDownload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::AskFileDownloadRequest *request, ::UsersBack_Maestro::AskFileDownloadStatus *response)
{
    const string &fileId{request->fileid()};

    const bsoncxx::document::value filter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{bsoncxx::stdx::string_view{fileId}}));
    const mongocxx::options::find options;
    mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

    if (cursor.begin() == cursor.end())
        return grpc::Status::CANCELLED;

    auto *waiting_time = new google::protobuf::Duration();
    waiting_time->set_seconds(FileServer::DEFAULT_WAITING_TIME);
    response->set_allocated_waitingtime(waiting_time);

    return grpc::Status::OK;
}

::grpc::Status FileServer::fileDownload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::FileDownloadRequest *request, ::File::File *response)
{
    const string &fileId{request->fileid()};

    const bsoncxx::document::value filter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{bsoncxx::stdx::string_view{fileId}}));
    const mongocxx::options::find options;
    mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

    if (cursor.begin() == cursor.end()) // todo check for exceptions
        return grpc::Status::CANCELLED;
    for (auto i : cursor) {
        for (auto j : i) {
            std::cout << "j.get_string().value" << std::endl;
            std::cout << j.get_string().value << std::endl;
            //            j.key();
            //            j.get_value();
        }
    }

    //    response->set_content(); // TODO
    auto *metadata(new File::FileMetadata());
    auto *approxMetadata(new File::FileApproxMetadata());
    //    approxMetadata->set_name();
    //    approxMetadata->set_userid();
    //    approxMetadata->set_dirname();
    metadata->set_allocated_approxmetadata(approxMetadata);
    auto *creationTime(new google::protobuf::Timestamp());
    //    creationTime->set_seconds();
    metadata->set_allocated_creation(creationTime);
    auto *lastEditTime(new google::protobuf::Timestamp());
    //    lastEditTime->set_seconds();
    metadata->set_allocated_lastedit(lastEditTime);
    //    metadata->set_lasteditorid("");
    //    metadata->set_fileid("");
    response->set_allocated_metadata(metadata); // todo
    return grpc::Status::OK;
}

::grpc::Status FileServer::getFilesIndex(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::GetFilesIndexRequest *request, ::File::FilesIndex *response)
{
    return Service::getFilesIndex(context, request, response);
}

void FileServer::setFileBucket()
{
    if (!_fileDatabase)
        throw std::logic_error("Cannot get bucket because don't have database");
    mongocxx::options::gridfs::bucket bucketOptions;
    bucketOptions.bucket_name("_fileBucketName");
    this->_fileBucket = this->_fileDatabase.gridfs_bucket(bucketOptions);
}