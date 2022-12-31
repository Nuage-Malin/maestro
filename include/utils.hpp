#ifndef MAESTRO_UTILS_HPP
#define MAESTRO_UTILS_HPP

#include <cstdlib>
#include <iostream>

typedef std::string string;
typedef std::size_t size_t;
#define toString(str)  std::to_string(str)
#define toInteger(str) std::stoi(str)
#define toSize_t(str)  std::stoul(str)
#define getenv(env)    std::getenv(env);

#endif