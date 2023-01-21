/**
 * @file FileApproxMetadata.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief FileApproxMetadata class declaration
 */

#ifndef MAESTRO_MESSAGE_FILEAPPROXMETADATA_HPP
#define MAESTRO_MESSAGE_FILEAPPROXMETADATA_HPP

#include "common/File.grpc.pb.h"

#include "messages/TemplateMessage.hpp"

class FileApproxMetadata : public TemplateMessage<File::FileApproxMetadata> {
  public:
    FileApproxMetadata(const File::FileApproxMetadata &message);
    FileApproxMetadata(const bsoncxx::v_noabi::document::view &view);

    void toProtobuf(File::FileApproxMetadata &message) const override;
    File::FileApproxMetadata *toProtobuf() const override;

    // Protobuf fields
    string name;
    string dirPath;
    string userId;

    FileApproxMetadata &operator=(const File::FileApproxMetadata &message) override;

  protected:
    void _validation() const override;
};

#endif