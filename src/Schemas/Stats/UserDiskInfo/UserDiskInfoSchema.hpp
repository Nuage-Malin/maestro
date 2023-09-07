/**
 * @file UserDiskInfoSchema.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Schema who interact with the stats database
 */

#ifndef MAESTRO_STATS_USERDISKINFO_SCHEMA_HPP
#define MAESTRO_STATS_USERDISKINFO_SCHEMA_HPP

#include <mongocxx/database.hpp>

#include "Utils/Date/Date.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

class StatsUserDiskInfoSchema : public TemplateSchema {
  public:
    StatsUserDiskInfoSchema(const mongocxx::database &database);
    ~StatsUserDiskInfoSchema() = default;

    uint64 getUserConsumption(const string &userId, const std::optional<Date> &startDate, const Date &endDate);
    uint64 getUserDiskSpace(const string &userId, const Date endDate);
};

#endif