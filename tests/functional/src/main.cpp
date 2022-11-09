/*
** EPITECH PROJECT, 2022
** main.c
** File description:
** Entry point for users-db implementation
*/

#include <iostream>
#include <vector>
#include <functional>

/**
 * @brief Example of a callable function for a functional test
 * @return 0 if success, otherwise indicates failure
 */
static int dummyTestFunction(void)
{
    return 0;
}

static const std::vector<std::function<int(void)>> functionalTestFunctions = {
    dummyTestFunction,
};

int main(const int ac, const char *av[])
{
    uint index = 0;
    int retVal = EXIT_SUCCESS;

    if (ac < 2)
        std::cerr << "Parameters must be indexes of functional tests to be executed" << std::endl;
    for (int i = 1; i < ac; ++i) {
        errno = 0;
        index = strtol(av[i], NULL, 10);
        if (errno != 0 || index < 1 || index > functionalTestFunctions.size()) {
            std::cerr << "Parameter " << index << " is not an index for any functional test" << std::endl;
            continue;
        }
        if (functionalTestFunctions[index - 1]())
            retVal = EXIT_FAILURE;
    }
    return retVal;
}