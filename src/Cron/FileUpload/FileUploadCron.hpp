/**
 * @file FileUploadCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_FILEUPLOAD_CRON_HPP
#define MAESTRO_FILEUPLOAD_CRON_HPP

#include "Cron/Template/TemplateCron.hpp"

class FileUploadCron : public TemplateCron {
  public:
    FileUploadCron();

    void run() override;
};

#endif