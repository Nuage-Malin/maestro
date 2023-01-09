/**
 * @file FileMetadata.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief FileMetadata class declaration
 */

#ifndef MAESTRO_MESSAGE_FILEMETADATA_HPP
#define MAESTRO_MESSAGE_FILEMETADATA_HPP

#include "messages/File/FileApproxMetadata/FileApproxMetadata.hpp"

class FileMetadata : public TemplateMessage<File::FileMetadata> {
  public:
    FileMetadata(const File::FileMetadata &message);
    FileMetadata(const bsoncxx::v_noabi::document::view &view);
    FileMetadata(const bsoncxx::v_noabi::document::view &view, const bool isDownloadable);

    void toProtobuf(File::FileMetadata &message) const override;
    File::FileMetadata *toProtobuf() const override;

    // Protobuf fields
    FileApproxMetadata approxMetadata;
    string fileId;
    string lastEditorId;
    google::protobuf::Timestamp creation;
    google::protobuf::Timestamp lastEdit;
    bool isDownloadable{false};

    FileMetadata &operator=(const File::FileMetadata &message) override;

  protected:
    void _validation() const override;
};

#endif