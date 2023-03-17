/**
 * @file TemplateFileBucket.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 17/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_TEMPLATE_FILEBUCKET_HPP
#define MAESTRO_TEMPLATE_FILEBUCKET_HPP

#include <mongocxx/database.hpp>

#include "utils.hpp"

class TemplateFileBucket {
  public:
    TemplateFileBucket(const mongocxx::database &database, const string &fileBucketName);
    ~TemplateFileBucket() = default;

  protected:
    mongocxx::gridfs::bucket _fileBucket;
};

#endif