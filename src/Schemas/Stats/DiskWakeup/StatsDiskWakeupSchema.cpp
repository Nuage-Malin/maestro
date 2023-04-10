/**
 * @file StatsDiskWakeupSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 21/02/2023
 * @copyright Nuage Malin
 */

#include "StatsDiskWakeupSchema.hpp"

StatsDiskWakeupSchema::StatsDiskWakeupSchema(const mongocxx::database &database) : TemplateSchema(database, "diskWakeup")
{
}