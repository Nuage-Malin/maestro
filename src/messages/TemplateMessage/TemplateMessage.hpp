/**
 * @file TemplateMessage.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief Template message declaration to abstract protobuf messages
 */

#ifndef MAESTRO_MESSAGE_TEMPLATEMESSAGE_HPP
#define MAESTRO_MESSAGE_TEMPLATEMESSAGE_HPP

#include <google/protobuf/message.h>
#include <bsoncxx/builder/stream/document.hpp>

#include "utils.hpp"

template <typename T> class TemplateMessage {
    static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T must extend google::protobuf::Message");

  public:
    TemplateMessage(const T &message);
    TemplateMessage(const bsoncxx::v_noabi::document::view &view);

    virtual T toProtobuf() const = 0;

    virtual TemplateMessage<T> &operator=(const T &message) = 0;

  protected:
    virtual void _validation() const = 0;
    bool _isValidFilename(const string &filename) const;
    bool _isValidDirectory(const string &directory) const;
};

#endif