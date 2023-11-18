/**
 * @file UsersBackService.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 17/02/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_USERSBACK_SERVICE_HPP
#define MAESTRO_USERSBACK_SERVICE_HPP

#include <utility>
#include <mongocxx/database.hpp>

#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"
#include "clients.hpp"
#include "schemas.hpp"
#include "Services/Template/TemplateService.hpp"
#include "Schemas/Stats/UserDiskInfo/UserDiskInfoSchema.hpp"

class UsersBackService : public TemplateService, public UsersBack_Maestro::UsersBack_Maestro_Service::Service {
  public:
    UsersBackService(const GrpcClients &clients, const EventsManager &events);
    ~UsersBackService() = default;

    grpc::Status fileUpload(
        grpc::ServerContext *context, const UsersBack_Maestro::FileUploadRequest *request,
        UsersBack_Maestro::FileUploadStatus *response
    ) override;

    grpc::Status getUserConsumption(
        grpc::ServerContext *context, const UsersBack_Maestro::GetUserConsumptionRequest *request,
        UsersBack_Maestro::GetUserConsumptionStatus *response
    ) override;

    grpc::Status getUserDiskSpace(
        grpc::ServerContext *context, const UsersBack_Maestro::GetUserDiskSpaceRequest *request,
        UsersBack_Maestro::GetUserDiskSpaceStatus *response
    ) override;

    grpc::Status askFileDownload(
        grpc::ServerContext *context, const UsersBack_Maestro::AskFileDownloadRequest *request,
        UsersBack_Maestro::AskFileDownloadStatus *response
    ) override;

    grpc::Status cancelFileDownload(
        grpc::ServerContext *context, const UsersBack_Maestro::CancelFileDownloadRequest *request,
        UsersBack_Maestro::CancelFileDownloadStatus *response
    ) override;

    grpc::Status fileDownload(
        grpc::ServerContext *context, const UsersBack_Maestro::FileDownloadRequest *request, File::File *response
    ) override;

    grpc::Status getFilesIndex(
        grpc::ServerContext *context, const UsersBack_Maestro::GetFilesIndexRequest *request,
        UsersBack_Maestro::GetFilesIndexStatus *response
    ) override;

    grpc::Status moveFile(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::MoveFileRequest *request,
        ::UsersBack_Maestro::MoveFileStatus *response
    ) override;

    grpc::Status renameFile(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::RenameFileRequest *request,
        ::UsersBack_Maestro::RenameFileStatus *response
    ) override;

    grpc::Status fileRemove(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::FileRemoveRequest *request,
        ::UsersBack_Maestro::FileRemoveStatus *response
    ) override;
    grpc::Status filesRemove(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::FilesRemoveRequest *request,
        ::UsersBack_Maestro::FilesRemoveStatus *response
    ) override;

    grpc::Status dirMake(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::DirMakeRequest *request,
        ::UsersBack_Maestro::DirMakeStatus *response
    ) override;

    grpc::Status dirRemove(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::DirRemoveRequest *request,
        ::UsersBack_Maestro::DirRemoveStatus *response
    ) override;

    grpc::Status moveDir(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::MoveDirectoryRequest *request,
        ::UsersBack_Maestro::MoveDirectoryStatus *response
    ) override;

    grpc::Status renameDir(
        ::grpc::ServerContext *context, const ::UsersBack_Maestro::RenameDirectoryRequest *request,
        ::UsersBack_Maestro::RenameDirectoryStatus *response
    ) override;

  private:
    /**
     * @brief Find files' diskId and group them like it
     * @tparam StrIterator Any iterator over a string
     * @param fileIdsBeg First element of the iterator
     * @param fileIdsEnd Last element of the iterator, will be compared to iterator to stop iterating
     * @return Map with diskId as key and unordered_set of fileIds as value
     */
    template <typename StrIterator>
        requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
    std::unordered_map<string, std::unordered_set<string>>
    getFilesDisk(const StrIterator &fileIdsBeg, const StrIterator &fileIdsEnd);

    /**
     * @brief Do the action of removing files (from santaclaus and vault)
     *         with the help of removeQueue if disks are not currently available.
     *         This method will be used by fileRemove, filesRemove and dirRemove to execute the same action.
     * @tparam StrIterator Any iterator over a string
     * @param fileIdsBeg First element of the iterator
     * @param fileIdsEnd Last element of the iterator, will be compared to iterator to stop iterating
     */
    template <typename StrIterator>
        requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
    UsersBack_Maestro::FilesRemoveStatus
    actFilesRemove(FilesSchemas &filesSchemas, StrIterator fileIdsBeg, const StrIterator &fileIdsEnd);

    void _fileUploadFailure(const File::NewFile &file, const Maestro_Santaclaus::AddFileStatus &addFileStatus);

    void _askFileDownloadFailure(
        FilesSchemas &filesSchemas, const string &fileId, const Maestro_Santaclaus::GetFileStatus &file,
        const Date &expirationDate, UsersBack_Maestro::AskFileDownloadStatus &response
    );
    void _fileRemoveFailure(FilesSchemas &filesSchemas, const string &diskId, const string &fileId);

  private:
    const GrpcClients &_clients;
};

template <typename StrIterator>
    requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
std::unordered_map<string, std::unordered_set<string>>
UsersBackService::getFilesDisk(const StrIterator &fileIdsBeg, const StrIterator &fileIdsEnd)
{
    // unordered_map with string as key, for diskId, unordered_set as value, to store each fileId corresponding to this diskId
    std::unordered_map<string, std::unordered_set<string>> filesDisks;
    string diskId;

    for (auto fileId = fileIdsBeg; fileId != fileIdsEnd; fileId++) {
        diskId = this->_clients.santaclaus.getFile(*fileId).diskid();
        filesDisks.find(diskId);
        if (auto files = filesDisks.find(diskId); files != filesDisks.end()) {
            files->second.insert(*fileId); // todo is this iterator affecting the filesDisks (is it a reference) ?
        } else {
            std::unordered_set<string> newFiles;

            newFiles.insert(*fileId);
            filesDisks.insert(std::make_pair(diskId, newFiles));
        }
    }
    return filesDisks;
}

template <typename StrIterator>
    requires std::input_iterator<StrIterator> && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string>
UsersBack_Maestro::FilesRemoveStatus
UsersBackService::actFilesRemove(FilesSchemas &filesSchemas, StrIterator fileIdsBeg, const StrIterator &fileIdsEnd)
{
    std::unordered_map<string, std::unordered_set<string>> filesDisks = getFilesDisk(fileIdsBeg, fileIdsEnd);
    UsersBack_Maestro::FilesRemoveStatus response;

    if (fileIdsBeg == fileIdsEnd)
        throw std::logic_error("Calling remove files with no files to remove, in function " + STR_FUNCTION);

    this->_clients.santaclaus.virtualRemoveFiles(fileIdsBeg, fileIdsEnd);
    for (const auto &filesDisk : filesDisks) {
        if (!this->_clients.bugle.diskStatus(filesDisk.first)) { // check if disk is turned off
            filesSchemas.removeQueue.add(filesDisk.first, filesDisk.second.begin(), filesDisk.second.end());
        } else {                                                 // if disk is turned on
            auto filesDiskRemoved = filesDisk.second;
            Maestro_Vault::RemoveFilesRequest my_request;
            auto sampleFile = this->_clients.santaclaus.getFile(*filesDisk.second.begin()); // get disk id from santaclaus

            my_request.set_userid(sampleFile.file().approxmetadata().userid());
            my_request.set_diskid(filesDisk.first);
            for (const auto &fileId : filesDisk.second) {
                my_request.add_fileid(fileId);
            }
            const auto &vaultResponse = this->_clients.vault.removeFiles(my_request);

            for (const auto &fileIdFailure : vaultResponse.fileidfailures()) {
                response.add_fileidfailures(fileIdFailure); // set response for UsersBack
                filesDiskRemoved.erase(fileIdFailure);      // file has not been removed, it is yet to be removed
            }
            try {
                this->_clients.santaclaus.physicalRemoveFiles(filesDiskRemoved.begin(), filesDiskRemoved.end());
            } catch (const RequestFailureException &e) { // if deletion in vault didn't succeed
                filesSchemas.removeQueue.add(
                    filesDisk.first, vaultResponse.fileidfailures().begin(), vaultResponse.fileidfailures().end()
                );
            }
        }
    }
    return response;
}

#endif