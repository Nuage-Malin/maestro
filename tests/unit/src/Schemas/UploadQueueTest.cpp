/**
 * @file UploadQueueTest.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 10/04/2023
 * @copyright Nuage Malin
 */

#include <gtest/gtest.h>

#include "Utils/UtilsTest.hpp"

TEST(UploadQueue, getFilesDisk)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    filesSchemas.uploadQueue.uploadFile("fileIdTest", "userIdTest", "diskIdTest", "contentTest");
    const std::unordered_set<string> &diskIds = filesSchemas.uploadQueue.getFilesDisk();

    ASSERT_EQ(diskIds.size(), 1);
    ASSERT_EQ(diskIds.count("diskIdTest"), 1);
}

TEST(UploadQueue, getDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    const std::pair<std::vector<MongoCXX::ValueView>, Maestro_Vault::UploadFilesRequest> &files =
        filesSchemas.uploadQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(files.second.files_size(), 1);
    ASSERT_EQ(files.second.files(0).fileid(), "fileIdTest");
    ASSERT_EQ(files.second.files(0).userid(), "userIdTest");
    ASSERT_EQ(files.second.files(0).diskid(), "diskIdTest");
    ASSERT_EQ(files.second.files(0).content(), "contentTest");
}

TEST(UploadQueue, deleteFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    std::pair<std::vector<MongoCXX::ValueView>, Maestro_Vault::UploadFilesRequest> files =
        filesSchemas.uploadQueue.getDiskFiles("diskIdTest");

    ASSERT_GT(files.first.size(), 0);
    filesSchemas.uploadQueue.deleteFiles(files.first);
    files = filesSchemas.uploadQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(files.first.size(), 0);
}