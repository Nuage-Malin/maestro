/**
 * @file mongocxx.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_MONGOCXX_HPP
#define MAESTRO_MONGOCXX_HPP

#include <mongocxx/database.hpp>

namespace MongoCXX
{
    typedef bsoncxx::oid ObjectId;
    typedef bsoncxx::v_noabi::types::bson_value::view ValueView;
    typedef bsoncxx::document::value Document;
    typedef bsoncxx::builder::basic::sub_document SubDocument;
    typedef bsoncxx::document::view DocumentView;
    typedef bsoncxx::document::element DocumentElement;
    typedef bsoncxx::types::b_null Null;
    typedef bsoncxx::types::b_array Array;
    typedef bsoncxx::array::view ArrayView;
    typedef bsoncxx::builder::basic::array ArrayBuilder;

    template <typename T> bsoncxx::array::view toArray(std::vector<T> list)
    {
        bsoncxx::builder::basic::array bArray;

        for (const T &item : list) {
            bArray.append(item);
        }
        return bArray.view();
    }
}; // namespace MongoCXX

#define makeDocument bsoncxx::builder::basic::make_document
#define makeField    bsoncxx::builder::basic::kvp
#define makeArray    bsoncxx::builder::basic::array

#endif