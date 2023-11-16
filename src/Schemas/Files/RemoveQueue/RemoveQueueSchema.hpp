/**
 * @file RemoveQueueSchema.hpp
 * @author Arthur Jourdan (arthur.jourdan@epitech.eu)
 * @date 08/05/2023
 * @copyright Nuage Malin
 * @brief Stores (temporarly) file metadata of files to be removed from main storage (Vault)
 */

#ifndef MAESTRO_FILES_REMOVEQUEUE_SCHEMA_HPP
#define MAESTRO_FILES_REMOVEQUEUE_SCHEMA_HPP

#include "Maestro_Vault/Maestro_Vault.grpc.pb.h"

#include "mongocxx.hpp"
#include "Schemas/Templates/Schema/TemplateSchema.hpp"

class FilesRemoveQueueSchema : public TemplateSchema {
  public:
    FilesRemoveQueueSchema(const mongocxx::database &database);
    ~FilesRemoveQueueSchema() = default;

    /**
     * @brief Add file metadata to the queue of files to be removed from main storage (Vault)
     */
    void add(const string &diskId, const string &fileId);

    /**
     * @brief Add multiple file IDs to the queue
     * @tparam StrIterator Template with concept requirements, meaning that the type has to be iterable and hold a string
     *                     Basically it has to be a container containing a string
     * @param diskId Disk Id of files to be added
     * @param fileIdsBeg begin iterator of an iterator of file ids
     * @param fileIdsEnd end iterator of an iterator of file ids
     */
    template <typename StrIterator>
        requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
    void add(const string &diskId, const StrIterator &fileIdsBeg, const StrIterator &fileIdsEnd);

    /**
     * @brief Delete file metadata, corresponding to all files in one disk, from the queue
     *  (not from the main storage aka Vault, nor from indexer aka  santaclaus)
     * @param diskId Files having this disk ID will be deleted from the queue
     */
    void deleteDiskFiles(const string &diskId);

    /**
     * @brief Get all files metadata in this queue corresponding to a disk
     * @param diskId Disk ID of files to get
     * @return Maestro-Vault gRPC request for removing all files with the diskID
     */
    NODISCARD Maestro_Vault::RemoveFilesRequest getDiskFiles(const string &diskId);

    /**
     * @brief Get all disk IDs among all files metadata currently in the queue
     * @return Disk IDs as string in a container
     */
    NODISCARD std::unordered_set<string> getFilesDisk();
};

template <typename StrIterator>
    requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
void FilesRemoveQueueSchema::add(const string &diskId, const StrIterator &fileIdsToKeepBeg, const StrIterator &fileIdsToKeepEnd)
{
    MongoCXX::Document filter = makeDocument(makeField("diskId", diskId));
    std::vector<MongoCXX::Document> documents;

    this->_model.delete_many(filter.view());
    for (auto fileIdToKeep = fileIdsToKeepBeg; fileIdToKeep != fileIdsToKeepEnd; fileIdToKeep++) {
        documents.emplace_back(makeDocument(makeField("fileId", fileIdToKeep->c_str()), makeField("diskId", diskId)));
    }
    this->_model.insert_many(documents);
}

#endif