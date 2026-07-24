#include "Utils.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace Utils {

std::string trim(const std::string& text) {
    auto start = std::find_if_not(text.begin(), text.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });

    auto end = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    if (start >= end) {
        return "";
    }

    return std::string(start, end);
}

bool isValidUsername(const std::string& username) {
    std::string cleaned = trim(username);

    if (cleaned.length() < 3 || cleaned.length() > 20) {
        return false;
    }

    std::regex pattern("^[A-Za-z0-9_]+$");
    return std::regex_match(cleaned, pattern);
}

bool isValidPassword(const std::string& password) {
    if (password.length() < 6 || password.length() > 30) {
        return false;
    }

    bool hasNonSpace = false;

    for (unsigned char ch : password) {
        if (!std::isspace(ch)) {
            hasNonSpace = true;
            break;
        }
    }

    return hasNonSpace;
}

bool isValidDate(const std::string& date) {
    std::regex pattern("^\\d{4}-\\d{2}-\\d{2}$");

    if (!std::regex_match(date, pattern)) {
        return false;
    }

    int year = 0;
    int month = 0;
    int day = 0;
    char dash1 = '\0';
    char dash2 = '\0';

    std::stringstream ss(date);
    ss >> year >> dash1 >> month >> dash2 >> day;

    if (dash1 != '-' || dash2 != '-') {
        return false;
    }

    if (year < 1900 || year > 2100) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);

    if (leapYear && month == 2) {
        daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

bool isValidTime(const std::string& time) {
    std::regex pattern("^\\d{2}:\\d{2}$");

    if (!std::regex_match(time, pattern)) {
        return false;
    }

    int hour = 0;
    int minute = 0;
    char colon = '\0';

    std::stringstream ss(time);
    ss >> hour >> colon >> minute;

    if (colon != ':') {
        return false;
    }

    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

bool isPositiveInteger(const std::string& text) {
    std::string cleaned = trim(text);

    if (cleaned.empty()) {
        return false;
    }

    for (unsigned char ch : cleaned) {
        if (!std::isdigit(ch)) {
            return false;
        }
    }

    return std::stoi(cleaned) > 0;
}

}
