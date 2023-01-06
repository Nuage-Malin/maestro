/**
 * @file utils.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/01/2023
 * @copyright Nuage Malin
 * @brief Utils commands
 */

#ifndef MAESTRO_UTILS_HPP
#define MAESTRO_UTILS_HPP

#include <cstdlib>
#include <iostream>

typedef std::string string;
typedef std::size_t size_t;
#define toString(str)  std::to_string(str)
#define toInteger(str) std::stoi(str)
#define toSize_t(str)  std::stoul(str)
#define getenv(env)    std::getenv(env)
#define UNUSED         __attribute__((unused))
#define NODISCARD      [[nodiscard]]

#endif