/**
 * @file TemplateSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "TemplateSchema.hpp"

TemplateSchema::TemplateSchema(
    const mongocxx::database &database, const string &collectionName, const std::optional<string> &fileBucketName
)
    : _model(database[collectionName])
{
    if (fileBucketName.has_value())
        this->_setFileBucket(database, *fileBucketName);
}

void TemplateSchema::_setFileBucket(const mongocxx::database &database, const string &fileBucketName)
{
    mongocxx::options::gridfs::bucket bucketOptions;

    bucketOptions.bucket_name(fileBucketName);
    this->_fileBucket = database.gridfs_bucket(bucketOptions);
}