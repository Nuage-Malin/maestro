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
        Mongo(const EventsManager &events);
        ~Mongo() = default;

        NODISCARD FilesSchemas getFilesSchemas() const;
        NODISCARD StatsSchemas getStatsSchemas() const;

      private:
        mongocxx::client _client;
        const EventsManager &_events;
    };
}; // namespace MongoCXX

#endif