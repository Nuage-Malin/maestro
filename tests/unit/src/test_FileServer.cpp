/**
 * @file test_FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 11/11/22.
 * @brief TODO
 */

#include <cstdlib>
#include <gtest/gtest.h>

#include "FileClient.hpp"

/**
 * @brief Get a client to interact with a gRPC stub
 * @return The client
 */
FileClient &getCommonFileClient()
{
    const std::string envStrHost("MAESTRO_TESTS_HOST");
    const std::string envStrPort("MAESTRO_TESTS_PORT");
    const char *host = std::getenv(envStrHost.c_str());
    const char *port = std::getenv(envStrPort.c_str());

    if (!host || !port)
        throw std::invalid_argument(((!host) ? envStrHost : envStrPort) + " not found");
    static FileClient client(host + std::string(":") + port);

    return client;
}

static const std::string DUMMY_USER_ID = "63b478c92303588732090c31";

static const std::string fileContent_uploaded = "file content";
static const std::string fileDirpath_uploaded = "/file/dirname/";
static const std::string fileName_uploaded = "filename";

static std::string fileId_toDownload; // get id from index, to ask download and download

/**
 * @brief Upload a file
 */
TEST(FileServer, fileUpload)
{
    FileClient &client = getCommonFileClient();
    UsersBack_Maestro::FileUploadStatus response;

    EXPECT_TRUE(
        client.fileUpload(response, fileName_uploaded, fileDirpath_uploaded, DUMMY_USER_ID, fileContent_uploaded));
}

/**
 * @brief Get the index containing all stored files
 */
TEST(FileServer, getFilesIndex)
{
    FileClient &client = getCommonFileClient();
    File::FilesIndex response;
    bool lastFileUploaded(false);

    EXPECT_TRUE(client.getFilesIndex(response, DUMMY_USER_ID, fileDirpath_uploaded));
    EXPECT_GT(response.index_size(), 0);
    std::cout << "File index:" << std::endl;
    for (const auto &file : response.index()) {
        std::cout << "============================" << std::endl;
        std::cout << "File id: " << file.fileid() << std::endl;
        std::cout << "File dirpath: " << file.approxmetadata().dirpath() << std::endl;
        std::cout << "File name: " << file.approxmetadata().name() << std::endl;
        std::cout << "File user id: " << file.approxmetadata().userid() << std::endl;
        std::cout << std::boolalpha << "File is downloadable: " << file.isdownloadable() << std::endl;
        std::cout << "File lastEditor: " << file.lasteditorid() << std::endl;
        std::cout << "File creation: " << file.creation().seconds() << std::endl;
        std::cout << "File lastEdit: " << file.lastedit().seconds() << std::endl;
        fileId_toDownload = file.fileid(); // set file id for further tests on download
        if (file.approxmetadata().dirpath() == fileDirpath_uploaded && file.approxmetadata().name() == fileName_uploaded
            && file.approxmetadata().userid() == DUMMY_USER_ID)
            lastFileUploaded = true;
    }
    EXPECT_TRUE(lastFileUploaded);
}

static int waitingTime(0);
/**
 * @brief Ask if possible to download the file uploaded in previous test
 */
TEST(FileServer, askFileDownload)
{
    FileClient &client = getCommonFileClient();
    UsersBack_Maestro::AskFileDownloadStatus response;

    EXPECT_TRUE(client.askFileDownload(response, fileId_toDownload));
    EXPECT_GT(response.waitingtime().seconds(), 0);
    waitingTime = response.waitingtime().seconds();
}

/**
 * @brief Ask (for the second time) if possible to download a file, the waiting time should have decreased
 */
TEST(FileServer, askFileDownload_sameFile)
{
    FileClient &client = getCommonFileClient();
    UsersBack_Maestro::AskFileDownloadStatus response;

    sleep(1);
    EXPECT_TRUE(client.askFileDownload(response, fileId_toDownload));
    EXPECT_GT(response.waitingtime().seconds(), 0);
    EXPECT_GT(waitingTime, response.waitingtime().seconds());
}

/**
 * @brief Download the file uploaded in previous test
 */
TEST(FileServer, fileDownload)
{
    FileClient &client = getCommonFileClient();
    File::File response;

    EXPECT_FALSE(client.fileDownload(response, fileId_toDownload));
    //    EXPECT_EQ(response.content(), fileContent_uploaded);
}

/**
 * @brief Upload a file and set the waiting time so that it is now available for download
 */
TEST(FileServer, fileDownloadAvailable)
{
    FileClient &client = getCommonFileClient();
    UsersBack_Maestro::FileUploadStatus responseUpload;
    UsersBack_Maestro::AskFileDownloadStatus responseAsk;
    File::FilesIndex responseFilesIndex;
    File::File responseDownload;
    const std::string fileNameUploaded("some_filename");
    const std::string fileDirpathUploaded("/FileServer/fileDownloadAvailable/");
    const std::string fileContentUploaded("the file content is such as this one");
    const char *downloadWaitingTime = std::getenv("DOWNLOAD_WAITING_TIME");

    if (!downloadWaitingTime)
        throw std::invalid_argument("DOWNLOAD_WAITING_TIME environment variable not found");
    EXPECT_TRUE(
        client.fileUpload(responseUpload, fileNameUploaded, fileDirpathUploaded, DUMMY_USER_ID, fileContentUploaded));
    EXPECT_TRUE(client.askFileDownload(responseAsk, responseUpload.fileid()));
    EXPECT_TRUE(client.getFilesIndex(responseFilesIndex, DUMMY_USER_ID, fileDirpathUploaded));
    for (const auto &file : responseFilesIndex.index())
        if (file.fileid() == responseUpload.fileid()) {
            EXPECT_FALSE(file.isdownloadable());
            break;
        }
    std::cout << "Waiting " << downloadWaitingTime << " seconds to download the file..." << std::endl;
    sleep(std::stoi(downloadWaitingTime));
    EXPECT_TRUE(client.getFilesIndex(responseFilesIndex, DUMMY_USER_ID, fileDirpathUploaded));
    for (const auto &file : responseFilesIndex.index())
        if (file.fileid() == responseUpload.fileid()) {
            EXPECT_TRUE(file.isdownloadable());
            break;
        }
    EXPECT_TRUE(client.fileDownload(responseDownload, responseUpload.fileid()));
    EXPECT_EQ(responseDownload.content(), fileContentUploaded);
}