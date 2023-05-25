/**
 * @file main.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 25/05/2023
 * @copyright Nuage Malin
 * @brief Main file for the system tests
 */

#include "utils.hpp"

#include "clients/UsersBack/UsersBackClient.hpp"

int main()
{
    const string maestroURI = getEnv("MAESTRO_TESTS_URI");
    UsersBackClient usersBackClient(maestroURI);

    return 0;
}