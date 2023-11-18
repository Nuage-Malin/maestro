/**
 * @file ExpiredDownloadedFilesCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_EXPIREDDOWNLOADEDFILES_CRON_HPP
#define MAESTRO_EXPIREDDOWNLOADEDFILES_CRON_HPP

#include <tuple>

#include "clients.hpp"
#include "schemas.hpp"
#include "Cron/Template/TemplateCron.hpp"
#include "Utils/Date/Date.hpp"

class ExpiredDownloadedFilesCron : public TemplateCron {
  public:
    ExpiredDownloadedFilesCron(GrpcClients &grpcClient, EventsManager &events);
    ~ExpiredDownloadedFilesCron() = default;

    void run() override /* todo : I removed `override` keyword, check if it changes anything */;

  private:
    //    template <typename StrIterator>
    //        requires std::input_iterator<StrIterator>
    //        && std::same_as<typename std::iterator_traits<StrIterator>::value_type, string> // todo replace with macro
    void removeExpiredDownloadedFiles();

    GrpcClients &_clients;
    EventsManager &_events;
};

#endif