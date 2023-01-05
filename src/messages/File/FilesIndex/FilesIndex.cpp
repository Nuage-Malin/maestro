/**
 * @file FilesIndex.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief FilesIndex class implementation
 */

#include "messages/MessagesUtils/MessagesUtils.hpp"
#include "FilesIndex.hpp"

FilesIndex::FilesIndex(const File::FilesIndex &message)
    : TemplateMessage(message), index(convertListIntoVector<FileMetadata>(message.index()))
{
}

FilesIndex::FilesIndex(const bsoncxx::v_noabi::document::view &view)
    : TemplateMessage(view), index(convertListIntoVector<FileMetadata>(view["index"].get_array().value))
{
}

File::FilesIndex FilesIndex::toProtobuf() const
{
    File::FilesIndex message;

    std::transform(this->index.begin(),
        this->index.end(),
        google::protobuf::RepeatedPtrFieldBackInserter(message.mutable_index()),
        [](const FileMetadata &item) {
            return item.toProtobuf();
        });

    return message;
}

FilesIndex &FilesIndex::operator=(const File::FilesIndex &message)
{
    this->index = convertListIntoVector<FileMetadata>(message.index());

    this->_validation();
    return *this;
}

void FilesIndex::_validation() const
{
}
