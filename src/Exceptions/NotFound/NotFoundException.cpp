/**
 * @file NotFoundException.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 11/04/2023
 * @copyright Nuage Malin
 */

#include "NotFoundException.hpp"

NotFoundException::NotFoundException(const string &message, string functionName)
    : std::runtime_error(functionName + ": [NOT_FOUND] " + message)
{
}