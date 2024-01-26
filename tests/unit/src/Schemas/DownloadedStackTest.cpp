/**
 * @file DownloadedStackTest.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include <gtest/gtest.h>

#include "Utils/UtilsTest.hpp"

TEST(DownloadedStack, doesFileExist_false)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();
    const bool &result = filesSchemas.downloadedStack.doesFileExist("fileIdTest");

    ASSERT_FALSE(result);
}

TEST(DownloadedStack, pushFile_doesFileExist)
{
    FilesSchemas filesSchemas = getCommonMongo().getFilesSchemas();

    filesSchemas.downloadedStack.add("fileIdTest", Date() + std::chrono::days(1));

    ASSERT_TRUE(filesSchemas.downloadedStack.doesFileExist("fileIdTest"));
}