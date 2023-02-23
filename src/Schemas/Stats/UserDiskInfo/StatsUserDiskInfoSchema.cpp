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

    options.projection(makeDocument(makeField("_id", false), makeField("diskWakeup", true)).view());
    mongocxx::cursor userDiskInfoCursor = this->_model.find(filter.view(), options);
    const auto &userDiskInfoBegin = userDiskInfoCursor.begin();
    const auto &userDiskInfoEnd = userDiskInfoCursor.end();
    std::vector<MongoCXX::ObjectId> diskWakeupIds;

    diskWakeupIds.resize(std::distance(userDiskInfoBegin, userDiskInfoEnd));
    std::transform(userDiskInfoBegin, userDiskInfoEnd, diskWakeupIds.begin(), [](const MongoCXX::DocumentView &document) {
        return document["diskWakeup"].get_oid().value;
    });
    return StatsDiskWakeupSchema(this->_database).getConsumption(diskWakeupIds);
}