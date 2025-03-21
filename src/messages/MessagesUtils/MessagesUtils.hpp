/**
 * @file messagesUtils.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief Global functions useful for messages
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
 * @brief Check if the filename is valid
 *
 * @param filename
 * @return true
 * @return false
 */
NODISCARD bool isValidFilename(const string &filename);
/**
 * @brief Check if the directory is valid
 *
 * @param directory
 * @return true
 * @return false
 */
NODISCARD bool isValidDirectory(const string &directory);