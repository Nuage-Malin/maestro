/**
 * @file EventsManager.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 09/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_EVENTS_MANAGER_HPP
#define MAESTRO_EVENTS_MANAGER_HPP

#include <functional>
#include <map>
#include <any>

#include "utils.hpp"

enum Event
{
    DISK_STARTUP,
    DISK_SHUTDOWN,
    DISK_SHUTDOWN_BEFORE
};

class EventsManager {
  public:
    EventsManager() = default;
    ~EventsManager() = default;

    /**
     * @brief Forwards to emit. emit() should be preferred if parameters are added to add their templates.
     *
     * @tparam Args
     * @param event
     * @param args
     */
    template <typename... Args> void operator()(const Event &event, Args &&...args) const;

    template <typename... Args> void emit(const Event &event, Args &&...args) const;

    template <typename... Args> void on(const Event &event, const std::function<void(Args...)> &&callback);

  private:
    std::map<Event, std::vector<std::any>> _events;
};

template <typename... Args> void EventsManager::operator()(const Event &event, Args &&...args) const
{
    this->emit<Args...>(event, std::forward<Args>(args)...);
}

template <typename... Args> void EventsManager::emit(const Event &event, Args &&...args) const
{
    try {
        for (const std::any &callbacks : this->_events.at(event))
            std::any_cast<std::function<void(Args...)>>(callbacks)(std::forward<Args>(args)...);
    } catch (const std::bad_any_cast &error) {
        std::cerr << "[" << __FUNCTION__ << "] Invalid event parameters template: " << error.what() << std::endl;
        throw std::bad_any_cast(error);
    }
}

template <typename... Args> void EventsManager::on(const Event &event, const std::function<void(Args...)> &&callback)
{
    this->_events[event].push_back(std::make_any<std::function<void(Args...)>>([callback](Args &&...args) {
        callback(std::forward<Args>(args)...);
    }));
}

#endif