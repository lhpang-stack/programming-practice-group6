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

namespace {

bool isLeapYear(int year) {
    if (year % 400 == 0) {
        return true;
    }

    if (year % 100 == 0) {
        return false;
    }

    return year % 4 == 0;
}

int daysInMonth(int year, int month) {
    static const int commonYearDays[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month < 1 || month > 12) {
        return 0;
    }

    if (month == 2 && isLeapYear(year)) {
        return 29;
    }

    return commonYearDays[month - 1];
}

bool parseDateTime(
    const std::string& dateTime,
    int& year,
    int& month,
    int& day,
    int& hour,
    int& minute
) {
    static const std::regex pattern(
        R"(^(\d{4})-(\d{2})-(\d{2})_(\d{2}):(\d{2})$)"
    );

    std::smatch match;

    if (!std::regex_match(dateTime, match, pattern)) {
        return false;
    }

    try {
        year = std::stoi(match[1].str());
        month = std::stoi(match[2].str());
        day = std::stoi(match[3].str());
        hour = std::stoi(match[4].str());
        minute = std::stoi(match[5].str());
    } catch (...) {
        return false;
    }

    return true;
}

} // namespace

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

std::string getCurrentTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm localTime {};

#if defined(_WIN32)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H:%M");
    return oss.str();
}

bool isValidDateTimeFormat(const std::string& dateTime) {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;

    if (!parseDateTime(dateTime, year, month, day, hour, minute)) {
        return false;
    }

    if (year < 1900 || year > 3000) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    const int maxDay = daysInMonth(year, month);

    if (day < 1 || day > maxDay) {
        return false;
    }

    if (hour < 0 || hour > 23) {
        return false;
    }

    if (minute < 0 || minute > 59) {
        return false;
    }

    return true;
}

bool isDateTimeReached(const std::string& dateTime) {
    if (!isValidDateTimeFormat(dateTime)) {
        return false;
    }

    return dateTime <= getCurrentTimeString();
}

} // namespace Utils
