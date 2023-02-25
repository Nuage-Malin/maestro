/**
 * @file Date.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 18/02/2023
 * @copyright Nuage Malin
 */

#include <iomanip>

#include "Date.hpp"

Date::Date(const string &date, const string &format)
{
    std::tm tm = {};
    std::istringstream ss(date);

    ss >> std::get_time(&tm, format.c_str());
    if (ss.fail())
        throw std::runtime_error("Failed to parse date");
    this->_date = std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

Date::Date(const std::chrono::system_clock::time_point &date) : _date(date)
{
}

Date::Date(const bsoncxx::v_noabi::types::b_date &date)
    : _date(std::chrono::system_clock::time_point(std::chrono::milliseconds(date.to_int64())))
{
}

Date::Date(const google::protobuf::Timestamp &date)
    : _date(std::chrono::system_clock::time_point(std::chrono::seconds(date.seconds()) + std::chrono::nanoseconds(date.nanos())))
{
}

Date &Date::operator=(const Date &date)
{
    this->_date = date.toChrono();

    return *this;
}

NODISCARD bsoncxx::v_noabi::types::b_date Date::toBSON() const
{
    return bsoncxx::types::b_date(this->_date);
}

NODISCARD google::protobuf::Timestamp Date::toProtobuf() const
{
    const auto &time = this->_date.time_since_epoch();

    google::protobuf::Timestamp timestamp;
    const auto &seconds = std::chrono::duration_cast<std::chrono::seconds>(time);
    timestamp.set_seconds(seconds.count());
    timestamp.set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(time - seconds).count()));
    return timestamp;
}

NODISCARD google::protobuf::Timestamp *Date::toAllocatedProtobuf() const
{
    google::protobuf::Timestamp *timestamp = new google::protobuf::Timestamp();

    this->_toProtobuf(*timestamp);
    return timestamp;
}

NODISCARD std::chrono::system_clock::time_point Date::toChrono() const
{
    return this->_date;
}

void Date::_toProtobuf(google::protobuf::Timestamp &timestamp) const
{
    const auto &time = this->_date.time_since_epoch();
    const auto &seconds = std::chrono::duration_cast<std::chrono::seconds>(time);

    timestamp.set_seconds(seconds.count());
    timestamp.set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(time - seconds).count()));
}
