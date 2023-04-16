/**
 * @file StatsUserDiskInfoSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "schemas.hpp"
#include "StatsUserDiskInfoSchema.hpp"
#include "Schemas/Stats/DiskWakeup/StatsDiskWakeupSchema.hpp"

StatsUserDiskInfoSchema::StatsUserDiskInfoSchema(const mongocxx::database &database) : TemplateSchema(database, "userDiskInfo")
{
}

uint64 StatsUserDiskInfoSchema::getUserConsumption(const string &userId, const Date &startDate, const Date &endDate)
{
    mongocxx::pipeline pipeline;

    pipeline.match(makeDocument(
        makeField("userId", userId),
        makeField("createdAt", makeDocument(makeField("$gte", startDate.toBSON()), makeField("$lte", endDate.toBSON())))
    ));
    pipeline.lookup(makeDocument(
        makeField("from", "diskWakeup"),
        makeField("localField", "diskWakeup"),
        makeField("foreignField", "_id"),
        makeField("as", "diskWakeup")
    ));
    pipeline.unwind("$diskWakeup");
    pipeline.group(makeDocument(
        makeField("_id", MongoCXX::Null()),
        makeField("consumption", makeDocument(makeField("$sum", "$diskWakeup.periodInfo.consumption")))
    ));

    mongocxx::cursor cursor = this->_model.aggregate(pipeline);

    if (cursor.begin() == cursor.end())
        return 0;

    const MongoCXX::DocumentElement &consumptionValue = (*cursor.begin())["consumption"];

    if (consumptionValue.type() == bsoncxx::type::k_int32)
        return static_cast<uint64>(consumptionValue.get_int32().value);
    else if (consumptionValue.type() == bsoncxx::type::k_int64)
        return consumptionValue.get_int64().value;
    else
        throw std::runtime_error("Invalid consumption type");
}

uint64 StatsUserDiskInfoSchema::getUserDiskSpace(const string &userId, const Date $endDate)
{
    mongocxx::pipeline pipeline;

    pipeline.match(
        makeDocument(makeField("userId", userId), makeField("createdAt", makeDocument(makeField("$lte", $endDate.toBSON()))))
    );
    pipeline.sort(makeDocument(makeField("diskId", 1), makeField("createdAt", -1)));
    pipeline.group(
        makeDocument(makeField("_id", "$diskId"), makeField("userDiskSpace", makeDocument(makeField("$first", "$usedMemory"))))
    );
    pipeline.group(makeDocument(
        makeField("_id", MongoCXX::Null()), makeField("totalUserDiskSpace", makeDocument(makeField("$sum", "$userDiskSpace")))
    ));

    mongocxx::cursor cursor = this->_model.aggregate(pipeline);

    if (cursor.begin() == cursor.end())
        return 0;

    const MongoCXX::DocumentElement &totalUserDiskSpace = (*cursor.begin())["totalUserDiskSpace"];

    if (totalUserDiskSpace.type() == bsoncxx::type::k_int32)
        return static_cast<uint64>(totalUserDiskSpace.get_int32().value);
    else if (totalUserDiskSpace.type() == bsoncxx::type::k_int64)
        return totalUserDiskSpace.get_int64().value;
    else
        throw std::runtime_error("Invalid totalUserDiskSpace type");
}