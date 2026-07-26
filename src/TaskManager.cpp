#include "TaskManager.h"
#include "Utils.h"
#include "SoundReminder.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::string toLowerCopy(const std::string& text) {
    std::string result = text;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return result;
}

} // namespace

TaskManager::TaskManager(const std::string& username)
    : username(username),
      taskFilePath("data/tasks_" + username + ".txt") {
    Utils::ensureDataDirectory();
    loadTasks();
}

std::string TaskManager::getTaskFilePath() const {
    return taskFilePath;
}

bool TaskManager::loadTasks() {
    std::lock_guard<std::mutex> lock(taskMutex);
    return loadTasksUnlocked();
}

bool TaskManager::loadTasksUnlocked() {
    Utils::ensureDataDirectory();

    tasks.clear();

    std::ifstream file(taskFilePath);

    if (!file.is_open()) {
        return true;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        Task task;
        if (Task::deserialize(line, task)) {
            tasks.push_back(task);
        }
    }

    sortTasksByStartTime();

    return true;
}

bool TaskManager::saveTasks() const {
    std::lock_guard<std::mutex> lock(taskMutex);
    return saveTasksUnlocked();
}

bool TaskManager::saveTasksUnlocked() const {
    Utils::ensureDataDirectory();

    std::vector<Task> sortedTasks = tasks;

    std::sort(
        sortedTasks.begin(),
        sortedTasks.end(),
        [](const Task& a, const Task& b) {
            if (a.startTime != b.startTime) {
                return a.startTime < b.startTime;
            }

            return a.id < b.id;
        }
    );

    std::ofstream file(taskFilePath, std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Error: cannot open task file for writing: " << taskFilePath << "\n";
        return false;
    }

    for (const auto& task : sortedTasks) {
        file << task.serialize() << "\n";
    }

    file.flush();

    if (!file.good()) {
        std::cerr << "Error: failed while writing task file: " << taskFilePath << "\n";
        return false;
    }

    return true;
}

void TaskManager::sortTasksByStartTime() {
    std::sort(
        tasks.begin(),
        tasks.end(),
        [](const Task& a, const Task& b) {
            if (a.startTime != b.startTime) {
                return a.startTime < b.startTime;
            }

            return a.id < b.id;
        }
    );
}

int TaskManager::getNextId() const {
    int maxId = 0;

    for (const auto& task : tasks) {
        if (task.id > maxId) {
            maxId = task.id;
        }
    }

    return maxId + 1;
}

bool TaskManager::isStartTimeUnique(const std::string& startTime) const {
    for (const auto& task : tasks) {
        if (task.startTime == startTime) {
            return false;
        }
    }

    return true;
}

bool TaskManager::isStartTimeUniqueExceptId(const std::string& startTime, int id) const {
    for (const auto& task : tasks) {
        if (task.id != id && task.startTime == startTime) {
            return false;
        }
    }

    return true;
}

bool TaskManager::isNameAndStartTimeUnique(
    const std::string& name,
    const std::string& startTime
) const {
    for (const auto& task : tasks) {
        if (task.name == name && task.startTime == startTime) {
            return false;
        }
    }

    return true;
}

bool TaskManager::isNameAndStartTimeUniqueExceptId(
    const std::string& name,
    const std::string& startTime,
    int id
) const {
    for (const auto& task : tasks) {
        if (
            task.id != id &&
            task.name == name &&
            task.startTime == startTime
        ) {
            return false;
        }
    }

    return true;
}

bool TaskManager::addTask(
    const std::string& name,
    const std::string& startTime,
    const std::string& priority,
    const std::string& category,
    const std::string& remindTime
) {
    std::lock_guard<std::mutex> lock(taskMutex);

    Utils::ensureDataDirectory();

    if (name.empty() || startTime.empty()) {
        std::cerr << "Error: task name and start time cannot be empty.\n";
        return false;
    }

    if (!Utils::isValidDateTimeFormat(startTime)) {
        std::cerr << "Error: invalid start time format. Expected YYYY-MM-DD_HH:MM\n";
        return false;
    }

    std::string finalRemindTime = remindTime.empty() ? startTime : remindTime;

    if (!Utils::isValidDateTimeFormat(finalRemindTime)) {
        std::cerr << "Error: invalid remind time format. Expected YYYY-MM-DD_HH:MM\n";
        return false;
    }

    /*
     * Important:
     * Check name + startTime uniqueness before checking startTime uniqueness.
     * Otherwise, when both name and startTime are duplicated, the program would
     * only report "same start time" and never show the required message.
     */
    if (!isNameAndStartTimeUnique(name, startTime)) {
        std::cerr << "Error: 任务名称——开始时间必须具有单一性.\n";
        return false;
    }

    if (!isStartTimeUnique(startTime)) {
        std::cerr << "Error: another task already has the same start time: " << startTime << "\n";
        return false;
    }

    Task task(
        getNextId(),
        name,
        startTime,
        priority.empty() ? "medium" : priority,
        category.empty() ? "life" : category,
        finalRemindTime,
        false,
        "pending"
    );

    tasks.push_back(task);
    sortTasksByStartTime();

    if (!saveTasksUnlocked()) {
        return false;
    }

    std::cout << "Task added successfully.\n";
    std::cout << "Task file: " << taskFilePath << "\n";
    std::cout << "ID: " << task.id << "\n";
    std::cout << "Name: " << task.name << "\n";
    std::cout << "Start Time: " << task.startTime << "\n";
    std::cout << "Priority: " << task.priority << "\n";
    std::cout << "Category: " << task.category << "\n";
    std::cout << "Remind Time: " << task.remindTime << "\n";
    std::cout << "Status: " << task.status << "\n";

    return true;
}

bool TaskManager::deleteTaskById(int id) {
    std::lock_guard<std::mutex> lock(taskMutex);

    Utils::ensureDataDirectory();

    auto oldSize = tasks.size();

    tasks.erase(
        std::remove_if(
            tasks.begin(),
            tasks.end(),
            [id](const Task& task) {
                return task.id == id;
            }
        ),
        tasks.end()
    );

    if (tasks.size() == oldSize) {
        std::cerr << "Error: task id not found: " << id << "\n";
        return false;
    }

    sortTasksByStartTime();

    if (!saveTasksUnlocked()) {
        return false;
    }

    std::cout << "Task deleted successfully. ID: " << id << "\n";
    std::cout << "Task file updated: " << taskFilePath << "\n";

    return true;
}

bool TaskManager::markTaskDone(int id) {
    std::lock_guard<std::mutex> lock(taskMutex);

    Utils::ensureDataDirectory();

    for (auto& task : tasks) {
        if (task.id == id) {
            task.status = "done";

            if (!saveTasksUnlocked()) {
                return false;
            }

            std::cout << "Task marked as done. ID: " << id << "\n";
            std::cout << "Task file updated: " << taskFilePath << "\n";

            return true;
        }
    }

    std::cerr << "Error: task id not found: " << id << "\n";
    return false;
}

bool TaskManager::editTask(int id, const std::string& field, const std::string& value) {
    std::lock_guard<std::mutex> lock(taskMutex);

    Utils::ensureDataDirectory();

    for (auto& task : tasks) {
        if (task.id != id) {
            continue;
        }

        if (field == "name") {
            if (value.empty()) {
                std::cerr << "Error: task name cannot be empty.\n";
                return false;
            }

            if (!isNameAndStartTimeUniqueExceptId(value, task.startTime, id)) {
                std::cerr << "Error: 任务名称——开始时间必须具有单一性.\n";
                return false;
            }

            task.name = value;
        } else if (field == "startTime") {
            if (!Utils::isValidDateTimeFormat(value)) {
                std::cerr << "Error: invalid start time format. Expected YYYY-MM-DD_HH:MM\n";
                return false;
            }

            if (!isNameAndStartTimeUniqueExceptId(task.name, value, id)) {
                std::cerr << "Error: 任务名称——开始时间必须具有单一性.\n";
                return false;
            }

            if (!isStartTimeUniqueExceptId(value, id)) {
                std::cerr << "Error: another task already has the same start time: " << value << "\n";
                return false;
            }

            task.startTime = value;
        } else if (field == "priority") {
            task.priority = value.empty() ? "medium" : value;
        } else if (field == "category") {
            task.category = value.empty() ? "life" : value;
        } else if (field == "remindTime") {
            if (!Utils::isValidDateTimeFormat(value)) {
                std::cerr << "Error: invalid remind time format. Expected YYYY-MM-DD_HH:MM\n";
                return false;
            }

            task.remindTime = value;
            task.reminded = false;
        } else if (field == "status") {
            if (value != "pending" && value != "done") {
                std::cerr << "Error: status must be pending or done.\n";
                return false;
            }

            task.status = value;
        } else {
            std::cerr << "Error: unknown editable field: " << field << "\n";
            std::cerr << "Editable fields: name, startTime, priority, category, remindTime, status\n";
            return false;
        }

        sortTasksByStartTime();

        if (!saveTasksUnlocked()) {
            return false;
        }

        std::cout << "Task edited successfully. ID: " << id << "\n";
        std::cout << "Task file updated: " << taskFilePath << "\n";

        return true;
    }

    std::cerr << "Error: task id not found: " << id << "\n";
    return false;
}

void TaskManager::printTaskTable(const std::vector<Task>& taskList) const {
    if (taskList.empty()) {
        std::cout << "No tasks found.\n";
        return;
    }

    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(20) << "Name"
              << std::setw(20) << "Start Time"
              << std::setw(12) << "Priority"
              << std::setw(12) << "Category"
              << std::setw(20) << "Remind Time"
              << std::setw(10) << "Reminded"
              << std::setw(10) << "Status"
              << "\n";

    std::cout << std::string(109, '-') << "\n";

    for (const auto& task : taskList) {
        std::cout << std::left
                  << std::setw(5) << task.id
                  << std::setw(20) << task.name
                  << std::setw(20) << task.startTime
                  << std::setw(12) << task.priority
                  << std::setw(12) << task.category
                  << std::setw(20) << task.remindTime
                  << std::setw(10) << (task.reminded ? "yes" : "no")
                  << std::setw(10) << task.status
                  << "\n";
    }
}

void TaskManager::showTasks(const std::string& mode, const std::string& date) {
    std::lock_guard<std::mutex> lock(taskMutex);

    loadTasksUnlocked();

    std::vector<Task> filteredTasks;

    for (const auto& task : tasks) {
        bool matched = false;

        if (mode == "all") {
            matched = true;
        } else if (mode == "day") {
            matched = task.startTime.rfind(date, 0) == 0;
        } else if (mode == "month") {
            matched = task.startTime.rfind(date, 0) == 0;
        } else if (mode == "pending") {
            matched = task.status == "pending";
        } else if (mode == "done") {
            matched = task.status == "done";
        } else {
            std::cerr << "Error: unknown show mode: " << mode << "\n";
            return;
        }

        if (matched) {
            filteredTasks.push_back(task);
        }
    }

    std::sort(
        filteredTasks.begin(),
        filteredTasks.end(),
        [](const Task& a, const Task& b) {
            if (a.startTime != b.startTime) {
                return a.startTime < b.startTime;
            }

            return a.id < b.id;
        }
    );

    std::cout << "Loaded tasks from file: " << taskFilePath << "\n";
    printTaskTable(filteredTasks);
}

void TaskManager::searchTasks(const std::string& keyword) {
    std::lock_guard<std::mutex> lock(taskMutex);

    loadTasksUnlocked();

    std::string lowerKeyword = toLowerCopy(keyword);
    std::vector<Task> result;

    for (const auto& task : tasks) {
        std::string combined =
            task.name + " " +
            task.startTime + " " +
            task.priority + " " +
            task.category + " " +
            task.remindTime + " " +
            task.status;

        if (toLowerCopy(combined).find(lowerKeyword) != std::string::npos) {
            result.push_back(task);
        }
    }

    std::sort(
        result.begin(),
        result.end(),
        [](const Task& a, const Task& b) {
            if (a.startTime != b.startTime) {
                return a.startTime < b.startTime;
            }

            return a.id < b.id;
        }
    );

    std::cout << "Loaded tasks from file: " << taskFilePath << "\n";
    printTaskTable(result);
}

void TaskManager::checkAndRemindTasks() {
    std::lock_guard<std::mutex> lock(taskMutex);

    Utils::ensureDataDirectory();

    bool changed = false;

    for (auto& task : tasks) {
        if (
            task.status != "done" &&
            !task.reminded &&
            Utils::isDateTimeReached(task.remindTime)
        ) {
            std::cout << "\n";
            std::cout << "==================== REMINDER ====================\n";
            std::cout << "Task: " << task.name << "\n";
            std::cout << "Start Time: " << task.startTime << "\n";
            std::cout << "Priority: " << task.priority << "\n";
            std::cout << "Category: " << task.category << "\n";
            std::cout << "Remind Time: " << task.remindTime << "\n";
            std::cout << "Status: " << task.status << "\n";
            std::cout << "==================================================\n";

            playReminderSound();

            std::cout << "myschedule> " << std::flush;

            task.reminded = true;
            changed = true;
        }
    }

    if (changed) {
        saveTasksUnlocked();
    }
}

const std::vector<Task>& TaskManager::getTasks() const {
    return tasks;
}
// fix: handle invalid task id
