/**
 * @file main.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 25/05/2023
 * @copyright Nuage Malin
 * @brief Main file for the system tests
 */

#include "utils.hpp"

#include "UsersBack/tests/UsersBackTests.hpp"

int main()
{
    const string &maestroURI = getEnv("MAESTRO_TESTS_URI");
    UsersBackClient usersBackClient(maestroURI);
    UsersBackTests usersBackTests(usersBackClient);

    usersBackTests.run();

    std::cout << "Total tests: " << usersBackTests.getTotalTests() << std::endl;
    std::cout << "Passed tests: " << usersBackTests.getPassedTests() << std::endl;
    std::cout << "Failed tests: " << usersBackTests.getTotalTests() - usersBackTests.getPassedTests() << std::endl;

    return usersBackTests.getTotalTests() != usersBackTests.getPassedTests();
}