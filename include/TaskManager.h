#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Task.h"

#include <mutex>
#include <string>
#include <vector>

class TaskManager {
public:
    explicit TaskManager(const std::string& username);

    bool loadTasks();
    bool saveTasks() const;

    bool addTask(
        const std::string& name,
        const std::string& startTime,
        const std::string& priority = "medium",
        const std::string& category = "life",
        const std::string& remindTime = ""
    );

    bool deleteTaskById(int id);
    bool markTaskDone(int id);
    bool editTask(int id, const std::string& field, const std::string& value);

    void showTasks(
        const std::string& mode = "all",
        const std::string& date = ""
    );

    void searchTasks(const std::string& keyword);

    void checkAndRemindTasks();

    const std::vector<Task>& getTasks() const;

    std::string getTaskFilePath() const;

private:
    std::string username;
    std::string taskFilePath;
    std::vector<Task> tasks;

    mutable std::mutex taskMutex;

    int getNextId() const;

    bool isStartTimeUnique(const std::string& startTime) const;
    bool isStartTimeUniqueExceptId(const std::string& startTime, int id) const;

    bool isNameAndStartTimeUnique(
        const std::string& name,
        const std::string& startTime
    ) const;

    bool isNameAndStartTimeUniqueExceptId(
        const std::string& name,
        const std::string& startTime,
        int id
    ) const;

    void sortTasksByStartTime();
    bool loadTasksUnlocked();
    bool saveTasksUnlocked() const;

    void printTaskTable(const std::vector<Task>& taskList) const;
};

#endif
