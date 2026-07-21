#include "Task.h"

#include <sstream>

Task::Task()
    : id(0), title(""), date(""), time(""), completed(false) {}

Task::Task(int id, const std::string& title, const std::string& date, const std::string& time)
    : id(id), title(title), date(date), time(time), completed(false) {}

int Task::getId() const {
    return id;
}

std::string Task::getTitle() const {
    return title;
}

std::string Task::getDate() const {
    return date;
}

std::string Task::getTime() const {
    return time;
}

bool Task::isCompleted() const {
    return completed;
}

void Task::setTitle(const std::string& title) {
    this->title = title;
}

void Task::setDate(const std::string& date) {
    this->date = date;
}

void Task::setTime(const std::string& time) {
    this->time = time;
}

void Task::setCompleted(bool completed) {
    this->completed = completed;
}

std::string Task::toString() const {
    std::ostringstream oss;
    oss << "[" << id << "] "
        << title
        << " | " << date
        << " " << time
        << " | "
        << (completed ? "Done" : "Pending");

    return oss.str();
}
