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
    const std::function<void(const string &)> &callback =
        std::bind(&FilesDownloadedStackSchema::_onFileExpiration, this, std::placeholders::_1);

    events.on<const string &>(Event::REMOVE_VAULT_CACHE_FILE, std::move(callback));
}

void FilesDownloadedStackSchema::add(const string &fileId, const Date &expirationDate)
{
    MongoCXX::Document file_expiration =
        makeDocument(makeField("fileId", fileId), makeField("expirationDate", expirationDate.toBSON()));

    this->_model.insert_one(file_expiration.view());
}

void FilesDownloadedStackSchema::deleteFile(const string &fileId)
{
    std::cout << "OK 1" << std::endl;
    std::cout << "fileId: " << fileId << std::endl;
    const bsoncxx::document::value filter = makeDocument(makeField("fileId", fileId));

    std::cout << "OK 2" << std::endl;
    this->_model.delete_one(filter.view());
    std::cout << "OK 3" << std::endl;
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

void FilesDownloadedStackSchema::_onFileExpiration(const string &fileId)
{
    std::cout << "BEFORE deleteFile" << std::endl;
    std::cout << "fileId: " << fileId << std::endl;
    this->deleteFile(fileId);
    std::cout << "AFTER deleteFile" << std::endl;
}