/**
 * @file UploadQueueSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#include "UploadQueueSchema.hpp"
#include "Utils/Date/Date.hpp"

UploadQueueSchema::UploadQueueSchema(const mongocxx::database &database) : TemplateFileBucket(database, "uploadQueue")
{
}

void UploadQueueSchema::uploadFile(const string &fileId, const string &userId, const string &diskId, const string &content)
{
    std::istringstream ss(content);
    std::istream fileStream(ss.rdbuf());
    mongocxx::v_noabi::options::gridfs::upload options;

    options.metadata(
        makeDocument(makeField("diskId", diskId), makeField("userId", userId), makeField("createdAt", Date().toBSON()))
    );

    this->_fileBucket.upload_from_stream(fileId, &fileStream, options);
}

Maestro_Vault::UploadFilesRequest UploadQueueSchema::getDiskFiles(const string &diskId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("metadata.diskId", diskId));
    const mongocxx::options::find options;
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view());
    Maestro_Vault::UploadFilesRequest result;

    std::for_each(cursor.begin(), cursor.end(), [&result, this](const MongoCXX::DocumentView &file) {
        const bsoncxx::document::view metadata = file["metadata"].get_document().value;
        auto *resultFile = result.add_files();

        resultFile->set_fileid(file["filename"].get_string().value.to_string());
        resultFile->set_userid(metadata["userId"].get_string().value.to_string());
        resultFile->set_diskid(metadata["diskId"].get_string().value.to_string());

        std::ostringstream oss("");
        std::ostream ostream(oss.rdbuf());

        this->_fileBucket.download_to_stream(file["_id"].get_value(), &ostream);
        resultFile->set_content(oss.str());
    });
    return result;
}

std::unordered_set<string> UploadQueueSchema::getFilesDisk()
{
    const bsoncxx::document::value filter = makeDocument();
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("metadata.diskId", true)));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view(), options);
    std::unordered_set<string> disks;

    std::for_each(cursor.begin(), cursor.end(), [&disks](const MongoCXX::DocumentView &file) {
        disks.insert(file["metadata"].get_document().value["diskId"].get_string().value.to_string());
    });
    return disks;
}