/**
 * @file FileApproxMetadata.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief FileApproxMetadata class implementation
 */

#include "messages/MessagesUtils/MessagesUtils.hpp"
#include "FileApproxMetadata.hpp"

FileApproxMetadata::FileApproxMetadata(const File::FileApproxMetadata &message)
    : TemplateMessage(message), name(message.name()), dirPath(message.dirpath()), userId(message.userid())
{
}

FileApproxMetadata::FileApproxMetadata(const bsoncxx::v_noabi::document::view &view)
    : TemplateMessage(view), name(view["filename"].get_string().value.to_string()),
      dirPath(view["metadata.dirPath"].get_string().value.to_string()),
      userId(view["metadata.userId"].get_string().value.to_string())
{
}

void FileApproxMetadata::toProtobuf(File::FileApproxMetadata &message) const
{
    message.set_name(this->name);
    message.set_dirpath(this->dirPath);
    message.set_userid(this->userId);
}

FileApproxMetadata &FileApproxMetadata::operator=(const File::FileApproxMetadata &message)
{
    this->name = message.name();
    this->dirPath = message.dirpath();
    this->userId = message.userid();

    this->_validation();
    return *this;
}

void FileApproxMetadata::_validation() const
{
    if (!this->_isValidFilename(this->name))
        throw std::invalid_argument("[FileApproxMetadata] Invalid name: " + this->name);
    if (!this->_isValidDirectory(this->dirPath))
        throw std::invalid_argument("[FileApproxMetadata] Invalid dirPath: " + this->dirPath);
    try {
        toObjectId(this->userId);
    } catch (...) {
        throw std::invalid_argument("[FileApproxMetadata] Invalid userId: " + this->userId);
    }
}
