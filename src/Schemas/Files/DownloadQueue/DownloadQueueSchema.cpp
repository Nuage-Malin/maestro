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

void FilesDownloadQueueSchema::deleteDiskFiles(const string &diskId)
{
    MongoCXX::Document filter = makeDocument(makeField("diskId", diskId));

    this->_model.delete_many(filter.view());
}

void FilesDownloadQueueSchema::deleteFile(const string &fileId)
{
    MongoCXX::Document filter = makeDocument(makeField("fileId", fileId));

    this->_model.delete_one(filter.view());
}

void FilesDownloadQueueSchema::deleteUser(const string &userId)
{
    MongoCXX::Document filter = makeDocument(makeField("userId", userId));

    this->_model.delete_many(filter.view());
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

NODISCARD std::vector<std::pair<string, string>> FilesDownloadQueueSchema::getDiskFiles(const string &diskId)
{
    const MongoCXX::Document filter = makeDocument(makeField("diskId", diskId));
    mongocxx::cursor cursor = this->_model.find(filter.view());
    std::vector<std::pair<string, string>> result;

    for (const MongoCXX::DocumentView &file : cursor) {
        result.push_back(std::make_pair(file["fileId"].get_string().value.to_string(), file["userId"].get_string().value.to_string()));
    };
    return result;
}

NODISCARD std::unordered_set<string> FilesDownloadQueueSchema::getFilesDisk()
{
    const MongoCXX::Document filter = makeDocument();
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("diskId", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);
    std::unordered_set<string> disks;

    for (const MongoCXX::DocumentView &file : cursor) {
        disks.insert(file["diskId"].get_string().value.to_string());
    };
    return disks;
}

NODISCARD bool FilesDownloadQueueSchema::doesFileExist(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("fileId", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);

    return cursor.begin() != cursor.end();
}