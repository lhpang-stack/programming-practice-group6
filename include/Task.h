#ifndef TASK_H
#define TASK_H

#include <string>

class Task {
public:
    int id;
    std::string name;
    std::string startTime;
    std::string priority;
    std::string category;
    std::string remindTime;
    bool reminded;
    std::string status;

    Task();

    Task(
        int id,
        const std::string& name,
        const std::string& startTime,
        const std::string& priority,
        const std::string& category,
        const std::string& remindTime,
        bool reminded,
        const std::string& status = "pending"
    );

    std::string serialize() const;
    static bool deserialize(const std::string& line, Task& task);
};

#endif
