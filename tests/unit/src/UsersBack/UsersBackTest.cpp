/**
 * @file UsersBackTest.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 23/02/2023
 * @copyright Nuage Malin
 * @brief Test UsersBack
 */

#include <gtest/gtest.h>

#include "Utils/Date/Date.hpp"
#include "Utils/UtilsTest.hpp"
#include "UsersBackClient.hpp"

const string userId = "61e3a3e4d9564b6f9c6bf24d";

TEST(UsersBackTest, getUserConsumption)
{
    const auto &client = getCommonFileClient<UsersBackClient>();
    UsersBack_Maestro::GetUserConsumptionRequest request;
    UsersBack_Maestro::GetUserConsumptionStatus response;
    google::protobuf::Timestamp *startDate = Date("2022-01-01 00:00:00").toAllocatedProtobuf();
    google::protobuf::Timestamp *endDate = Date("2022-01-15 00:00:00").toAllocatedProtobuf();

    request.set_userid(userId);
    request.set_allocated_startdate(startDate);
    request.set_allocated_enddate(endDate);

    ASSERT_TRUE(client.getUserConsumption(response, request));
    ASSERT_EQ(response.consumption(), 1000 + 1500);
}