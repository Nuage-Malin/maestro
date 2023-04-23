/**
 * @file DownloadedStackSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 */

#include "DownloadedStackSchema.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

FilesDownloadedStackSchema::FilesDownloadedStackSchema(const mongocxx::database &database, const EventsManager &events)
    : TemplateFileBucket(database, "downloadedStack"), _events(events)
{
}

void FilesDownloadedStackSchema::pushFile(const string &fileId, const Date &expirationDate, const string &content)
{
    const std::istringstream ss(content);
    std::istream fileStream(ss.rdbuf());
    mongocxx::v_noabi::options::gridfs::upload options;

    options.metadata(makeDocument(makeField("expirationDate", expirationDate.toBSON())));

    this->_fileBucket.upload_from_stream(fileId, &fileStream, options);
}

NODISCARD string FilesDownloadedStackSchema::downloadFile(const string &fileId)
{
    const std::ostringstream oss("");
    std::ostream ostream(oss.rdbuf());
    mongocxx::cursor cursor = this->_fileBucket.find(makeDocument(makeField("filename", fileId)).view());

    this->_fileBucket.download_to_stream((*cursor.begin())["_id"].get_value(), &ostream);
    return oss.str();
}

void FilesDownloadedStackSchema::deleteExpiredFiles(const Date &expirationDate)
{
    const bsoncxx::document::value filter =
        makeDocument(makeField("metadata.expirationDate", makeDocument(makeField("$lt", expirationDate.toBSON()))));

    mongocxx::cursor cursor = this->_fileBucket.find(filter.view());

    for (const auto &file : cursor) {
        this->_events.emit<const string &, const Date &>(
            Event::DOWNLOADEDSTACK_FILE_EXPIRATION,
            file["filename"].get_string().value.to_string(),
            Date(file["metadata"]["expirationDate"].get_date())
        );
        const MongoCXX::ValueView &fileId = file["_id"].get_value();

        this->_fileBucket.delete_file(fileId);
    }
}

NODISCARD Date FilesDownloadedStackSchema::getFileExpirationDate(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("filename", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("metadata.expirationDate", true)));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view(), options);

    if (cursor.begin() == cursor.end())
        throw NotFoundException("File not found");
    return Date((*cursor.begin())["metadata"]["expirationDate"].get_date());
}

NODISCARD bool FilesDownloadedStackSchema::doesFileExist(const string &fileId)
{
    const bsoncxx::document::value filter = makeDocument(makeField("filename", fileId));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", true)));
    mongocxx::cursor cursor = this->_fileBucket.find(filter.view(), options);

    return cursor.begin() != cursor.end();
}