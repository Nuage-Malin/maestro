/**
 * @file DiskInfoSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/09/2023
 * @copyright Nuage Malin
 */

#include "schemas.hpp"
#include "DiskInfoSchema.hpp"

StatsDiskInfoSchema::StatsDiskInfoSchema(const mongocxx::database &database) : TemplateSchema(database, "diskInfo")
{
}

uint64 StatsDiskInfoSchema::getTotalDisksSpace(const Date & endDate)
{
    mongocxx::pipeline pipeline;

    pipeline.match(
        makeDocument(makeField("createdAt", makeDocument(makeField("$lte", endDate.toBSON()))))
    );
    pipeline.sort(makeDocument(makeField("diskId", 1), makeField("createdAt", -1)));
    pipeline.group(
        makeDocument(makeField("_id", "$diskId"), makeField("diskSpace", makeDocument(makeField("$first", "$totalMemory"))))
    );
    pipeline.group(makeDocument(
        makeField("_id", MongoCXX::Null()), makeField("totalDisksSpace", makeDocument(makeField("$sum", "$diskSpace")))
    ));

    mongocxx::cursor cursor = this->_model.aggregate(pipeline);

    if (cursor.begin() == cursor.end())
        return 0;

    const MongoCXX::DocumentElement &totalDisksSpace = (*cursor.begin())["totalDisksSpace"];

    if (totalDisksSpace.type() == bsoncxx::type::k_int32)
        return static_cast<uint64>(totalDisksSpace.get_int32().value);
    else if (totalDisksSpace.type() == bsoncxx::type::k_int64)
        return totalDisksSpace.get_int64().value;
    else
        throw std::runtime_error("Invalid totalDisksSpace type");
}