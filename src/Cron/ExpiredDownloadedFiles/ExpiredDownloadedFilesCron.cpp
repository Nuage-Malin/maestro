/**
 * @file ExpiredDownloadedFilesCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#include "ExpiredDownloadedFilesCron.hpp"

ExpiredDownloadedFilesCron::ExpiredDownloadedFilesCron(FilesSchemas &filesSchemas)
    : TemplateCron("ExpiredDownloadedFiles"), _filesSchemas(filesSchemas)
{
}

void ExpiredDownloadedFilesCron::run()
{
    this->_filesSchemas.downloadedStack.deleteExpiredFiles();
}