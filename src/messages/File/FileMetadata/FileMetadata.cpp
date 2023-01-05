/**
 * @file FileMetadata.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief FileMetadata class implementation
 */

#include "messages/MessagesUtils/MessagesUtils.hpp"
#include "FileMetadata.hpp"

FileMetadata::FileMetadata(const File::FileMetadata &message)
    : TemplateMessage(message), approxMetadata(message.approxmetadata()), fileId(message.fileid()),
      lastEditorId(message.lasteditorid()), creation(message.creation()), lastEdit(message.lastedit())
{
}

FileMetadata::FileMetadata(const bsoncxx::v_noabi::document::view &view)
    : TemplateMessage(view), approxMetadata(view), fileId(view["_id"].get_string().value.to_string()),
      lastEditorId(view["metadata.lastEditorId"].get_string().value.to_string()),
      creation(convertTimestamp(view["metadata.creation"].get_timestamp())),
      lastEdit(convertTimestamp(view["metadata.lastEdit"].get_timestamp()))
{
}

void FileMetadata::toProtobuf(File::FileMetadata &message) const
{
    message.set_allocated_approxmetadata(&this->approxMetadata.toProtobuf());
    message.set_fileid(this->fileId);
    message.set_lasteditorid(this->lastEditorId);
    message.set_allocated_creation(new google::protobuf::Timestamp(this->creation));
    message.set_allocated_lastedit(new google::protobuf::Timestamp(this->lastEdit));
}

FileMetadata &FileMetadata::operator=(const File::FileMetadata &message)
{
    this->approxMetadata = message.approxmetadata();
    this->fileId = message.fileid();
    this->lastEditorId = message.lasteditorid();
    this->creation = message.creation();
    this->lastEdit = message.lastedit();

    this->_validation();
    return *this;
}

void FileMetadata::_validation() const
{
    try {
        toObjectId(this->fileId);
    } catch (...) {
        throw std::invalid_argument("[FileMetadata] Invalid fileId: " + this->fileId);
    }
    try {
        toObjectId(this->lastEditorId);
    } catch (...) {
        throw std::invalid_argument("[FileMetadata] Invalid lastEditorId: " + this->lastEditorId);
    }
}
