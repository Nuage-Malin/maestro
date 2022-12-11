/**
 * @file test_FileServer.cpp
 * @author Arthur Jourdan
 * @date of creation 11/11/22.
 * @brief TODO
 */

#include <gtest/gtest.h>

#include "FileClient.hpp"

// Demonstrate some basic assertions.
TEST(FileServer, FileUpload)
{
    FileClient client("0.0.0.0");

    EXPECT_TRUE(client.fileUpload("filename", "filedirname", "1", "file content"));
}