/**
 * @file schemas.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_SCHEMAS_HPP
#define MAESTRO_SCHEMAS_HPP

#include "Schemas/Files/DownloadedStack/DownloadedStackSchema.hpp"
#include "Schemas/Files/DownloadQueue/DownloadQueueSchema.hpp"
#include "Schemas/Files/RemoveQueue/RemoveQueueSchema.hpp"
#include "Schemas/Stats/DiskInfo/DiskInfoSchema.hpp"
#include "Schemas/Stats/UserDiskInfo/UserDiskInfoSchema.hpp"

struct FilesSchemas
{
    FilesDownloadedStackSchema downloadedStack;
    FilesDownloadQueueSchema downloadQueue;
    FilesRemoveQueueSchema removeQueue;
};

struct StatsSchemas
{
    StatsDiskInfoSchema diskInfo;
    StatsUserDiskInfoSchema userDiskInfo;
};

#endif