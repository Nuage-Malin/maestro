/**
 * @file RemoveQueueSchema.cpp
 * @author Arthur Jourdan (arthur.jourdan@epitech.eu)
 * @date 08/05/2023
 * @copyright Nuage Malin
 */

#include <sstream>
#include <concepts>

#include "RemoveQueueSchema.hpp"

FilesRemoveQueueSchema::FilesRemoveQueueSchema(const mongocxx::database &database) : TemplateSchema(database, "removeQueue")
{
}

void FilesRemoveQueueSchema::add(const string &diskId, const string &fileId)
{
    const MongoCXX::Document &fileMetadata = makeDocument(makeField("fileId", fileId), makeField("diskId", diskId));

    this->_model.insert_one(fileMetadata.view());
}

void FilesRemoveQueueSchema::deleteDiskFiles(const string &diskId)
{
    const MongoCXX::Document &filter = makeDocument(makeField("diskId", diskId));

    this->_model.delete_many(filter.view());
}

void FilesRemoveQueueSchema::deleteFile(const string &fileId)
{
    const MongoCXX::Document &filter = makeDocument(makeField("fileId", fileId));

    this->_model.delete_one(filter.view());
}

NODISCARD std::vector<std::pair<string, string>> FilesRemoveQueueSchema::getDiskFiles(const string &diskId)
{
    const MongoCXX::Document filter = makeDocument(makeField("diskId", diskId));
    mongocxx::cursor cursor = this->_model.find(filter.view());
    std::vector result;

    for (const MongoCXX::DocumentView &file : cursor) {
        result.push_back(std::make_pair(file["fileId"].get_string().value.to_string(), file["userId"].get_string().value.to_string()));
    };
    return result;
}

NODISCARD std::unordered_set<string> FilesRemoveQueueSchema::getFilesDisk()
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

NODISCARD std::unordered_set<string> FilesRemoveQueueSchema::getFilesWithoutDisk()
{
    const MongoCXX::Document filter = makeDocument(makeField("diskId", makeDocument(makeField("$exists", false))));
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("fileId", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);
    std::unordered_set<string> files;

    for (const MongoCXX::DocumentView &file : cursor) {
        files.insert(file["fileId"].get_string().value.to_string());
    };
    return files;
}