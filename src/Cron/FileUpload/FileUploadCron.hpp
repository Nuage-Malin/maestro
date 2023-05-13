/**
 * @file FileUploadCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_FILEUPLOAD_CRON_HPP
#define MAESTRO_FILEUPLOAD_CRON_HPP

#include "schemas.hpp"
#include "clients.hpp"
#include "EventsManager.hpp"
#include "Cron/Template/TemplateCron.hpp"

class FileUploadCron : public TemplateCron {
  public:
    FileUploadCron(FilesSchemas &filesSchemas, GrpcClients &grpcClient, EventsManager &events);

    void run() override;

  private:
    void _uploadFiles(const string &diskId);

  private:
    FilesSchemas &_filesSchemas;
    GrpcClients &_clients;
};

#endif