/**
 * @file RemoveQueueSchema.cpp
 * @author Arthur Jourdan (arthur.jourdan@epitech.eu)
 * @date 08/05/2023
 * @copyright Nuage Malin
 */

#include <sstream>
#include <concepts>

#include "RemoveQueueSchema.hpp"

FilesRemoveQueueSchema::FilesRemoveQueueSchema(const mongocxx::database &database) : TemplateSchema(database, "downloadQueue")
{
}

void FilesRemoveQueueSchema::add(const string &diskId /*, const string &userId*/, const string &fileId)
{
    const MongoCXX::Document &fileMetadata =
        makeDocument(makeField("fileId", fileId), /*makeField("userId", userId),*/ makeField("diskId", diskId));

    this->_model.insert_one(fileMetadata.view());
}

void FilesRemoveQueueSchema::deleteDiskFiles(const string &diskId)
{
    const MongoCXX::Document &filter = makeDocument(makeField("diskId", diskId));

    this->_model.delete_many(filter.view());
}

NODISCARD Maestro_Vault::RemoveFilesRequest FilesRemoveQueueSchema::getDiskFiles(const string &diskId)
{
    const MongoCXX::Document filter = makeDocument(makeField("diskId", diskId));
    mongocxx::cursor cursor = this->_model.find(filter.view());
    Maestro_Vault::RemoveFilesRequest result;

    result.set_diskid(diskId);
    for (const MongoCXX::DocumentView &file : cursor) {
        result.add_fileid(file["fileId"].get_string().value.to_string());
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