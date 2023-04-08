/**
 * @file ManagerCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_MANAGERCRON_HPP
#define MAESTRO_MANAGERCRON_HPP

#include <unordered_map>

#include <libcron/Cron.h>

#include "Cron/Template/TemplateCron.hpp"

class ManagerCron {
  public:
    ManagerCron(bool allowMultipleInstances = false);
    ~ManagerCron();

    void run(const string &name);
    void add(const string &schedule, const std::shared_ptr<TemplateCron> &job);
    void remove(const string &name);
    void resume(); // Resume the global state
    void resume(const string &name);
    void resumeAll();               // Resume the global state and all tasks
    void pause();                   // Pause the global state
    void pause(const string &name); // Pause a specific task
    void wait(); // Wait is blocking for all tasks to finish (This method should be called before the destructor)
    void wait(const string &name); // Wait is blocking for a specific task to finish
    NODISCARD size_t getRunningTasksCount(const string &name) const;
    NODISCARD size_t getTotalRunningTasksCount() const;

  private:
    void _start();
    void _checkStoppedTasks();
    NODISCARD TemplateCron &_getJob(const string &name);
    NODISCARD const TemplateCron &_getJob(const string &name) const;

  private:
    libcron::Cron<libcron::LocalClock, libcron::NullLock> _cron;
    std::thread _runner;
    std::vector<std::shared_ptr<TemplateCron>> _jobs;
    bool _isPaused = false;
    bool _allowMultipleInstances;
};

#endif