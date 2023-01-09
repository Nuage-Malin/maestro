/**
 * @file FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 9/11/22.
 * @brief Implementation of the gRPC procedures (server) for file upload, index and download,
 *          interfacing with mongo db's gridfs for storage
 */

#include <sstream>
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <mongocxx/gridfs/bucket.hpp>

#include <mongocxx/exception/query_exception.hpp>

#include "messages/MessagesUtils/MessagesUtils.hpp"
#include "messages/File/FileMetadata/FileMetadata.hpp"
#include "FileServer.hpp"

FileServer::FileServer(const mongocxx::database &file_database) : _fileDatabase(file_database)
{
    this->_setFileBucket();
}

::grpc::Status FileServer::fileUpload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::FileUploadRequest *request, ::UsersBack_Maestro::FileUploadStatus *response)
{
    try {
        const FileApproxMetadata metadata = request->file().metadata();
        std::istream fileStream{std::istringstream(request->file().content()).rdbuf()};
        mongocxx::v_noabi::options::gridfs::upload options;

        options.metadata(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("userId", metadata.userId),
            bsoncxx::builder::basic::kvp("dirPath", metadata.dirPath),
            bsoncxx::builder::basic::kvp("creation", bsoncxx::types::b_date(std::chrono::high_resolution_clock::now())),
            bsoncxx::builder::basic::kvp("lastEdit", bsoncxx::types::b_date(std::chrono::high_resolution_clock::now())),
            bsoncxx::builder::basic::kvp("lastEditorId", metadata.userId)));

        const auto &result =
            this->_fileBucket.upload_from_stream(bsoncxx::stdx::string_view{metadata.name}, &fileStream, options);

        //        response->set_fileid(result.id().get_oid().value.to_string());
    } catch (const mongocxx::query_exception &e) {
        std::cerr << "[FileServer::fileUpload] mongocxx::query_exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Query exception", e.what());
    } catch (const std::invalid_argument &e) {
        std::cerr << "[FileServer::fileUpload] std::invalid_argument: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument", e.what());
    } catch (const std::exception &e) {
        std::cerr << "[FileServer::fileUpload] std::exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error", e.what());
    } catch (...) {
        std::cerr << "[FileServer::fileUpload] Upload of file could not be registered in database" << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
    }
    return ::grpc::Status::OK;
}

::grpc::Status FileServer::askFileDownload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::AskFileDownloadRequest *request, ::UsersBack_Maestro::AskFileDownloadStatus *response)
{
    try {
        const bsoncxx::document::value filter =
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", toObjectId(request->fileid())));
        const mongocxx::options::find options;
        mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

        if (cursor.begin() == cursor.end())
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "File not found");

        auto *waitingTime = new google::protobuf::Duration();
        const char *envWaitingTime = getenv("DOWNLOAD_WAITING_TIME");

        if (!envWaitingTime)
            waitingTime->set_seconds(DEFAULT_WAITING_TIME);
        else
            waitingTime->set_seconds(toInteger(envWaitingTime));
        response->set_allocated_waitingtime(waitingTime);

    } catch (const mongocxx::query_exception &e) {
        std::cerr << "[FileServer::askFileDownload] mongocxx::query_exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Query exception", e.what());
    } catch (const std::exception &e) {
        std::cerr << "[FileServer::askFileDownload] std::exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error", e.what());
    } catch (...) {
        std::cerr << "[FileServer::askFileDownload] Error" << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Unknown error");
    }
    return grpc::Status::OK;
}

::grpc::Status FileServer::fileDownload(
    ::grpc::ServerContext *context, const ::UsersBack_Maestro::FileDownloadRequest *request, ::File::File *response)
{
    const string &fileId{request->fileid()};

    const bsoncxx::document::value filter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{bsoncxx::stdx::string_view{fileId}}));
    const mongocxx::options::find options;
    mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

    if (cursor.begin() == cursor.end()) // todo check for exceptions
        return grpc::Status::CANCELLED;

    std::ostringstream my_ostringstream(std::ostringstream() << "");
    std::ostream my_ostream{my_ostringstream.rdbuf()};
    for (auto i : cursor) {
        FileMetadata metadata(i);
        response->set_allocated_metadata(metadata.toProtobuf());
        _fileBucket.download_to_stream(i["_id"].get_value(), &my_ostream);
        response->set_content(my_ostringstream.str());
        return grpc::Status::OK;
    }
    return grpc::Status::CANCELLED;
}

::grpc::Status FileServer::getFilesIndex(
    grpc::ServerContext *context, const UsersBack_Maestro::GetFilesIndexRequest *request, File::FilesIndex *response)
{
    try {
        const string &userId{request->userid()};
        const string &dirPath{request->dirpath()};

        // Validate request
        toObjectId(userId);
        if (!isValidDirectory(dirPath))
            throw std::invalid_argument("Invalid directory path: " + dirPath);

        // Get files index
        const bsoncxx::document::value filter =
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("metadata.userId", request->userid()),
                bsoncxx::builder::basic::kvp("metadata.dirPath", request->dirpath()));
        const mongocxx::options::find options;
        mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

        for (auto i : cursor) {
            FileMetadata(i).toProtobuf(*response->add_index());
        }
    } catch (const mongocxx::query_exception &e) {
        std::cerr << "[FileServer::getFilesIndex] mongocxx::query_exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Query exception", e.what());
    } catch (const mongocxx::exception &e) {
        std::cerr << "[FileServer::getFilesIndex] mongocxx::exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error", e.what());
    } catch (const std::exception &e) {
        std::cerr << "[FileServer::getFilesIndex] std::exception: " << e.what() << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error", e.what());
    } catch (...) {
        std::cerr << "[FileServer::getFilesIndex] Unknown exception" << std::endl;
        return grpc::Status(grpc::StatusCode::INTERNAL, "Unknown error");
    }
    return grpc::Status::OK;
}

void FileServer::_setFileBucket()
{
    if (!this->_fileDatabase)
        throw std::logic_error("Cannot get bucket because don't have database");
    mongocxx::options::gridfs::bucket bucketOptions;
    bucketOptions.bucket_name(this->_fileBucketName);
    this->_fileBucket = this->_fileDatabase.gridfs_bucket(bucketOptions);
}