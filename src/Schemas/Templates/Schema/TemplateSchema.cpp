/**
 * @file TemplateSchema.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include "TemplateSchema.hpp"

TemplateSchema::TemplateSchema(const mongocxx::database &database, const string &collectionName)
    : _model(database[collectionName])
{
}