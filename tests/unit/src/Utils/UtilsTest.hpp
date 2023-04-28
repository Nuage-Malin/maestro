/**
 * @file UtilsTest.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 23/02/2023
 * @copyright Nuage Malin
 * @brief Utils functions for tests
 */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "utils.hpp"
#include "Schemas/Mongo/Mongo.hpp"

static const EventsManager events;

/**
 * @brief Get a client to interact with a gRPC stub
 * @return The client
 */
template <class Client> Client &getCommonFileClient()
{
    const string envStrHost("MAESTRO_TESTS_HOST");
    const string envStrPort("MAESTRO_TESTS_PORT");
    const char *host = getenv(envStrHost.c_str());
    const char *port = getenv(envStrPort.c_str());

    if (!host || !port)
        throw std::invalid_argument(((!host) ? envStrHost : envStrPort) + " environment variable not found");
    static Client client(host + string(":") + port);

    return client;
}

inline MongoCXX::Mongo &getCommonMongo()
{
    static MongoCXX::Mongo mongo(events);

    return mongo;
}

#endif