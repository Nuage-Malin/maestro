/**
 * @file MessagesUtils.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief Messages utils implementation
 */

#include "MessagesUtils.hpp"

bsoncxx::oid toObjectId(const string &id)
{
    try {
        return bsoncxx::oid{bsoncxx::stdx::string_view{id}};
    } catch (...) {
        throw std::invalid_argument("[TemplateMessage] Invalid ObjectId: " + id);
    }
}

google::protobuf::Timestamp convertTimestamp(const bsoncxx::v_noabi::types::b_timestamp &mongoTimestamp)
{
    google::protobuf::Timestamp protobufTimestamp;

    protobufTimestamp.set_seconds(mongoTimestamp.timestamp);
    protobufTimestamp.set_nanos(mongoTimestamp.increment * 1000);
    return protobufTimestamp;
}
