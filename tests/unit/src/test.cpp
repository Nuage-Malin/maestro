/**
* @file test.cpp
* @author Arthur Jourdan
* @date of creation 11/11/22.
* @brief Example test with google test
 */
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(TestSuiteName, TestName)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}