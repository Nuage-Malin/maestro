/**
 * @file TemplateCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 06/04/2023
 * @copyright Nuage Malin
 */

#include "TemplateCron.hpp"

TemplateCron::TemplateCron(const string &name) : _name(name)
{
}

TemplateCron::TemplateCron(const TemplateCron &other) : _name(other._name), _isPaused(other._isPaused)
{
}

TemplateCron::~TemplateCron()
{
    if (!this->_tasks.empty()) {
        std::cerr << "[WARNING] CRON " << this->_name << " stopped while " << this->_tasks.size() << " instances was running"
                  << std::endl;
        for (auto &task : this->_tasks)
            task.thread.detach();
    }
}

TemplateCron &TemplateCron::operator=(const TemplateCron &job)
{
    this->_name = job._name;
    this->_isPaused = job._isPaused;

    return *this;
}

void TemplateCron::onStart()
{
}

void TemplateCron::onFinish()
{
}

void TemplateCron::onPause()
{
}

void TemplateCron::onResume()
{
}

void TemplateCron::onAdd()
{
}

void TemplateCron::onRemove()
{
}

const string &TemplateCron::getName() const
{
    return this->_name;
}

void TemplateCron::wait()
{
    this->checkStoppedTasks();

    this->_mutex.lock();
    for (auto &task : this->_tasks)
        if (task.thread.joinable())
            task.thread.join();
    this->_mutex.unlock();

    this->checkStoppedTasks();
}

void TemplateCron::resume(bool callEvent)
{
    if (!this->_isPaused)
        throw std::runtime_error("CRON " + this->_name + " is already running");

    this->_isPaused = false;
    if (callEvent)
        this->onResume();
}

void TemplateCron::pause()
{
    if (this->_isPaused)
        throw std::runtime_error("CRON " + this->_name + " is already paused");

    this->_isPaused = true;
    this->onPause();
}

const bool &TemplateCron::isPaused() const
{
    return this->_isPaused;
}

void TemplateCron::addRunningTask()
{
    this->_mutex.lock();
    this->_tasks.push_back({.isRunning = std::make_unique<std::atomic_bool>(true)});
    CronTask &task = this->_tasks.back();
    this->_mutex.unlock();

    task.thread = std::thread(
        [this](std::atomic_bool &isRunning) {
            this->onStart();
            std::cout << "CRON start: " << this->_name << std::endl;
            this->run();

            isRunning = false;
            this->onFinish();
            std::cout << "CRON finished: " << this->_name << std::endl;
        },
        std::ref(*task.isRunning)
    );
}

std::vector<CronTask> &TemplateCron::getRunningTasks()
{
    return this->_tasks;
}

const std::vector<CronTask> &TemplateCron::getRunningTasks() const
{
    return this->_tasks;
}

void TemplateCron::checkStoppedTasks()
{
    std::vector<std::vector<CronTask>::iterator> toRemove;

    this->_mutex.lock();
    for (auto it = this->_tasks.begin(); it != this->_tasks.end(); it++)
        if (!*it->isRunning) {
            if (it->thread.joinable())
                it->thread.join();
            toRemove.push_back(it);
        }

    for (auto &task : toRemove)
        this->_tasks.erase(task);
    this->_mutex.unlock();
}