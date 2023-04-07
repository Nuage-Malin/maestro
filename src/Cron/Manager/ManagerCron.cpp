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
        if (!task.second.running.empty()) {
            std::cerr << "[WARNING] CronManager stopped while task " << task.first << " was running "
                      << task.second.running.size() << " instances" << std::endl;
            for (auto &runningTask : task.second.running)
                runningTask.thread.detach();
        }

    if (this->_runner.joinable())
        this->_runner.join();
}

void ManagerCron::run(const string &name)
{
    CronTaskInfo &taskInfo = this->_tasks.at(name);

    if (!this->_allowMultipleInstances && taskInfo.running.size() > 0)
        std::cerr << "[WARNING] CronManager task " << name << " forced to run while already running" << std::endl;
    if (this->_isPaused || taskInfo.isPaused)
        std::cerr << "[WARNING] CronManager task " << name << " forced to run while paused" << std::endl;

    this->_runningTasksMutex.lock();
    taskInfo.running.push_back({.isRunning = std::make_unique<std::atomic_bool>(true)});
    CronRunningTask &runningTask = taskInfo.running.back();
    this->_runningTasksMutex.unlock();

    runningTask.thread = std::thread(
        [name](std::atomic_bool &isRunning, const CronTask &task) {
            std::cout << "CRON start: " << name << std::endl;
            task();

            isRunning = false;
            std::cout << "CRON finished: " << name << std::endl;
        },
        std::ref(*runningTask.isRunning),
        std::ref(taskInfo.task)
    );
}

void ManagerCron::add(const string &name, const string &schedule, const CronTask &&task)
{
    if (this->_tasks.find(name) != this->_tasks.end())
        throw std::runtime_error("CronManager task " + name + " already exists");
    this->_tasks[name] = {.schedule = schedule, .task = task, .isPaused = false, .running = std::vector<CronRunningTask>()};

    this->_cron.add_schedule(name, schedule, [this](const libcron::TaskInformation &taskInfo) {
        const CronTaskInfo &cronTaskInfo = this->_tasks.at(taskInfo.get_name());

        if (!this->_isPaused && !cronTaskInfo.isPaused) {
            if (!this->_allowMultipleInstances && cronTaskInfo.running.size() > 0) {
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
    std::vector<CronRunningTask> &runningTasks = this->_tasks.at(name).running;

    if (runningTasks.size() > 0) {
        std::cerr << "[WARNING] CronManager task " << name << " removed while running " << runningTasks.size() << " instances"
                  << std::endl;
        for (auto &runningTask : runningTasks)
            runningTask.thread.detach();
    }
    this->_tasks.erase(name);
    this->_runningTasksMutex.unlock();
}

void ManagerCron::resume()
{
    if (!this->_isPaused)
        throw std::runtime_error("CronManager is already running");
    this->_isPaused = false;

    if (this->_runner.joinable())
        this->_runner.join();
    this->_start();
}

void ManagerCron::resume(const string &name)
{
    CronTaskInfo &taskInfo = this->_tasks.at(name);

    if (!taskInfo.isPaused)
        throw std::runtime_error("CronManager task " + name + " is already running");
    taskInfo.isPaused = false;
}

void ManagerCron::resumeAll()
{
    for (auto &task : this->_tasks)
        task.second.isPaused = false;

    if (this->_isPaused)
        this->resume();
}

void ManagerCron::pause()
{
    if (this->_isPaused)
        throw std::runtime_error("CronManager is already paused");
    this->_isPaused = true;
}

void ManagerCron::pause(const string &name)
{
    CronTaskInfo &taskInfo = this->_tasks.at(name);

    if (taskInfo.isPaused)
        throw std::runtime_error("CronManager task " + name + " is already paused");
    taskInfo.isPaused = true;
}

void ManagerCron::wait()
{
    this->_checkStoppedTasks();

    this->_runningTasksMutex.lock();
    for (auto &task : this->_tasks)
        for (auto &running : task.second.running)
            if (running.thread.joinable())
                running.thread.join();
    this->_runningTasksMutex.unlock();
}

void ManagerCron::wait(const string &name)
{
    this->_checkStoppedTasks();

    this->_runningTasksMutex.lock();
    for (auto &runningTask : this->_tasks.at(name).running)
        if (runningTask.thread.joinable())
            runningTask.thread.join();
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
            std::cout << "CRON running" << std::endl;
            this->_checkStoppedTasks();
            this->_cron.tick();

            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
    });
}

void ManagerCron::_checkStoppedTasks()
{
    std::unordered_multimap<string, std::vector<CronRunningTask>::const_iterator> toRemove;

    this->_runningTasksMutex.lock();
    for (auto &task : this->_tasks)
        for (auto it = task.second.running.begin(); it != task.second.running.end(); it++)
            if (!*it->isRunning) {
                if (it->thread.joinable())
                    it->thread.join();
                toRemove.insert({task.first, it});
            }

    for (auto &task : toRemove)
        this->_tasks[task.first].running.erase(task.second);
    this->_runningTasksMutex.unlock();
}