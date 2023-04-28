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
#include <google/protobuf/duration.pb.h>

#include "utils.hpp"

class Date {
  public:
    Date(const string &date, const string &format = "%Y-%m-%d %H:%M:%S");
    Date(const std::chrono::system_clock::time_point &date = std::chrono::high_resolution_clock::now());
    Date(const bsoncxx::v_noabi::types::b_date &date);
    Date(const google::protobuf::Timestamp &date);

    NODISCARD const std::chrono::system_clock::time_point &operator*() const;
    Date &operator=(const Date &date);
    bool operator==(const Date &date) const;
    bool operator!=(const Date &date) const;
    bool operator<(const Date &date) const;
    bool operator>(const Date &date) const;
    bool operator<=(const Date &date) const;
    bool operator>=(const Date &date) const;
    Date operator+(const std::chrono::seconds &seconds) const;
    Date operator+(const std::chrono::minutes &minutes) const;
    Date operator+(const std::chrono::hours &hours) const;
    Date operator+(const std::chrono::days &days) const;
    Date operator-(const std::chrono::seconds &seconds) const;
    Date operator-(const std::chrono::minutes &minutes) const;
    Date operator-(const std::chrono::hours &hours) const;
    Date operator-(const std::chrono::days &days) const;

    NODISCARD const std::chrono::system_clock::time_point &toChrono() const;
    NODISCARD bsoncxx::v_noabi::types::b_date toBSON() const;
    NODISCARD google::protobuf::Timestamp toProtobuf() const;
    NODISCARD google::protobuf::Timestamp *toAllocatedProtobuf() const;
    /**
     * @brief Convert the date to a duration from a given date. This date can be in the past or in the future.
     *
     * @param from Optional date to convert from. If not given, the current date is used.
     * @return google::protobuf::Duration
     */
    NODISCARD google::protobuf::Duration toDuration(const Date &from = Date()) const;
    // Same as toDuration but allocates the duration on the heap
    NODISCARD google::protobuf::Duration *toAllocatedDuration(const Date &from = Date()) const;

  private:
    void _toProtobuf(google::protobuf::Timestamp &timestamp) const;
    void _toDuration(google::protobuf::Duration &duration, const Date &from) const;

  private:
    std::chrono::system_clock::time_point _date;
};

std::ostream &operator<<(std::ostream &stream, const Date &date);

#endif