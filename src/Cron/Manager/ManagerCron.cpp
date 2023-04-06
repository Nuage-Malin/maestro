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

    for (auto &task : _tasks)
        if (!task.second.running.empty())
            std::cerr << "[WARNING] CronManager stopped while task " << task.first << " was running "
                      << task.second.running.size() << " instances" << std::endl;

    if (this->_runner.joinable())
        this->_runner.join();
}

void ManagerCron::run(const string &name)
{
    if (!this->_allowMultipleInstances)
        std::cerr << "[WARNING] CronManager task " << name << " forced to run while already running" << std::endl;
    CronTaskInfo &taskInfo = this->_tasks.at(name);

    if (this->_isPaused || taskInfo.isPaused)
        std::cerr << "[WARNING] CronManager task " << name << " forced to run while paused" << std::endl;

    this->_runningTasksMutex.lock();
    taskInfo.running.push_back(std::thread(taskInfo.task));
    this->_runningTasksMutex.unlock();
    std::cout << "CRON started: " << name << std::endl;
}

void ManagerCron::add(const string &name, const string &schedule, CronTask &&task)
{
    if (this->_tasks.find(name) != this->_tasks.end())
        throw std::runtime_error("CronManager task " + name + " already exists");
    this->_tasks[name] = {.schedule = schedule, .task = task, .isPaused = false, .running = std::vector<std::thread>()};

    this->_cron.add_schedule(name, schedule, [this](const libcron::TaskInformation &taskInfo) {
        if (!this->_isPaused && !this->_tasks.at(taskInfo.get_name()).isPaused) {
            if (!this->_allowMultipleInstances && this->_tasks.at(taskInfo.get_name()).running.size() > 0) {
                std::cerr << "[WARNING] CronManager task " << taskInfo.get_name() << " skipped. Task already running."
                          << std::endl;
                return;
            }

            this->run(taskInfo.get_name());
        }
    });
}

void ManagerCron::remove(const string &name)
{
    this->_cron.remove_schedule(name);
    this->_runningTasksMutex.lock();
    const size_t &taskInstances = this->_tasks.at(name).running.size();

    if (taskInstances > 0)
        std::cerr << "[WARNING] CronManager task " << name << " removed while running " << taskInstances << " instances"
                  << std::endl;
    this->_tasks.erase(name);
    this->_runningTasksMutex.unlock();
}

void ManagerCron::resume()
{
    this->_isPaused = false;

    this->_start();
}

void ManagerCron::resume(const string &name)
{
    this->_tasks.at(name).isPaused = false;
}

void ManagerCron::resumeAll()
{
    for (auto &task : this->_tasks)
        task.second.isPaused = false;

    this->resume();
}

void ManagerCron::pause()
{
    this->_isPaused = true;
}

void ManagerCron::pause(const string &name)
{
    this->_tasks.at(name).isPaused = true;
}

void ManagerCron::wait()
{
    this->_checkStoppedTasks();

    this->_runningTasksMutex.lock();
    for (auto &task : this->_tasks)
        for (auto &thread : task.second.running)
            if (thread.joinable())
                thread.join();
    this->_runningTasksMutex.unlock();
}

void ManagerCron::wait(const string &name)
{
    this->_checkStoppedTasks();
    this->_runningTasksMutex.lock();
    std::vector<std::thread> &runningTasks = this->_tasks.at(name).running;

    for (auto &thread : runningTasks)
        if (thread.joinable())
            thread.join();
    this->_runningTasksMutex.unlock();
}

NODISCARD size_t ManagerCron::getRunningTasksCount(const string &name) const
{
    return this->_tasks.at(name).running.size();
}

NODISCARD size_t ManagerCron::getTotalRunningTasksCount() const
{
    return this->_cron.count();
}

void ManagerCron::_start()
{
    this->_runner = std::thread([this]() {
        while (!this->_isPaused) {
            this->_checkStoppedTasks();
            this->_cron.tick();

            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });
}

void ManagerCron::_checkStoppedTasks()
{
    std::unordered_multimap<string, std::vector<std::thread>::const_iterator> toRemove;

    this->_runningTasksMutex.lock();
    for (const auto &task : this->_tasks)
        for (auto it = task.second.running.begin(); it != task.second.running.end(); it++)
            if (!it->joinable())
                toRemove.insert({task.first, it});

    for (auto &task : toRemove)
        this->_tasks[task.first].running.erase(task.second);
    this->_runningTasksMutex.unlock();
}