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

    std::cout << "CRON start: " << name << std::endl;
    job.addRunningTask();
}

void ManagerCron::add(const string &schedule, const std::shared_ptr<TemplateCron> &job)
{
    for (auto &job : this->_jobs)
        if (job->getName() == job->getName())
            throw std::runtime_error("CronManager job " + job->getName() + " already exists");
    this->_jobs.push_back(job);

    this->_cron.add_schedule(job->getName(), schedule, [this](const libcron::TaskInformation &taskInfo) {
        const TemplateCron &job = this->_getJob(taskInfo.get_name());

        if (!this->_isPaused && !job.isPaused()) {
            if (!this->_allowMultipleInstances && job.getRunningTasks().size() > 0) {
                std::cerr << "[WARNING] CronManager task " << taskInfo.get_name() << " skipped. Task already running."
                          << std::endl;
                return;
            }

            this->run(taskInfo.get_name());
        }
    });
    job->onAdd();
}

void ManagerCron::remove(const string &name)
{
    this->_cron.remove_schedule(name);
    const auto &it = std::find_if(this->_jobs.begin(), this->_jobs.end(), [&name](const std::shared_ptr<TemplateCron> &job) {
        return job->getName() == name;
    });

    if (it == this->_jobs.end())
        throw std::runtime_error("CronManager task " + name + " not found");
    (*it)->onRemove();
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
        job->onResume();
}

void ManagerCron::resume(const string &name)
{
    this->_getJob(name).resume();
}

void ManagerCron::resumeAll()
{
    for (auto &job : this->_jobs)
        if (job->isPaused())
            job->resume(false);

    if (this->_isPaused)
        this->resume();
}

void ManagerCron::pause()
{
    if (this->_isPaused)
        throw std::runtime_error("CronManager is already paused");

    this->_isPaused = true;
    for (auto &job : this->_jobs)
        job->onPause();
}

void ManagerCron::pause(const string &name)
{
    this->_getJob(name).pause();
}

void ManagerCron::wait()
{
    for (auto &job : this->_jobs)
        job->wait();
}

void ManagerCron::wait(const string &name)
{
    this->_getJob(name).wait();
}

NODISCARD size_t ManagerCron::getRunningTasksCount(const string &name) const
{
    return this->_getJob(name).getRunningTasks().size();
}

NODISCARD size_t ManagerCron::getTotalRunningTasksCount() const
{
    return this->_cron.count();
}

void ManagerCron::_start()
{
    this->_runner = std::thread([this]() {
        while (!this->_isPaused) {
            std::cout << "CRON running" << std::endl;
            this->_checkStoppedTasks();
            this->_cron.tick();

            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    });
}

void ManagerCron::_checkStoppedTasks()
{
    for (auto &job : this->_jobs)
        job->checkStoppedTasks();
}

NODISCARD TemplateCron &ManagerCron::_getJob(const string &name)
{
    for (auto &job : this->_jobs)
        if (job->getName() == name)
            return *job;

    throw std::runtime_error("CronManager task " + name + " not found");
}

const TemplateCron &ManagerCron::_getJob(const string &name) const
{
    for (auto &job : this->_jobs)
        if (job->getName() == name)
            return *job;

    throw std::runtime_error("CronManager task " + name + " not found");
}