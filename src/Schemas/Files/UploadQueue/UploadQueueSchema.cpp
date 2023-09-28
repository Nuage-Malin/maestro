/**
 * @file FilesUploadQueueSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#include <sstream>

#include "schemas.hpp"
#include "UploadQueueSchema.hpp"
#include "Utils/Date/Date.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

FilesUploadQueueSchema::FilesUploadQueueSchema(const mongocxx::database &database) : TemplateFileBucket(database, "uploadQueue"), TemplateSchema(database, "uploadQueue.files")
{
}

void FilesUploadQueueSchema::uploadFile(const string &fileId, const string &userId, const string &diskId, const string &content)
{
    std::istringstream ss(content);
    std::istream fileStream(ss.rdbuf());
    mongocxx::v_noabi::options::gridfs::upload options;

    options.metadata(
        makeDocument(makeField("diskId", diskId), makeField("userId", userId), makeField("createdAt", Date().toBSON()))
    );

    this->_fileBucket.upload_from_stream(fileId, &fileStream, options);
}

std::pair<std::vector<MongoCXX::ValueView>, Maestro_Vault::UploadFilesRequest>
FilesUploadQueueSchema::getDiskFiles(const string &diskId)
{
    const MongoCXX::Document filter = makeDocument(makeField("metadata.diskId", diskId));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view());
    Maestro_Vault::UploadFilesRequest result;
    std::vector<MongoCXX::ValueView> filesId;

    std::for_each(cursor.begin(), cursor.end(), [&result, &filesId, this](const MongoCXX::DocumentView &file) {
        const bsoncxx::document::view metadata = file["metadata"].get_document().value;
        auto *resultFile = result.add_files();
        const MongoCXX::ValueView &fileId = file["_id"].get_value();

        filesId.push_back(fileId);
        resultFile->set_fileid(file["filename"].get_string().value.to_string());
        resultFile->set_userid(metadata["userId"].get_string().value.to_string());
        resultFile->set_diskid(metadata["diskId"].get_string().value.to_string());

        std::ostringstream oss("");
        std::ostream ostream(oss.rdbuf());

        this->_fileBucket.download_to_stream(fileId, &ostream);
        resultFile->set_content(oss.str());
    });
    return std::make_pair(filesId, result);
}

std::unordered_set<string> FilesUploadQueueSchema::getFilesDisk()
{
    const MongoCXX::Document filter = makeDocument();
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("metadata.diskId", true)));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view(), options);
    std::unordered_set<string> disks;

    std::for_each(cursor.begin(), cursor.end(), [&disks](const MongoCXX::DocumentView &file) {
        disks.insert(file["metadata"].get_document().value["diskId"].get_string().value.to_string());
    });
    return disks;
}

NODISCARD string FilesUploadQueueSchema::getFile(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("filename", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", true)));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view(), options);

    if (cursor.begin() == cursor.end())
        throw NotFoundException("[FilesUploadQueueSchema::getFile] File not found: " + fileId);
    std::ostringstream oss("");
    std::ostream ostream(oss.rdbuf());

    this->_fileBucket.download_to_stream((*cursor.begin())["_id"].get_value(), &ostream);
    return oss.str();
}

NODISCARD uint64 FilesUploadQueueSchema::getUserQueueSpace(const string & userId, const Date &endDate)
{
    mongocxx::pipeline pipeline;

    pipeline.match(makeDocument(makeField("metadata.userId", userId), makeField("metadata.createdAt", makeDocument(makeField("$lte", endDate.toBSON())))));
    pipeline.group(makeDocument(makeField("_id", MongoCXX::Null()), makeField("totalUserQueueSpace", makeDocument(makeField("$sum", "$length")))));

    mongocxx::cursor cursor = this->_model.aggregate(pipeline);

    if (cursor.begin() == cursor.end())
        return 0;

    const MongoCXX::DocumentElement &totalUserQueueSpace = (*cursor.begin())["totalUserQueueSpace"];

    if (totalUserQueueSpace.type() == bsoncxx::type::k_int32)
        return static_cast<uint64>(totalUserQueueSpace.get_int32().value);
    else if (totalUserQueueSpace.type() == bsoncxx::type::k_int64)
        return totalUserQueueSpace.get_int64().value;
    else
        throw std::runtime_error("Invalid totalUserQueueSpace type");
}

// TODO: Does need to use thread to optimize it.
//  Delete files asynchronously with threads and then join them to wait for the end of the deletion
void FilesUploadQueueSchema::deleteFiles(const std::vector<MongoCXX::ValueView> &files)
{
    for (const auto &file : files)
        this->_fileBucket.delete_file(file);
}