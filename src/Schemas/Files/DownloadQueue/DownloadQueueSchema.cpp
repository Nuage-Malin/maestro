/**
 * @file DownloadQueueSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 12/04/2023
 * @copyright Nuage Malin
 */

#include "Utils/Date/Date.hpp"
#include "DownloadQueueSchema.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

FilesDownloadQueueSchema::FilesDownloadQueueSchema(const mongocxx::database &database) : TemplateSchema(database, "downloadQueue")
{
}

void FilesDownloadQueueSchema::add(const string &fileId, const string &userId, const string &diskId)
{
    MongoCXX::Document filter = makeDocument(
        makeField("fileId", fileId),
        makeField("userId", userId),
        makeField("diskId", diskId),
        makeField("requestedAt", Date().toBSON())
    );

    this->_model.insert_one(filter.view());
}

void FilesDownloadQueueSchema::remove(const MongoCXX::ObjectId &id)
{
    MongoCXX::Document filter = makeDocument(makeField("_id", id));

    this->_model.delete_one(filter.view());
}

Date FilesDownloadQueueSchema::getRequestedDate(const string &fileId, const string &diskId)
{
    MongoCXX::Document filter = makeDocument(makeField("fileId", fileId), makeField("diskId", diskId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("requestedAt", true)));
    options.sort(makeDocument(makeField("requestedAt", -1)));
    const auto result = this->_model.find_one(filter.view(), options);

    if (result)
        return Date((*result)["requestedAt"].get_date());
    throw NotFoundException("File not found");
}
