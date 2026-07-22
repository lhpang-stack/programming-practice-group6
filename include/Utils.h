#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string hashPassword(const std::string& password);

    bool ensureDataDirectory();
    bool fileExists(const std::string& path);
}

#endif
