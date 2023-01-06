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

bool isValidFilename(const string &filename)
{
    return !filename.empty() && filename.find_first_not_of("/\t\r\n ") != string::npos;
}

bool isValidDirectory(const string &directory)
{
    return directory[0] == '/' && directory[directory.size() - 1] == '/' && directory.find("//") == string::npos;
}