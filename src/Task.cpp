#include "Task.h"

#include <sstream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> splitByPipe(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, '|')) {
        parts.push_back(item);
    }

    return parts;
}

std::string sanitizeField(std::string value) {
    for (char& c : value) {
        if (c == '|') {
            c = '/';
        }

        if (c == '\n' || c == '\r') {
            c = ' ';
        }
    }

    return value;
}

std::string normalizePriority(const std::string& priority) {
    if (priority == "high" || priority == "medium" || priority == "low") {
        return priority;
    }

    if (priority.empty()) {
        return "medium";
    }

    return priority;
}

std::string normalizeCategory(const std::string& category) {
    if (category.empty()) {
        return "life";
    }

    return category;
}

std::string normalizeStatus(const std::string& status) {
    if (status == "pending" || status == "done") {
        return status;
    }

    return "pending";
}

bool parseBool01(const std::string& value) {
    return value == "1" || value == "true" || value == "yes";
}

} // namespace

Task::Task()
    : id(0),
      name(""),
      startTime(""),
      priority("medium"),
      category("life"),
      remindTime(""),
      reminded(false),
      status("pending") {}

Task::Task(
    int id,
    const std::string& name,
    const std::string& startTime,
    const std::string& priority,
    const std::string& category,
    const std::string& remindTime,
    bool reminded,
    const std::string& status
)
    : id(id),
      name(name),
      startTime(startTime),
      priority(normalizePriority(priority)),
      category(normalizeCategory(category)),
      remindTime(remindTime),
      reminded(reminded),
      status(normalizeStatus(status)) {}

std::string Task::serialize() const {
    std::ostringstream oss;

    oss << id << "|"
        << sanitizeField(name) << "|"
        << sanitizeField(startTime) << "|"
        << sanitizeField(priority) << "|"
        << sanitizeField(category) << "|"
        << sanitizeField(remindTime) << "|"
        << (reminded ? 1 : 0) << "|"
        << sanitizeField(status);

    return oss.str();
}

bool Task::deserialize(const std::string& line, Task& task) {
    std::vector<std::string> parts = splitByPipe(line);

    if (parts.size() != 7 && parts.size() != 8) {
        return false;
    }

    try {
        Task parsed;

        parsed.id = std::stoi(parts[0]);
        parsed.name = parts[1];
        parsed.startTime = parts[2];
        parsed.priority = normalizePriority(parts[3]);
        parsed.category = normalizeCategory(parts[4]);
        parsed.remindTime = parts[5].empty() ? parts[2] : parts[5];
        parsed.reminded = parseBool01(parts[6]);
        parsed.status = (parts.size() == 8) ? normalizeStatus(parts[7]) : "pending";

        if (parsed.id <= 0) {
            return false;
        }

        if (parsed.name.empty() || parsed.startTime.empty()) {
            return false;
        }

        task = parsed;
    } catch (...) {
        return false;
    }

    return true;
}
