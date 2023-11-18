/**
 * @file DownloadedStackSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 * @brief Keep track of expiration date for downloadable files
 *        (in other words, knows until what datetime files are downloadable)
 */

#include <sstream>

#include "DownloadedStackSchema.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

FilesDownloadedStackSchema::FilesDownloadedStackSchema(const mongocxx::database &database, EventsManager &events)
    : TemplateSchema(database, "downloadedStack")
{
    const std::function<void(const string &, const Date &)> &callback =
        std::bind(&FilesDownloadedStackSchema::_onFileExpiration, this, std::placeholders::_1, std::placeholders::_2);

    events.on<const string &, const Date &>(Event::DOWNLOADEDSTACK_FILE_EXPIRATION, std::move(callback));
}

void FilesDownloadedStackSchema::add(const string &fileId, const Date &expirationDate)
{
    MongoCXX::Document file_expiration =
        makeDocument(makeField("fileId", fileId), makeField("expirationDate", expirationDate.toBSON()));

    this->_model.insert_one(file_expiration.view());
}

void FilesDownloadedStackSchema::deleteFile(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("fileId", fileId));

    this->_model.delete_one(filter.view());
}

NODISCARD std::vector<DownloadedStack> FilesDownloadedStackSchema::getExpiredFiles(const Date &expirationDate)
{
    const bsoncxx::document::value filter =
        makeDocument(makeField("expirationDate", makeDocument(makeField("$lt", expirationDate.toBSON()))));

    mongocxx::cursor cursor = this->_model.find(filter.view());
    std::vector<DownloadedStack> files{};

    for (const auto &file : cursor) {
        DownloadedStack downloadedStackFile = {
            .fileId = file["fileId"].get_string().value.to_string(), .expirationDate = Date(file["expirationDate"].get_date())};
        files.push_back(downloadedStackFile);
    }
    return files;
}

NODISCARD bool FilesDownloadedStackSchema::doesFileExist(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("fileId", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);

    return cursor.begin() != cursor.end();
}

void FilesDownloadedStackSchema::_onFileExpiration(const string &fileId, UNUSED const Date &expirationDate)
{
    this->deleteFile(fileId);
}