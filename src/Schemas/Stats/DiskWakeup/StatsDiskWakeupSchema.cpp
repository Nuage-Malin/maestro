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

uint64 StatsDiskWakeupSchema::getConsumption(const MongoCXX::ArrayView &ids)
{
    MongoCXX::Document filter = makeDocument(
        makeField("_id", makeDocument(makeField("$in", ids))),
        makeField("periodInfo", makeDocument(makeField("$ne", MongoCXX::Null{})))
    );
    mongocxx::options::find options;

    options.projection(makeDocument(makeField("_id", false), makeField("periodInfo.consumption", true)));
    mongocxx::cursor cursor = this->_model.find(filter.view(), options);
    uint64 consumption = 0;

    for (const auto &document : cursor) {
        const auto &docValue = document["periodInfo"]["consumption"];

        if (docValue.type() == bsoncxx::type::k_int32)
            consumption += static_cast<uint64>(docValue.get_int32().value);
        else if (docValue.type() == bsoncxx::type::k_int64)
            consumption += static_cast<uint64>(docValue.get_int64().value);
        else
            throw std::runtime_error("Invalid consumption type");
    }
    return consumption;
}
