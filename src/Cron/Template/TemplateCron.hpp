/**
 * @file TemplateCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 04/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_TEMPLATE_CRON_HPP
#define MAESTRO_TEMPLATE_CRON_HPP

#include <thread>

#include "utils.hpp"

struct CronTask
{
    std::thread thread;
    std::unique_ptr<std::atomic_bool> isRunning;
};

class TemplateCron {
  public:
    TemplateCron(const string &name);
    TemplateCron(const TemplateCron &other);
    ~TemplateCron(); // wait() should be called before the destructor

    TemplateCron &operator=(const TemplateCron &job);

    virtual void onStart();  // Called before run
    virtual void onFinish(); // Called after run
    virtual void run() = 0;  // Called every schedule
    virtual void onPause();  // Called when the job or the cron manager is paused
    virtual void onResume(); // Called when the job or the cron manager is resumed
    virtual void onAdd();    // Called when the task is added to the cron
    virtual void onRemove(); // Called when the task is removed from the cron
    NODISCARD const string &getName() const;
    void wait(); // Wait is blocking for all tasks to finish (This method should be called before the destructor)

    // This methods should only be called by the cron manager
    void resume(bool callEvent = true);
    void pause();
    // Return true if the job is paused.
    // If the cron manager is paused, this method still depend on the job state, not the global one.
    NODISCARD const bool &isPaused() const;
    void addRunningTask();
    NODISCARD std::vector<CronTask> &getRunningTasks();
    NODISCARD const std::vector<CronTask> &getRunningTasks() const;
    void checkStoppedTasks();

  protected:
    string _name;
    bool _isPaused = false; // Reffer to isPaused() comment
    std::vector<CronTask> _tasks;

  private:
    std::mutex _mutex;
};

#endif