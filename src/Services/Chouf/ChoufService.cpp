/**
 * @file ChoufService.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 01/07/2023
 * @copyright Nuage Malin
 */

#include "schemas.hpp"
#include "ChoufService.hpp"
#include "Exceptions/NotFound/NotFoundException.hpp"

ChoufService::ChoufService(const GrpcClients &clients, EventsManager &events, ManagerCron &managerCron)
    : TemplateService(events), _clients(clients), _managerCron(managerCron)
{
}

grpc::Status ChoufService::getCronStatus(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::GetCronStatusRequest *request,
    Chouf_Maestro::GetCronStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            response->set_ispaused(this->_managerCron.isPaused());
            response->set_doesallowmultipleinstances(this->_managerCron.doesAllowMultipleInstances());
            response->set_totalnbrjobs(this->_managerCron.getTotalJobsCount());
            response->set_totalnbrtasks(this->_managerCron.getTotalRunningTasksCount());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::getJobs(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::GetJobsRequest *request, Chouf_Maestro::GetJobsStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            for (const auto &job : this->_managerCron.getJobs()) {
                auto *jobStatus = response->add_jobs();

                this->_convertJobIntoProtobuf(job, *jobStatus);
            }

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::getJob(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::GetJobRequest *request, Chouf_Maestro::GetJobStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            auto *jobResponse = new Cron::Job();

            this->_convertJobIntoProtobuf(this->_managerCron.getJob(request->name()), *jobResponse);
            response->set_allocated_job(jobResponse);
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::forceStartJobTask(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::ForceStartJobTaskRequest *request,
    Chouf_Maestro::ForceStartJobTaskStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            this->_managerCron.run(request->name());
            auto *jobResponse = new Cron::Job();

            this->_convertJobIntoProtobuf(this->_managerCron.getJob(request->name()), *jobResponse);
            response->set_allocated_job(jobResponse);
            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::changeCronState(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::ChangeCronStateRequest *request,
    Chouf_Maestro::ChangeCronStateStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            if (request->ispaused())
                this->_managerCron.resume();
            else
                this->_managerCron.pause();

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::changeJobState(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::ChangeJobStateRequest *request,
    Chouf_Maestro::ChangeJobStateStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            if (request->ispaused())
                this->_managerCron.resume(request->name());
            else
                this->_managerCron.pause(request->name());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::changeMultipleInstancesState(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::ChangeMultipleInstancesStateRequest *request,
    Chouf_Maestro::ChangeMultipleInstancesStateStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            this->_managerCron.setAllowMultipleInstances(request->doesallowmultipleinstances());

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

grpc::Status ChoufService::waitTasks(
    UNUSED grpc::ServerContext *context, const Chouf_Maestro::WaitTasksRequest *request, Chouf_Maestro::WaitTasksStatus *response
)
{
    return this->_procedureRunner(
        [this, request, response](FilesSchemas &&, StatsSchemas &&) {
            if (request->has_name()) {
                response->set_totalnbrtasks(this->_managerCron.getRunningTasksCount(request->name()));
                this->_managerCron.wait(request->name());
            } else {
                response->set_totalnbrtasks(this->_managerCron.getTotalRunningTasksCount());
                this->_managerCron.wait();
            }

            return grpc::Status::OK;
        },
        __FUNCTION__
    );
}

void ChoufService::_convertJobIntoProtobuf(const std::pair<string, std::shared_ptr<TemplateCron>> &source, Cron::Job &dest)
{
    dest.set_name(source.second->getName());
    dest.set_schedule(source.first);
    dest.set_ispaused(source.second->isPaused());
    for (const auto &task : source.second->getRunningTasks())
        dest.add_tasks()->set_isrunning(*task.isRunning);
}
