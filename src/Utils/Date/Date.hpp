/**
 * @file Date.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 * @brief Date management. Mainly used to convert between gRPC and MongoDB dates.
 */

#ifndef MAESTRO_UTILS_DATE_HPP
#define MAESTRO_UTILS_DATE_HPP

#include <chrono>

#include <bsoncxx/types.hpp>
#include <google/protobuf/timestamp.pb.h>

#include "utils.hpp"

class Date {
  public:
    Date(const std::chrono::system_clock::time_point &date = std::chrono::high_resolution_clock::now());
    Date(const bsoncxx::v_noabi::types::b_date &date);
    Date(const google::protobuf::Timestamp &date);

    Date &operator=(const Date &date);

    NODISCARD bsoncxx::v_noabi::types::b_date toBSON() const;
    NODISCARD google::protobuf::Timestamp toProtobuf() const;
    NODISCARD std::chrono::system_clock::time_point toChrono() const;

  private:
    std::chrono::system_clock::time_point _date;
};

#endif