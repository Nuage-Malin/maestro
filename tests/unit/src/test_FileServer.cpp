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

    // client.fileUpload("filename", "filedirname", "1", "file content");
    return client;
}

TEST(FileServer, fileUpload)
{
    // TODO start server
    // TODO create automated and only one way to start server simply for every test or at the beginning of tests
    FileClient &client = getCommonFileClient();

    std::cout << "BEFORE fileUpload" << std::endl;
    EXPECT_TRUE(client.fileUpload("filename", "filedirname", "1", "file content"));
    std::cout << "AFTER fileUpload" << std::endl;
}

TEST(FileServer, askFileDownload)
{
    //    ask if possible to download the file uploaded in last test
}

TEST(FileServer, fileDownload)
{
    //    download the file uploaded in previous test
}

TEST(FileServer, getFilesIndex)
{
    //    download the file uploaded in last test
}