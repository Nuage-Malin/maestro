/**
 * @file UsersBackClient.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 25/05/2023
 * @copyright Nuage Malin
 */

#ifndef TEST_USERSBACK_CLIENT_HPP
#define TEST_USERSBACK_CLIENT_HPP

#include <grpcpp/grpcpp.h>

#include "UsersBack_Maestro/UsersBack_Maestro.grpc.pb.h"

#include "utils.hpp"

class UsersBackClient {
  public:
    UsersBackClient(const string &serverURI);
    ~UsersBackClient() = default;

  private:
    std::unique_ptr<UsersBack_Maestro::UsersBack_Maestro_Service::Stub> _client;
};

#endif