/**
 * @file FileUploadCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_FILEUPLOAD_CRON_HPP
#define MAESTRO_FILEUPLOAD_CRON_HPP

#include "clients.hpp"
#include "EventsManager.hpp"
#include "Cron/Template/TemplateCron.hpp"
#include "Schemas/Files/UploadQueue/UploadQueueSchema.hpp"

class FileUploadCron : public TemplateCron {
  public:
    FileUploadCron(const mongocxx::database &filesDatabase, GrpcClients &grpcClient, EventsManager &events);

    void run() override;

  private:
    void _uploadFiles(const string &diskId);

  private:
    UploadQueueSchema _uploadQueue;
    GrpcClients &_clients;
};

#endif