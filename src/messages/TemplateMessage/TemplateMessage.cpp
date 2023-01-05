/**
 * @file TemplateMessage.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief Template message implementation to abstract protobuf messages
 */

#include <bsoncxx/oid.hpp>
#include "TemplateMessage.hpp"

template <typename T> TemplateMessage<T>::TemplateMessage(const T &message)
{
    this->_validation();
}

template <typename T> TemplateMessage<T>::TemplateMessage(const bsoncxx::v_noabi::document::view &view)
{
    this->_validation();
}

template <typename T> bool TemplateMessage<T>::_isValidFilename(const string &filename) const
{
    return !filename.empty() && filename.find_first_not_of("/\t\r\n ") != string::npos;
}

template <typename T> bool TemplateMessage<T>::_isValidDirectory(const string &directory) const
{
    return directory[0] == '/' && directory[directory.size() - 1] == '/' && directory.find("//") == string::npos;
}