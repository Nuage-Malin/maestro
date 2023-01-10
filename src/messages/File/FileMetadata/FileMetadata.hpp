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
    /**
     * @brief Construct a new File Metadata object
     *
     * @param view
     * @param isDownloadable {bool} Can be check with FileServer::_isDownloadable
     */
    FileMetadata(const bsoncxx::v_noabi::document::view &view, const bool isDownloadable = false);

    void toProtobuf(File::FileMetadata &message) const override;
    File::FileMetadata *toProtobuf() const override;

    // Protobuf fields
    FileApproxMetadata approxMetadata;
    string fileId;
    bool isDownloadable;
    string lastEditorId;
    google::protobuf::Timestamp creation;
    google::protobuf::Timestamp lastEdit;

    FileMetadata &operator=(const File::FileMetadata &message) override;

  protected:
    void _validation() const override;
};

#endif