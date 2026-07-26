#ifndef REMINDER_H
#define REMINDER_H

#include "TaskManager.h"

#include <atomic>
#include <thread>

class Reminder {
public:
    explicit Reminder(TaskManager& taskManager);
    ~Reminder();

    void start();
    void stop();

private:
    TaskManager& taskManager;
    std::atomic<bool> running;
    std::thread worker;
};

#endif
