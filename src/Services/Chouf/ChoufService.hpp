/**
 * @file ChoufService.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 01/07/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_CHOUF_SERVICE_HPP
#define MAESTRO_CHOUF_SERVICE_HPP

#include <utility>
#include <mongocxx/database.hpp>

#include "Chouf_Maestro/Chouf_Maestro.grpc.pb.h"

#include "clients.hpp"
#include "schemas.hpp"
#include "Services/Template/TemplateService.hpp"
#include "Cron/Manager/ManagerCron.hpp"

class ChoufService : public TemplateService, public Chouf_Maestro::Chouf_Maestro_Service::Service {
  public:
    ChoufService(const GrpcClients &clients, const EventsManager &events, ManagerCron &managerCron);
    ~ChoufService() = default;

    grpc::Status getCronStatus(
        grpc::ServerContext *context, const Chouf_Maestro::GetCronStatusRequest *request, Chouf_Maestro::GetCronStatus *response
    ) override;

    grpc::Status getJobs(
        grpc::ServerContext *context, const Chouf_Maestro::GetJobsRequest *request, Chouf_Maestro::GetJobsStatus *response
    ) override;

    grpc::Status getJob(
        grpc::ServerContext *context, const Chouf_Maestro::GetJobRequest *request, Chouf_Maestro::GetJobStatus *response
    ) override;

    grpc::Status forceStartJobTask(
        grpc::ServerContext *context, const Chouf_Maestro::ForceStartJobTaskRequest *request,
        Chouf_Maestro::ForceStartJobTaskStatus *response
    ) override;

    grpc::Status changeCronState(
        grpc::ServerContext *context, const Chouf_Maestro::ChangeCronStateRequest *request,
        Chouf_Maestro::ChangeCronStateStatus *response
    ) override;

    grpc::Status changeJobState(
        grpc::ServerContext *context, const Chouf_Maestro::ChangeJobStateRequest *request,
        Chouf_Maestro::ChangeJobStateStatus *response
    ) override;

    grpc::Status changeMultipleInstancesState(
        grpc::ServerContext *context, const Chouf_Maestro::ChangeMultipleInstancesStateRequest *request,
        Chouf_Maestro::ChangeMultipleInstancesStateStatus *response
    ) override;

    grpc::Status waitTasks(
        grpc::ServerContext *context, const Chouf_Maestro::WaitTasksRequest *request, Chouf_Maestro::WaitTasksStatus *response
    ) override;

  private:
    void _convertJobIntoProtobuf(const std::pair<string, std::shared_ptr<TemplateCron>> &source, Cron::Job &dest);

  private:
    const GrpcClients &_clients;
    ManagerCron &_managerCron;
};

#endif