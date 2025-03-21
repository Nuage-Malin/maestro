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

NODISCARD const std::chrono::system_clock::time_point &Date::operator*() const
{
    return this->toChrono();
}

Date &Date::operator=(const Date &date)
{
    this->_date = date.toChrono();

    return *this;
}

bool Date::operator==(const Date &date) const
{
    return this->_date == date.toChrono();
}

bool Date::operator!=(const Date &date) const
{
    return this->_date != date.toChrono();
}

bool Date::operator<(const Date &date) const
{
    return this->_date < date.toChrono();
}

bool Date::operator>(const Date &date) const
{
    return this->_date > date.toChrono();
}

bool Date::operator<=(const Date &date) const
{
    return this->_date <= date.toChrono();
}

bool Date::operator>=(const Date &date) const
{
    return this->_date >= date.toChrono();
}

Date Date::operator+(const std::chrono::seconds &seconds) const
{
    return Date(this->_date + seconds);
}

Date Date::operator+(const std::chrono::minutes &minutes) const
{
    return Date(this->_date + minutes);
}

Date Date::operator+(const std::chrono::hours &hours) const
{
    return Date(this->_date + hours);
}

Date Date::operator+(const std::chrono::days &days) const
{
    return Date(this->_date + days);
}

Date Date::operator-(const std::chrono::seconds &seconds) const
{
    return Date(this->_date - seconds);
}

Date Date::operator-(const std::chrono::minutes &minutes) const
{
    return Date(this->_date - minutes);
}

Date Date::operator-(const std::chrono::hours &hours) const
{
    return Date(this->_date - hours);
}

Date Date::operator-(const std::chrono::days &days) const
{
    return Date(this->_date - days);
}

NODISCARD const std::chrono::system_clock::time_point &Date::toChrono() const
{
    return this->_date;
}

NODISCARD bsoncxx::v_noabi::types::b_date Date::toBSON() const
{
    return bsoncxx::types::b_date(this->_date);
}

NODISCARD google::protobuf::Timestamp Date::toProtobuf() const
{
    google::protobuf::Timestamp timestamp;

    this->_toProtobuf(timestamp);
    return timestamp;
}

NODISCARD google::protobuf::Timestamp *Date::toAllocatedProtobuf() const
{
    google::protobuf::Timestamp *timestamp = new google::protobuf::Timestamp();

    this->_toProtobuf(*timestamp);
    return timestamp;
}

NODISCARD google::protobuf::Duration Date::toDuration(const Date &from) const
{
    google::protobuf::Duration duration;

    this->_toDuration(duration, from);
    return duration;
}

NODISCARD google::protobuf::Duration *Date::toAllocatedDuration(const Date &from) const
{
    google::protobuf::Duration *duration = new google::protobuf::Duration();

    this->_toDuration(*duration, from);
    return duration;
}

void Date::_toProtobuf(google::protobuf::Timestamp &timestamp) const
{
    const auto &time = this->_date.time_since_epoch();
    const auto &seconds = std::chrono::duration_cast<std::chrono::seconds>(time);

    timestamp.set_seconds(seconds.count());
    timestamp.set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(time - seconds).count()));
}

void Date::_toDuration(google::protobuf::Duration &duration, const Date &from) const
{
    const auto &difference = this->_date > from.toChrono() ? this->_date - from.toChrono() : from.toChrono() - this->_date;
    const auto &seconds = std::chrono::duration_cast<std::chrono::seconds>(difference);

    duration.set_seconds(seconds.count());
    duration.set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(difference - seconds).count()));
}

std::ostream &operator<<(std::ostream &stream, const Date &date)
{
    std::time_t time = std::chrono::system_clock::to_time_t(date.toChrono());

    stream << std::put_time(std::localtime(&time), "%d-%m-%Y %H:%M:%S");
    return stream;
}