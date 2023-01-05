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
    : TemplateMessage(view), approxMetadata(view["approxMetadata"].get_document().value),
      fileId(view["fileId"].get_string().value.to_string()),
      lastEditorId(view["lastEditorId"].get_string().value.to_string()),
      creation(convertTimestamp(view["creation"].get_timestamp())),
      lastEdit(convertTimestamp(view["lastEdit"].get_timestamp()))
{
}

File::FileMetadata FileMetadata::toProtobuf() const
{
    File::FileMetadata message;

    message.set_allocated_approxmetadata(new File::FileApproxMetadata(this->approxMetadata.toProtobuf()));
    message.set_fileid(this->fileId);
    message.set_lasteditorid(this->lastEditorId);
    message.set_allocated_creation(new google::protobuf::Timestamp(this->creation));
    message.set_allocated_lastedit(new google::protobuf::Timestamp(this->lastEdit));

    return message;
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
