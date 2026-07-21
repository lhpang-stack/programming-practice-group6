#ifndef TASK_H
#define TASK_H

#include <string>

class Task {
private:
    int id;
    std::string title;
    std::string date;
    std::string time;
    bool completed;

public:
    Task();
    Task(int id, const std::string& title, const std::string& date, const std::string& time);

    int getId() const;
    std::string getTitle() const;
    std::string getDate() const;
    std::string getTime() const;
    bool isCompleted() const;

    void setTitle(const std::string& title);
    void setDate(const std::string& date);
    void setTime(const std::string& time);
    void setCompleted(bool completed);

    std::string toString() const;
};

#endif
