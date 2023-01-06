/**
 * @file test_FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 11/11/22.
 * @brief TODO
 */

#include <cstdlib>
#include <gtest/gtest.h>

#include "FileClient.hpp"

FileClient &getCommonFileClient()
{
    const char *host = std::getenv("MAESTRO_TESTS_HOST");
    const char *port = std::getenv("MAESTRO_TESTS_PORT");

    if (!host || !port)
        throw std::invalid_argument("MAESTRO_TESTS_HOST or MAESTRO_TESTS_PORT not found");
    static FileClient client(host + std::string(":") + port);

    return client;
}

const std::string DUMMY_USER_ID = "63b478c92303588732090c31";

TEST(FileServer, fileUpload)
{
    FileClient &client = getCommonFileClient();
    UsersBack_Maestro::FileUploadStatus response;

    EXPECT_TRUE(client.fileUpload(response, "filename", "/file/dirname/", DUMMY_USER_ID, "file content"));
    EXPECT_FALSE(response.fileid().empty());
}

TEST(FileServer, getFilesIndex)
{
    FileClient &client = getCommonFileClient();
    File::FilesIndex response;

    EXPECT_TRUE(client.getFilesIndex(response, DUMMY_USER_ID, "/file/dirname/"));
    EXPECT_GT(response.index_size(), 0);
    for (const auto &file : response.index()) {
        std::cout << "============================" << std::endl;
        std::cout << "File id: " << file.fileid() << std::endl;
        std::cout << "File dirpath: " << file.approxmetadata().dirpath() << std::endl;
        std::cout << "File name: " << file.approxmetadata().name() << std::endl;
        std::cout << "File user id: " << file.approxmetadata().userid() << std::endl;
        std::cout << "File lastEditor: " << file.lasteditorid() << std::endl;
        std::cout << "File creation: " << file.creation().seconds() << std::endl;
        std::cout << "File lastEdit: " << file.lastedit().seconds() << std::endl;
    }
}

TEST(FileServer, askFileDownload)
{
    FileClient &client = getCommonFileClient();
    File::FilesIndex filesResponse;
    UsersBack_Maestro::AskFileDownloadStatus response;

    EXPECT_TRUE(client.getFilesIndex(filesResponse, DUMMY_USER_ID, "/file/dirname/"));
    EXPECT_GT(filesResponse.index_size(), 0);
    EXPECT_TRUE(client.askFileDownload(response, filesResponse.index().at(0).fileid()));
    EXPECT_GT(response.waitingtime().seconds(), 0);
}

TEST(FileServer, fileDownload)
{
    //    download the file uploaded in previous test
}