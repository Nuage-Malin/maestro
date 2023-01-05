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
    // Check if T is a protobuf message
    static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T must extend google::protobuf::Message");

  public:
    /**
     * @brief Constructor from a protobuf message
     *
     * @param message
     */
    TemplateMessage(const T &message);
    /**
     * @brief Constructor from a MongoDB document
     *
     * @param view
     */
    TemplateMessage(const bsoncxx::v_noabi::document::view &view);

    /**
     * @brief Convert the class into a protobuf message
     *
     * @param message
     */
    virtual void toProtobuf(T &message) const = 0;
    /**
     * @brief Convert the class into a protobuf message
     * @attention The other toProtobuf method should be prioritized.
     *  This one create an allocated message and call the other method before returning it.
     *  This method should be used with protobuf .set_allocated_*() methods.
     *
     * @return T&
     */
    T &toProtobuf() const;

    virtual TemplateMessage<T> &operator=(const T &message) = 0;

  protected:
    /**
     * @brief Validate the message
     */
    virtual void _validation() const = 0;
    /**
     * @brief Check if the filename is valid
     *
     * @param filename
     * @return true
     * @return false
     */
    bool _isValidFilename(const string &filename) const;
    /**
     * @brief Check if the directory is valid
     *
     * @param directory
     * @return true
     * @return false
     */
    bool _isValidDirectory(const string &directory) const;
};

#endif