/**
 * @file TemplateSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Schema used as a base for all schemas
 */

#ifndef MAESTRO_TEMPLATE_SCHEMA_HPP
#define MAESTRO_TEMPLATE_SCHEMA_HPP

#include <optional>

#include <mongocxx/database.hpp>

#include "utils.hpp"

class TemplateSchema {
  public:
    TemplateSchema(const mongocxx::database &database, const string &collectionName);
    ~TemplateSchema() = default;

  protected:
    mongocxx::collection _model;
};

#endif