#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string trim(const std::string& text);

    bool isValidUsername(const std::string& username);
    bool isValidPassword(const std::string& password);

    bool isValidDate(const std::string& date);
    bool isValidTime(const std::string& time);

    bool isPositiveInteger(const std::string& text);
}

#endif
