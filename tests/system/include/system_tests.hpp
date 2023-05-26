/**
 * @file system_tests.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 26/05/2023
 * @copyright Nuage Malin
 */

#ifndef TEST_SYSTEM_TESTS_HPP
#define TEST_SYSTEM_TESTS_HPP

#define ASSERT_TRUE(condition) \
    if (!(condition))          \
        return false;
#define ASSERT_FALSE(condition) \
    if ((condition))            \
        return false;
#define ASSERT_EQ(a, b) \
    if ((a) != (b))     \
        return false;
#define ASSERT_NE(a, b) \
    if ((a) == (b))     \
        return false;

#endif