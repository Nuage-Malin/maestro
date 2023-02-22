/**
 * @file utils.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief Utils commands
 */

#ifndef MAESTRO_UTILS_HPP
#define MAESTRO_UTILS_HPP

#include <cstdlib>
#include <iostream>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

typedef std::string string;
typedef std::size_t size_t;
typedef uint64_t uint64;
#define toString(str)  std::to_string(str)
#define toInteger(str) std::stoi(str)
#define toSize_t(str)  std::stoul(str)
#define getenv(env)    std::getenv(env)
#define UNUSED         __attribute__((unused))
#define NODISCARD      [[nodiscard]]

namespace MongoCXX
{
    typedef bsoncxx::oid ObjectId;
    typedef bsoncxx::document::value Document;
    typedef bsoncxx::builder::basic::sub_document SubDocument;
    typedef bsoncxx::document::view DocumentView;
    typedef bsoncxx::types::b_null Null;
    typedef bsoncxx::types::b_array Array;

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