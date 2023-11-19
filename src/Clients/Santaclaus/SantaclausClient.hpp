/**
 * @file SantaclausClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 14/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_SANTACLAUS_CLIENT_HPP
#define MAESTRO_SANTACLAUS_CLIENT_HPP

#include <optional>

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "Maestro_Santaclaus/Maestro_Santaclaus.grpc.pb.h"
#include "Exceptions/RequestFailure/RequestFailureException.hpp"

#include "utils.hpp"

class SantaclausClient {
  public:
    SantaclausClient(const std::shared_ptr<grpc::ChannelInterface> &channel);
    ~SantaclausClient() = default;

    /// Add
    NODISCARD Maestro_Santaclaus::AddFileStatus addFile(const File::FileApproxMetadata &file, const uint64 &fileSize) const;
    Maestro_Santaclaus::AddDirectoryStatus addDirectory(const File::FileApproxMetadata &directory) const;

    /// Get
    NODISCARD Maestro_Santaclaus::GetFileStatus getFile(const string &fileId) const;

    NODISCARD Maestro_Santaclaus::GetDirectoryStatus
    getDirectory(const string &userId, const std::optional<string> &dirId = std::nullopt, bool isRecursive = false) const;

    /// Remove
    Maestro_Santaclaus::RemoveFileStatus virtualRemoveFile(const string &fileId) const;
    template <typename StrIterator>
        requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
    Maestro_Santaclaus::RemoveFilesStatus virtualRemoveFiles(StrIterator fileIdsBeg, const StrIterator &fileIdsEnd) const;
    Maestro_Santaclaus::RemoveFileStatus physicalRemoveFile(const string &fileId) const;
    Maestro_Santaclaus::RemoveFilesStatus physicalRemoveFiles(const Maestro_Santaclaus::RemoveFilesRequest &files) const;

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
    Maestro_Santaclaus::RemoveFilesStatus physicalRemoveFiles(StrIterator fileIdsBeg, const StrIterator &fileIdsEnd) const;

    Maestro_Santaclaus::RemoveDirectoryStatus removeDirectory(const string &dirId) const;
    Maestro_Santaclaus::RemoveDirectoryStatus removeUser(const string &userId) const;

    /// Move
    Maestro_Santaclaus::MoveFileStatus moveFile(const string &fileId, const string &newDirId) const;
    Maestro_Santaclaus::RenameFileStatus renameFile(const string &fileId, const string &name) const;

    Maestro_Santaclaus::MoveDirectoryStatus moveDirectory(const string &dirId, const string &newDirId) const;
    Maestro_Santaclaus::RenameDirectoryStatus renameDirectory(const string &dirId, const string &name) const;

  private:
    void _callLogger(const string &functionName) const;

  private:
    std::unique_ptr<Maestro_Santaclaus::Maestro_Santaclaus_Service::Stub> _stub;
};

template <typename StrIterator>
    requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
Maestro_Santaclaus::RemoveFilesStatus SantaclausClient::physicalRemoveFiles(StrIterator fileIdsBeg, const StrIterator &fileIdsEnd)
    const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFilesStatus response;
    Maestro_Santaclaus::RemoveFilesRequest request;

    for (auto &fileId = fileIdsBeg; fileId != fileIdsEnd; fileId++) {
        request.add_fileids(fileId->data());
    }
    auto status = this->_stub->physicalRemoveFiles(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

template <typename StrIterator>
    requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
Maestro_Santaclaus::RemoveFilesStatus SantaclausClient::virtualRemoveFiles(StrIterator fileIdsBeg, const StrIterator &fileIdsEnd)
    const
{
    grpc::ClientContext context;
    Maestro_Santaclaus::RemoveFilesStatus response;
    Maestro_Santaclaus::RemoveFilesRequest request;
    uint index = 0;

    if (fileIdsBeg == fileIdsEnd)
        throw std::logic_error("Calling remove files with no files to remove, in function " + STR_FUNCTION);

    for (auto fileId = fileIdsBeg; fileId != fileIdsEnd; fileId++) {
        request.set_fileids(index++, fileId->data());
    }
    auto status = _stub->virtualRemoveFiles(&context, request, &response);

    if (!status.ok())
        throw RequestFailureException(status, __FUNCTION__);
    return response;
}

#endif