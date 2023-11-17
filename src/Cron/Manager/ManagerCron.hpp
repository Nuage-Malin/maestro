/**
 * @file ManagerCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_MANAGER_CRON_HPP
#define MAESTRO_MANAGER_CRON_HPP

#include <unordered_map>

#include <libcron/Cron.h>

#include "Cron/Template/TemplateCron.hpp"

class ManagerCron {
  public:
    ManagerCron(bool allowMultipleInstances = false);
    ~ManagerCron();

    void run(const string &name);
    template <typename T> void add(const string &schedule, const T &job);
    void remove(const string &name);
    void resume();                  // Resume the global state
    void resume(const string &name);
    void resumeAll();               // Resume the global state and all tasks
    void pause();                   // Pause the global state
    void pause(const string &name); // Pause a specific task
    NODISCARD const bool &isPaused() const;
    void wait(); // Wait is blocking for all tasks to finish (This method should be called before the destructor)
    void wait(const string &name); // Wait is blocking for a specific task to finish
    NODISCARD size_t getTotalJobsCount() const;
    NODISCARD size_t getRunningTasksCount(const string &name) const;
    NODISCARD size_t getTotalRunningTasksCount() const;
    NODISCARD const bool &doesAllowMultipleInstances() const;
    void setAllowMultipleInstances(const bool &allowMultipleInstances);
    NODISCARD const std::vector<std::pair<string, std::shared_ptr<TemplateCron>>> &getJobs() const;
    NODISCARD const std::pair<string, std::shared_ptr<TemplateCron>> &getJob(const string &name) const;

  private:
    void _start();
    void _checkStoppedTasks();
    NODISCARD TemplateCron &_getJob(const string &name);

  private:
    libcron::Cron<libcron::LocalClock, libcron::NullLock> _cron;
    std::thread _runner;
    std::vector<std::pair<string, std::shared_ptr<TemplateCron>>> _jobs;
    bool _isPaused = false;
    bool _allowMultipleInstances;
};

template <typename T> void ManagerCron::add(const string &schedule, const T &cronJob)
{
    for (auto &job : this->_jobs)
        if (cronJob.getName() == job.second->getName())
            throw std::runtime_error("CronManager job " + job.second->getName() + " already exists");
    this->_jobs.push_back(std::make_pair(schedule, std::make_shared<T>(cronJob)));

    this->_cron.add_schedule(cronJob.getName(), schedule, [this](const libcron::TaskInformation &taskInfo) {
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
    this->getJob(cronJob.getName()).second->onAdd();
}

#endif