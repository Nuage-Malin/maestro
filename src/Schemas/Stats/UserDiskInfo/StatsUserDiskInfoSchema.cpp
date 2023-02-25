/**
 * @file StatsUserDiskInfoSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "StatsUserDiskInfoSchema.hpp"
#include "Schemas/Stats/DiskWakeup/StatsDiskWakeupSchema.hpp"

StatsUserDiskInfoSchema::StatsUserDiskInfoSchema(const mongocxx::database &database)
    : TemplateSchema(database, "userDiskInfo"), _database(database)
{
}

uint64 StatsUserDiskInfoSchema::getUserConsumption(const string &userId, const Date &startDate, const Date &endDate)
{
    MongoCXX::Document filter = makeDocument(
        makeField("userId", userId),
        makeField("createdAt", makeDocument(makeField("$gte", startDate.toBSON()), makeField("$lte", endDate.toBSON())))
    );
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("diskWakeup", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);
    MongoCXX::ArrayBuilder diskWakeupIds;

    for (const auto &document : cursor) {
        diskWakeupIds.append(document["diskWakeup"].get_oid().value);
    }

    return StatsDiskWakeupSchema(this->_database).getConsumption(diskWakeupIds.view());
}