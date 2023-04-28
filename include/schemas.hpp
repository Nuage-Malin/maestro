/**
 * @file schemas.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_SCHEMAS_HPP
#define MAESTRO_SCHEMAS_HPP

#include "Schemas/Files/UploadQueue/UploadQueueSchema.hpp"
#include "Schemas/Files/DownloadedStack/DownloadedStackSchema.hpp"
#include "Schemas/Files/DownloadQueue/DownloadQueueSchema.hpp"
#include "Schemas/Stats/UserDiskInfo/UserDiskInfoSchema.hpp"

struct FilesSchemas
{
    FilesUploadQueueSchema uploadQueue;
    FilesDownloadedStackSchema downloadedStack;
    FilesDownloadQueueSchema downloadQueue;
};

struct StatsSchemas
{
    StatsUserDiskInfoSchema userDiskInfo;
};

#endif