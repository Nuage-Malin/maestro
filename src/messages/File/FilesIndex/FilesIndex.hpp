/**
 * @file FilesIndex.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/01/2023
 * @copyright Nuage Malin
 * @brief FilesIndex class declaration
 */

#ifndef MAESTRO_MESSAGE_FILESINDEX_HPP
#define MAESTRO_MESSAGE_FILESINDEX_HPP

#include "messages/File/FileMetadata/FileMetadata.hpp"

class FilesIndex : public TemplateMessage<File::FilesIndex> {
  public:
    FilesIndex(const File::FilesIndex &message);
    FilesIndex(const bsoncxx::v_noabi::document::view &view);

    File::FilesIndex toProtobuf() const override;

    // Protobuf fields
    std::vector<FileMetadata> index;

    FilesIndex &operator=(const File::FilesIndex &message) override;

  protected:
    void _validation() const override;
};

#endif