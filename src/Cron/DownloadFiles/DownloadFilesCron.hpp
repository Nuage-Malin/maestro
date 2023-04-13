/**
 * @file DownloadFilesCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_DOWNLOADFILES_CRON_HPP
#define MAESTRO_DOWNLOADFILES_CRON_HPP

#include "schemas.hpp"
#include "clients.hpp"
#include "Cron/Template/TemplateCron.hpp"

class DownloadFilesCron : public TemplateCron {
  public:
    DownloadFilesCron(FilesSchemas &filesSchemas, GrpcClients &grpcClient, EventsManager &events);

    void run() override;

  private:
    void _downloadDiskFiles(const string &diskId);

  private:
    FilesSchemas &_filesSchemas;
    GrpcClients &_clients;
};

#endif