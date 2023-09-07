/**
 * @file ManagerCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/04/2023
 * @copyright Nuage Malin
 */

#include "ManagerCron.hpp"

ManagerCron::ManagerCron(bool allowMultipleInstances) : _allowMultipleInstances(allowMultipleInstances)
{
    this->_start();
}

ManagerCron::~ManagerCron()
{
    this->_isPaused = true;
    this->_checkStoppedTasks();

    if (this->_runner.joinable())
        this->_runner.join();
}

void ManagerCron::run(const string &name)
{
    TemplateCron &job = this->_getJob(name);

    if (!this->_allowMultipleInstances && job.getRunningTasks().size() > 0)
        std::cerr << "[WARNING] CronManager job " << name << " forced to run while already running" << std::endl;
    if (this->_isPaused || job.isPaused())
        std::cerr << "[WARNING] CronManager job " << name << " forced to run while paused" << std::endl;

    job.addRunningTask();
}

void ManagerCron::remove(const string &name)
{
    this->_cron.remove_schedule(name);
    const auto &it = std::find_if(
        this->_jobs.begin(),
        this->_jobs.end(),
        [&name](const std::pair<string, std::shared_ptr<TemplateCron>> &job) {
            return job.second->getName() == name;
        }
    );

    if (it == this->_jobs.end())
        throw std::runtime_error("CronManager task " + name + " not found");
    it->second->onRemove();
    this->_jobs.erase(it);
}

void ManagerCron::resume()
{
    if (!this->_isPaused)
        throw std::runtime_error("CronManager is already running");
    this->_isPaused = false;

    if (this->_runner.joinable())
        this->_runner.join();
    this->_start();
    for (auto &job : this->_jobs)
        job.second->onResume();
}

void ManagerCron::resume(const string &name)
{
    this->_getJob(name).resume();
}

void ManagerCron::resumeAll()
{
    for (auto &job : this->_jobs)
        if (job.second->isPaused())
            job.second->resume(false);

    if (this->_isPaused)
        this->resume();
}

void ManagerCron::pause()
{
    if (this->_isPaused)
        throw std::runtime_error("CronManager is already paused");

    this->_isPaused = true;
    for (auto &job : this->_jobs)
        job.second->onPause();
}

void ManagerCron::pause(const string &name)
{
    this->_getJob(name).pause();
}

NODISCARD const bool &ManagerCron::isPaused() const
{
    return this->_isPaused;
}

void ManagerCron::wait()
{
    for (auto &job : this->_jobs)
        job.second->wait();
}

void ManagerCron::wait(const string &name)
{
    this->_getJob(name).wait();
}

NODISCARD size_t ManagerCron::getTotalJobsCount() const
{
    return this->_jobs.size();
}

NODISCARD size_t ManagerCron::getRunningTasksCount(const string &name) const
{
    return this->getJob(name).second->getRunningTasks().size();
}

NODISCARD size_t ManagerCron::getTotalRunningTasksCount() const
{
    return this->_cron.count();
}

NODISCARD const bool &ManagerCron::doesAllowMultipleInstances() const
{
    return this->_allowMultipleInstances;
}

void ManagerCron::setAllowMultipleInstances(const bool &allowMultipleInstances)
{
    this->_allowMultipleInstances = allowMultipleInstances;
}

NODISCARD const std::vector<std::pair<string, std::shared_ptr<TemplateCron>>> &ManagerCron::getJobs() const
{
    return this->_jobs;
}

void ManagerCron::_start()
{
    this->_runner = std::thread([this]() {
        while (!this->_isPaused) {
            std::cout << "Check CRON" << std::endl;
            this->_checkStoppedTasks();
            this->_cron.tick();

            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });
}

void ManagerCron::_checkStoppedTasks()
{
    for (auto &job : this->_jobs)
        job.second->checkStoppedTasks();
}

NODISCARD TemplateCron &ManagerCron::_getJob(const string &name)
{
    for (const auto &job : this->_jobs)
        if (job.second->getName() == name)
            return *job.second;

    throw std::runtime_error("CronManager task " + name + " not found");
}

const std::pair<string, std::shared_ptr<TemplateCron>> &ManagerCron::getJob(const string &name) const
{
    for (const auto &job : this->_jobs)
        if (job.second->getName() == name)
            return job;

    throw std::runtime_error("CronManager task " + name + " not found");
}