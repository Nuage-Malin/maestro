/**
 * @file StatsDiskWakeupSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 21/02/2023
 * @copyright Nuage Malin
 */

#include "StatsDiskWakeupSchema.hpp"

StatsDiskWakeupSchema::StatsDiskWakeupSchema(const mongocxx::database &database) : TemplateSchema(database, "diskWakeup")
{
}

uint64 StatsDiskWakeupSchema::getConsumption(std::vector<MongoCXX::ObjectId> ids)
{
    MongoCXX::Document filter = makeDocument(
        makeField("_id", makeDocument(makeField("$in", MongoCXX::toArray(ids)))),
        makeField("periodInfo", makeDocument(makeField("$ne", MongoCXX::Null{})))
    );
    mongocxx::options::find options;

    options.projection(
        makeDocument(makeField("_id", false), makeField("periodInfo", makeDocument(makeField("consumption", true)))).view()
    );
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);
    uint64 consumption = 0;

    for (const auto &document : cursor) {
        consumption += static_cast<uint64>(document["periodInfo"]["consumption"].get_int64().value);
    }
    return consumption;
}
