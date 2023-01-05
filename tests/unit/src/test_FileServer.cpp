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

TEST(FileServer, getFilesIndex)
{
    //    download the file uploaded in last test
}

TEST(FileServer, fileUpload)
{
    // TODO start server
    // TODO create automated and only one way to start server simply for every test or at the beginning of tests
    FileClient &client = getCommonFileClient();

    EXPECT_TRUE(client.fileUpload("filename", "/file/dirname/", "63b478c92303588732090c31", "file content"));
}

TEST(FileServer, askFileDownload)
{
    FileClient &client = getCommonFileClient();

    EXPECT_TRUE(client.askFileDownload("63b73f1e4891d5d2160ac241")); // This ID should be dynamic
}

TEST(FileServer, fileDownload)
{
    //    download the file uploaded in previous test
}