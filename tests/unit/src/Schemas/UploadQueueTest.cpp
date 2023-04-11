/**
 * @file UploadQueueTest.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 10/04/2023
 * @copyright Nuage Malin
 */

#include <gtest/gtest.h>

#include "Schemas/Mongo/Mongo.hpp"

MongoCXX::Mongo mongo;
FilesSchemas filesSchemas = mongo.getFilesSchemas();

TEST(UploadQueue, getFilesDisk)
{
    filesSchemas.uploadQueue.uploadFile("fileIdTest", "userIdTest", "diskIdTest", "contentTest");
    std::unordered_set<string> diskIds = filesSchemas.uploadQueue.getFilesDisk();

    ASSERT_EQ(diskIds.size(), 1);
    ASSERT_EQ(diskIds.count("diskIdTest"), 1);

    Maestro_Vault::UploadFilesRequest files = filesSchemas.uploadQueue.getDiskFiles("diskIdTest");
}

TEST(UploadQueue, getDiskFiles)
{
    Maestro_Vault::UploadFilesRequest files = filesSchemas.uploadQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(files.files_size(), 1);
    ASSERT_EQ(files.files(0).fileid(), "fileIdTest");
    ASSERT_EQ(files.files(0).userid(), "userIdTest");
    ASSERT_EQ(files.files(0).diskid(), "diskIdTest");
    ASSERT_EQ(files.files(0).content(), "contentTest");
}