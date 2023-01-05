/**
 * @file FileApproxMetadata.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief FileApproxMetadata class implementation
 */

#include "FileApproxMetadata.hpp"

FileApproxMetadata::FileApproxMetadata(const File::FileApproxMetadata &message)
    : TemplateMessage(message), name(message.name()), dirPath(message.dirpath()), userId(message.userid())
{
}

FileApproxMetadata::FileApproxMetadata(const bsoncxx::v_noabi::document::view &view)
    : TemplateMessage(view), name(view["name"].get_string().value.to_string()),
      dirPath(view["dirPath"].get_string().value.to_string()), userId(view["userId"].get_string().value.to_string())
{
}

File::FileApproxMetadata FileApproxMetadata::toProtobuf() const
{
    File::FileApproxMetadata fileApproxMetadata;

    fileApproxMetadata.set_name(this->name);
    fileApproxMetadata.set_dirpath(this->dirPath);
    fileApproxMetadata.set_userid(this->userId);
    return fileApproxMetadata;
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
        this->toObjectId(this->userId);
    } catch (...) {
        throw std::invalid_argument("[FileApproxMetadata] Invalid userId: " + this->userId);
    }
}
