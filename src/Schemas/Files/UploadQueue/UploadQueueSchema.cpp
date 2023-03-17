/**
 * @file UploadQueueSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#include "UploadQueueSchema.hpp"
#include "Utils/Date/Date.hpp"

UploadQueueSchema::UploadQueueSchema(const mongocxx::database &database)
    : TemplateSchema(database, "uploadQueue", "uploadQueueFiles")
{
}

void UploadQueueSchema::uploadFile(const string &fileId, const string &diskId, const string &content)
{
    std::istringstream ss(content);
    std::istream fileStream(ss.rdbuf());
    mongocxx::v_noabi::options::gridfs::upload options;

    options.metadata(makeDocument(makeField("diskId", diskId), makeField("createdAt", Date().toBSON())));

    const auto &result = this->_fileBucket->upload_from_stream(fileId, &fileStream, options);
}