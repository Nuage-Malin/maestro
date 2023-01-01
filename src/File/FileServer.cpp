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

#include <mongocxx/exception/query_exception.hpp>

FileServer::FileServer(const mongocxx::database &file_database) : _fileDatabase(file_database)
{
    this->setFileBucket();
}

::grpc::Status FileServer::fileUpload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::FileUploadRequest *request, ::UsersBack_Maestro::FileUploadStatus *response)
{
    auto metadata = request->file().metadata();
    auto content = request->file().content();
    std::istream fileStream{std::istringstream(content).rdbuf()};
    bsoncxx::stdx::string_view my_filename{metadata.name()};

    try {
        _fileBucket.upload_from_stream(my_filename, &fileStream);
    } catch (const mongocxx::query_exception &e) {
        std::cerr << "mongocxx::query_exception: " << e.what() << std::endl;
        return ::grpc::Status::CANCELLED;
    } catch (...) {
        std::cerr << "Upload of file '" << my_filename << "' could not be registered in database" << std::endl;
        return ::grpc::Status::CANCELLED;
    }
    return ::grpc::Status::OK;
}

::grpc::Status FileServer::askFileDownload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::AskFileDownloadRequest *request, ::UsersBack_Maestro::AskFileDownloadStatus *response)
{
    std::string my_filename{request->GetMetadata().descriptor->file()->name()};

    bsoncxx::document::value my_filter =
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("filename", my_filename));
    mongocxx::options::find my_options;

    mongocxx::cursor my_cursor = _fileBucket.find(bsoncxx::document::view_or_value(my_filter), my_options);
    if (my_cursor.begin() == my_cursor.end())
        return grpc::Status::CANCELLED;
    auto *my_waiting_time(new google::protobuf::Duration());
    my_waiting_time->set_seconds(FileServer::DEFAULT_WAITING_TIME);
    response->set_allocated_waitingtime(my_waiting_time);
    return grpc::Status::OK;
}

::grpc::Status FileServer::fileDownload(
    ::grpc::ServerContext *context, const ::UsersBack_Maestro::FileDownloadRequest *request, ::File::File *response)
{
    std::string my_filename{request->GetMetadata().descriptor->file()->name()};

    bsoncxx::document::value my_filter =
        bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("filename", my_filename));
    mongocxx::options::find my_options;

    mongocxx::cursor my_cursor = _fileBucket.find(bsoncxx::document::view_or_value(my_filter), my_options);
    if (my_cursor.begin() == my_cursor.end()) // todo check for exceptions
        return grpc::Status::CANCELLED;
    for (auto i : my_cursor) {
        for (auto j : i) {
            std::cout << "j.get_string().value" << std::endl;
            std::cout << j.get_string().value << std::endl;
            //            j.key();
            //            j.get_value();
        }
    }

    //    response->set_content(); // TODO
    auto *my_metadata(new File::FileMetadata());
    auto *my_approxMetadata(new File::FileApproxMetadata());
    //    my_approxMetadata->set_name();
    //    my_approxMetadata->set_userid();
    //    my_approxMetadata->set_dirname();
    my_metadata->set_allocated_approxmetadata(my_approxMetadata);
    auto *my_creationTime(new google::protobuf::Timestamp());
    //    my_creationTime->set_seconds();
    my_metadata->set_allocated_creation(my_creationTime);
    auto *my_lastEditTime(new google::protobuf::Timestamp());
    //    my_lastEditTime->set_seconds();
    my_metadata->set_allocated_lastedit(my_lastEditTime);
    //    my_metadata->set_lasteditorid("");
    //    my_metadata->set_fileid("");
    response->set_allocated_metadata(my_metadata); // todo
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
    mongocxx::options::gridfs::bucket my_bucketOptions;
    my_bucketOptions.bucket_name("_fileBucketName");
    _fileBucket = _fileDatabase.gridfs_bucket(my_bucketOptions);
}