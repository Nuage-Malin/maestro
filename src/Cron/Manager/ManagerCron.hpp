/**
 * @file ManagerCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 05/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_MANAGERCRON_HPP
#define MAESTRO_MANAGERCRON_HPP

#include <functional>
#include <unordered_map>
#include <thread>

#include <libcron/Cron.h>

#include "utils.hpp"

typedef std::function<void()> CronTask;

struct CronTaskInfo
{
    string schedule;
    CronTask task;
    bool isPaused;
    std::vector<std::thread> running;
};

class ManagerCron {
  public:
    ManagerCron(bool allowMultipleInstances = false);
    ~ManagerCron();

    void run(const string &name);
    void add(const string &name, const string &schedule, CronTask &&task);
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

    void _start();

  private:
    void _checkStoppedTasks();

  private:
    libcron::Cron<libcron::LocalClock, libcron::NullLock> _cron;
    std::thread _runner;
    std::unordered_map<string, CronTaskInfo> _tasks;
    std::mutex _runningTasksMutex;
    bool _isPaused = false;
    bool _allowMultipleInstances;
};

#endif