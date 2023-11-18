/**
 * @file DownloadedStackSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 */

#include <sstream>

#include "DownloadedStackSchema.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

FilesDownloadedStackSchema::FilesDownloadedStackSchema(const mongocxx::database &database, const EventsManager &events)
    : TemplateSchema(database, "downloadedStack"), _events(events)
{
}

void FilesDownloadedStackSchema::add(const string &fileId, const Date &expirationDate)
{
    MongoCXX::Document file_expiration =
        makeDocument(makeField("fileId", fileId), makeField("expirationDate", expirationDate.toBSON()));

    this->_model.insert_one(file_expiration.view());
}

NODISCARD std::vector<string> FilesDownloadedStackSchema::deleteExpiredFiles(const Date &expirationDate)
{
    const bsoncxx::document::value filter =
        makeDocument(makeField("expirationDate", makeDocument(makeField("$lt", expirationDate.toBSON()))));

    mongocxx::cursor cursor = this->_model.find(filter.view());
    std::vector<string> fileIds{};

    for (const auto &file : cursor) {
        this->_events.emit<const string &, const Date &>(
            Event::DOWNLOADEDSTACK_FILE_EXPIRATION,
            file["fileId"].get_string().value.to_string(),
            Date(file["expirationDate"].get_date())
        );
        fileIds.push_back(file["_id"].get_oid().value.to_string());
    }
    // todo remove files from mongoDB
    // todo remove files from vault cache ?
    return fileIds;
}

NODISCARD Date FilesDownloadedStackSchema::getFileExpirationDate(const string &fileId)
{
    /*todo remove cause useless ? */
    const bsoncxx::document::value filter = makeDocument(makeField("filename", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("metadata.expirationDate", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);

    if (cursor.begin() == cursor.end())
        throw NotFoundException("File not found");
    return Date((*cursor.begin())["metadata"]["expirationDate"].get_date());
}

NODISCARD bool FilesDownloadedStackSchema::doesFileExist(const string &fileId)
{
    /*todo*/
    const bsoncxx::document::value filter = makeDocument(makeField("filename", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);

    return cursor.begin() != cursor.end();
}