#include "Utils.h"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace Utils {

std::string hashPassword(const std::string& password) {
    const std::string salt = "myschedule_salt_2026";
    std::hash<std::string> hasher;
    size_t value = hasher(password + salt);

    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool ensureDataDirectory() {
    const std::filesystem::path dataDir("data");

    try {
        if (std::filesystem::exists(dataDir)) {
            return std::filesystem::is_directory(dataDir);
        }

        return std::filesystem::create_directories(dataDir);
    } catch (const std::exception& e) {
        std::cerr << "Error: failed to ensure data directory: " << e.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "Error: failed to ensure data directory.\n";
        return false;
    }
}

bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

} // namespace Utils
