#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string hashPassword(const std::string& password);

    bool ensureDataDirectory();
    bool fileExists(const std::string& path);

    std::string getCurrentTimeString();
    bool isDateTimeReached(const std::string& dateTime);
    bool isValidDateTimeFormat(const std::string& dateTime);
}

#endif
