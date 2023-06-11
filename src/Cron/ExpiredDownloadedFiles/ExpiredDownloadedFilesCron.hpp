/**
 * @file ExpiredDownloadedFilesCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_EXPIREDDOWNLOADEDFILES_CRON_HPP
#define MAESTRO_EXPIREDDOWNLOADEDFILES_CRON_HPP

#include "schemas.hpp"
#include "Cron/Template/TemplateCron.hpp"

class ExpiredDownloadedFilesCron : public TemplateCron {
  public:
    ExpiredDownloadedFilesCron(const EventsManager &events);

    void run() override;

  private:
    const EventsManager &_events;
};

#endif