#include "Reminder.h"

#include <chrono>

Reminder::Reminder(TaskManager& taskManager)
    : taskManager(taskManager),
      running(false) {}

Reminder::~Reminder() {
    stop();
}

void Reminder::start() {
    if (running) {
        return;
    }

    running = true;

    worker = std::thread([this]() {
        while (running) {
            taskManager.checkAndRemindTasks();

            for (int i = 0; i < 5 && running; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    });
}

void Reminder::stop() {
    if (!running) {
        return;
    }

    running = false;

    if (worker.joinable()) {
        worker.join();
    }
}
