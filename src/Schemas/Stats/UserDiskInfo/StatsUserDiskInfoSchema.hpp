/**
 * @file StatsUserDiskInfoSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Schema who interact with the stats database
 */

#ifndef MAESTRO_STATS_USERDISKINFO_SCHEMA_HPP
#define MAESTRO_STATS_USERDISKINFO_SCHEMA_HPP

#include <mongocxx/database.hpp>

#include "Utils/Date/Date.hpp"
#include "Schemas/Template/TemplateSchema.hpp"

class StatsUserDiskInfoSchema : public TemplateSchema {
  public:
    StatsUserDiskInfoSchema(const mongocxx::database &database);
    ~StatsUserDiskInfoSchema() = default;

    uint64 getUserConsumption(const string &userId, const Date &startDate, const Date &endDate);

  private:
    const mongocxx::database &_database;
};

#endif