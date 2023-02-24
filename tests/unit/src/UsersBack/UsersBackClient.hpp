/**
 * @file UsersBackClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 22/02/2023
 * @copyright Nuage Malin
 * @brief
 */

#ifndef TEST_USERSBACK_CLIENT_HPP
#define TEST_USERSBACK_CLIENT_HPP

#include <grpcpp/grpcpp.h>

#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"

class UsersBackClient {
  public:
    UsersBackClient(const string &serverIP);
    ~UsersBackClient() = default;

    bool getUserConsumption(
        UsersBack_Maestro::GetUserConsumptionStatus &response, const UsersBack_Maestro::GetUserConsumptionRequest &request
    ) const;

  private:
    std::unique_ptr<UsersBack_Maestro::UsersBack_Maestro_Service::Stub> _client;
};

#endif // TEST_TESTING_FILECLIENT_HPP