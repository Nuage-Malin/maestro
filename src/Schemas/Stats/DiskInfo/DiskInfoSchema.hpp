/**
 * @file DiskInfoSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/09/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_STATS_DISKWAKEUP_SCHEMA_HPP
#define MAESTRO_STATS_DISKWAKEUP_SCHEMA_HPP

#include <mongocxx/database.hpp>

#include "Utils/Date/Date.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

class StatsDiskInfoSchema : public TemplateSchema {
  public:
    StatsDiskInfoSchema(const mongocxx::database &database);
    ~StatsDiskInfoSchema() = default;

    uint64 getTotalDisksSpace(const Date &endDate);
};

#endif