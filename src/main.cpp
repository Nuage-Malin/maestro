/**
 * @file main.cpp
 * @author Arthur Jourdan
 * @date of creation 22/10/22.
 * @brief TODO
 */

#include <cstdlib>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

/**
 * @brief main function
 * @param ac Number of arguments
 * @param av Argument vector
 * @return EXIT_SUCCESS if success, EXIT_FAILURE otherwise
 */
int main(__attribute__((unused)) const int ac, __attribute__((unused)) const char *av[])
{
    mongocxx::instance inst{}; // This should be done only once.
    mongocxx::client conn{
        mongocxx::uri{
            "mongodb+srv://<username>:<password>@<cluster-url>/test?retryWrites=true&w=majority"
        }
    };
    mongocxx::database db = conn["test"];

    return EXIT_SUCCESS;
}