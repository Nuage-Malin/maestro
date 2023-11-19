/**
 * @file DownloadQueueTest.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include <gtest/gtest.h>

#include "Utils/UtilsTest.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

TEST(DownloadQueue, getRequestedDate_throwNotFound)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();

    ASSERT_THROW({ filesSchemas.downloadQueue.getRequestedDate("fileIdTest", "diskIdTest"); }, NotFoundException);
}

TEST(DownloadQueue, getFilesDisk_empty)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    std::unordered_set<string> disks = filesSchemas.downloadQueue.getFilesDisk();

    ASSERT_EQ(disks.size(), 0);
}

TEST(DownloadQueue, add_getRequestedDate)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();

    filesSchemas.downloadQueue.add("fileIdTest", "userIdTest", "diskIdTest");
    ASSERT_NO_THROW({ filesSchemas.downloadQueue.getRequestedDate("fileIdTest", "diskIdTest"); });
}

TEST(DownloadQueue, getFilesDisk_one)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    std::unordered_set<string> disks = filesSchemas.downloadQueue.getFilesDisk();

    ASSERT_EQ(disks.size(), 1);
    ASSERT_EQ(disks.count("diskIdTest"), 1);
}

TEST(DownloadQueue, getDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    const Maestro_Vault::DownloadFilesRequest &request = filesSchemas.downloadQueue.getDiskFiles("diskIdTest");

    ASSERT_EQ(request.files_size(), 1);
    ASSERT_EQ(request.files(0).fileid(), "fileIdTest");
}

TEST(DownloadQueue, deleteDiskFiles)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    filesSchemas.downloadQueue.deleteDiskFiles("diskIdTest");
    std::unordered_set<string> disks = filesSchemas.downloadQueue.getFilesDisk();

    ASSERT_EQ(disks.size(), 0);
}