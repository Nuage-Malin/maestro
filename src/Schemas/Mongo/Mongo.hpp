/**
 * @file Mongo.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 10/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_MONGO_HPP
#define MAESTRO_MONGO_HPP

#include <mongocxx/client.hpp>

#include "schemas.hpp"

namespace MongoCXX
{
    class Mongo {
      public:
        Mongo();
        ~Mongo() = default;

        NODISCARD FilesSchemas getFilesSchema() const;
        NODISCARD StatsSchemas getStatsSchema() const;

      private:
        mongocxx::client _client;
    };
}; // namespace MongoCXX

#endif