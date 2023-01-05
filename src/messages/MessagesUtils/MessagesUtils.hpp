/**
 * @file messagesUtils.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief Messages utils
 */

#include <bsoncxx/builder/stream/document.hpp>
#include <google/protobuf/timestamp.pb.h>

#include "utils.hpp"

/**
 * @brief Convert a string to an ObjectId
 *
 * @param id ObjectId
 * @return bsoncxx::oid
 * @throw std::invalid_argument if id is not a valid ObjectId
 */
bsoncxx::oid toObjectId(const string &id);

/**
 * @brief Convert a bsoncxx::v_noabi::types::b_timestamp to a google::protobuf::Timestamp
 *
 * @param mongoTimestamp
 * @return google::protobuf::Timestamp
 */
google::protobuf::Timestamp convertTimestamp(const bsoncxx::v_noabi::types::b_timestamp &mongoTimestamp);

template <typename TTarget, typename TList> inline std::vector<TTarget> convertListIntoVector(const TList &list)
{
    const vector = std::vector(list.begin(), list.end());
    const result = std::vector<TTarget>(list.size());

    std::transform(vector.begin(), vector.end(), result.begin(), [](const auto &item) {
        return TTarget(item);
    });
    return result;
}