/**
 * @file test_FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 11/11/22.
 * @brief TODO
 */

#include <gtest/gtest.h>

#include "FileClient.hpp"

FileClient &getCommonFileClient()
{
    static FileClient client("127.0.0.1:50051");

    client.fileUpload("filename", "filedirname", "1", "file content");
    return client;
}

TEST(FileServer, fileUpload)
{
    // TODO start server
    // TODO create automated and only one way to start server simply for every test or at the beginning of tests
    FileClient &client(getCommonFileClient());

    EXPECT_TRUE(client.fileUpload("filename", "filedirname", "1", "file content"));
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