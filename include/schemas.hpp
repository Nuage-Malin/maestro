/**
 * @file schemas.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 16/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_SCHEMAS_HPP
#define MAESTRO_SCHEMAS_HPP

#include "Schemas/Files/UploadQueue/UploadQueueSchema.hpp"

struct FilesSchemas
{
    UploadQueueSchema uploadQueue;
};

#endif