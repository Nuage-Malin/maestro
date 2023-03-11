/**
 * @file StatsUserDiskInfoSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Schema who interact with the stats database
 */

#ifndef MAESTRO_STATS_DISKWAKEUP_SCHEMA_HPP
#define MAESTRO_STATS_DISKWAKEUP_SCHEMA_HPP

#include <mongocxx/database.hpp>

#include "Utils/Date/Date.hpp"
#include "Schemas/Template/TemplateSchema.hpp"

class StatsDiskWakeupSchema : public TemplateSchema {
  public:
    StatsDiskWakeupSchema(const mongocxx::database &database);
    ~StatsDiskWakeupSchema() = default;
};

#endif