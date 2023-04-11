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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

typedef std::string string;
typedef std::size_t size_t;
typedef uint64_t uint64;
#define toString(str)  std::to_string(str)
#define toInteger(str) std::stoi(str)
#define toSize_t(str)  std::stoul(str)
#define UNUSED         __attribute__((unused))
#define NODISCARD      [[nodiscard]]
#define STR_FUNCTION   string(__FUNCTION__)

inline string getEnv(const string &env)
{
    const char *value = std::getenv(env.c_str());

    if (!value)
        throw std::invalid_argument(env + " environment variable not found");
    return value;
}

#endif