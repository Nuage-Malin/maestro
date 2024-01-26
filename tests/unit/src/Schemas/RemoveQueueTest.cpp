/**
 * @file RemoveQueueTest.cpp
 * @author Arthur Jourdan (arthur.jourdan@epitech.eu)
 * @date of creation 12/05/23.
 * @copyright Nuage Malin
 * @brief TODO
 */

#include <gtest/gtest.h>

#include "Utils/UtilsTest.hpp"

TEST(RemoveQueue, getFilesDisk)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    filesSchemas.removeQueue.add("diskIdTest", "fileIdTest");
    const std::unordered_set<string> &diskIds = filesSchemas.removeQueue.getFilesDisk();

    ASSERT_EQ(diskIds.size(), 1);
    ASSERT_EQ(diskIds.count("diskIdTest"), 1);
}

TEST(RemoveQueue, getSeveralFilesDisk)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    filesSchemas.removeQueue.add("diskId2Test", "fileId2Test");
    const std::unordered_set<string> &diskIds = filesSchemas.removeQueue.getFilesDisk();

    ASSERT_EQ(diskIds.size(), 2);
    ASSERT_EQ(diskIds.count("diskIdTest"), 1);
    ASSERT_EQ(diskIds.count("diskId2Test"), 1);
}

TEST(RemoveQueue, getDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    const Maestro_Vault::RemoveFilesRequest &files = filesSchemas.removeQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(files.fileids_size(), 1);
    ASSERT_EQ(files.fileids(0), "fileIdTest");
}

TEST(RemoveQueue, getSeveralDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    filesSchemas.removeQueue.add("diskIdTest", "fileId2Test");
    const Maestro_Vault::RemoveFilesRequest &files = filesSchemas.removeQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(files.fileids_size(), 2);
    ASSERT_EQ(files.fileids(0), "fileIdTest");
    ASSERT_EQ(files.fileids(1), "fileId2Test");
}

TEST(RemoveQueue, addGetSeveralDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    std::vector<string> filesToInsert;
    filesToInsert.push_back("fileId0");
    filesToInsert.push_back("fileId1");
    filesSchemas.removeQueue.add("diskId3Test", filesToInsert.begin(), filesToInsert.end());
    const Maestro_Vault::RemoveFilesRequest &files = filesSchemas.removeQueue.getDiskFiles("diskId3Test");

    ASSERT_EQ(files.fileids_size(), 2);
    ASSERT_EQ(files.fileids(0), "fileId0");
    ASSERT_EQ(files.fileids(1), "fileId1");
}

TEST(RemoveQueue, deleteDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    Maestro_Vault::RemoveFilesRequest files = filesSchemas.removeQueue.getDiskFiles("diskIdTest");

    ASSERT_GT(files.fileids_size(), 0);
    filesSchemas.removeQueue.deleteDiskFiles("diskIdTest");
    files = filesSchemas.removeQueue.getDiskFiles("diskIdTest");
    ASSERT_EQ(files.fileids_size(), 0);
}