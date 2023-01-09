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

/**
 * @brief Know if a file is downloadable in a certain time, available for download or none
 * @return Duration, in seconds, in which the file will be available
 *          if 0, the file is available,
 *          if negative (-1), the file has not been asked for download
 */
int FileServer::isDownloadable(string fileId)
{
    try {
        auto fileAvailabilityCountdown(_availabilityCountdown.at(fileId));
        auto [waitingTime, start] = fileAvailabilityCountdown;
        auto timeToWait(std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()
            - std::chrono::high_resolution_clock::now().time_since_epoch()
            + std::chrono::seconds(waitingTime.seconds()))
                            .count());
        if (timeToWait < 0) {
            _downloadCountdown.insert(_availabilityCountdown.extract(fileId));
            return 0;
        }
        return timeToWait;
    } catch (const std::out_of_range &e) {
        // could not find the file id in _availabilityCountdown,
        // which means that the file has not been asked for download
        try {
            auto fileDownloadCountdown(_downloadCountdown.at(fileId));
            auto [downloadTime, start] = fileDownloadCountdown;
            auto timeAvailable(std::chrono::duration_cast<std::chrono::seconds>(start.time_since_epoch()
                - std::chrono::high_resolution_clock::now().time_since_epoch()
                + std::chrono::seconds(downloadTime.seconds()))
                                   .count());
            if (timeAvailable < 0) {
                _downloadCountdown.erase(fileId);
                return -1;
            }
            return 0;
        } catch (const std::out_of_range &e) {
            // could not find the file id in _downloadCountdown,
            // which means that the file has not been asked for download
            return -1;
        }
        return -1;
    }
}

::grpc::Status FileServer::askFileDownload(::grpc::ServerContext *context,
    const ::UsersBack_Maestro::AskFileDownloadRequest *request, ::UsersBack_Maestro::AskFileDownloadStatus *response)
{
    auto timeToWait(isDownloadable(request->fileid()));
    auto *availabilityCountdown = new google::protobuf::Duration();

    if (timeToWait >= 0) {
        availabilityCountdown->set_seconds(timeToWait);
        response->set_allocated_waitingtime(availabilityCountdown);
    } else {
        try {
            const bsoncxx::document::value filter = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("_id", toObjectId(request->fileid())));
            const mongocxx::options::find options;
            mongocxx::cursor cursor = this->_fileBucket.find(bsoncxx::document::view_or_value(filter), options);

            if (cursor.begin() == cursor.end())
                return grpc::Status(grpc::StatusCode::NOT_FOUND, "File not found");

            const char *envWaitingTime = getenv("DOWNLOAD_WAITING_TIME");

            if (!envWaitingTime)
                availabilityCountdown->set_seconds(DEFAULT_WAITING_TIME);
            else
                availabilityCountdown->set_seconds(toInteger(envWaitingTime));
            response->set_allocated_waitingtime(availabilityCountdown);
            _availabilityCountdown.emplace(
                request->fileid(), std::make_tuple(*availabilityCountdown, std::chrono::high_resolution_clock::now()));
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
    }

    return grpc::Status::OK;
}

::grpc::Status FileServer::fileDownload(
    ::grpc::ServerContext *context, const ::UsersBack_Maestro::FileDownloadRequest *request, ::File::File *response)
{
    const string &fileId{request->fileid()};

    auto timeToWait(isDownloadable(request->fileid()));

    if (timeToWait != 0) {
        return grpc::Status::CANCELLED;
    }

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