/**
 * @file TemplateFileBucket.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 17/03/2023
 * @copyright Nuage Malin
 */

#include "TemplateFileBucket.hpp"

TemplateFileBucket::TemplateFileBucket(const mongocxx::database &database, const string &fileBucketName)
{
    mongocxx::options::gridfs::bucket bucketOptions;

    bucketOptions.bucket_name(fileBucketName);
    this->_fileBucket = database.gridfs_bucket(bucketOptions);
}