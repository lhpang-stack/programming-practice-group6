#include "VoiceTaskParser.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string toLowerCopy(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}

std::vector<std::string> splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

std::string onlyDigits(const std::string& value) {
    std::string result;

    for (char c : value) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            result += c;
        }
    }

    return result;
}

std::string sanitizeNamePart(const std::string& value) {
    std::string result;

    for (char c : value) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            result += c;
        } else if (c == '_' || c == '-') {
            result += c;
        }
    }

    return result;
}

bool isCommandKeyword(const std::string& value) {
    const std::string lower = toLowerCopy(value);

    return lower == "task" ||
           lower == "add" ||
           lower == "create" ||
           lower == "new" ||
           lower == "ask";
}

bool isAtKeyword(const std::string& value) {
    const std::string lower = toLowerCopy(value);

    return lower == "at" ||
           lower == "time" ||
           lower == "on";
}

bool isPriorityKeyword(const std::string& value) {
    return toLowerCopy(value) == "priority";
}

bool isCategoryKeyword(const std::string& value) {
    return toLowerCopy(value) == "category";
}

bool isRemindKeyword(const std::string& value) {
    const std::string lower = toLowerCopy(value);

    return lower == "remind" ||
           lower == "reminder" ||
           lower == "alert";
}

bool isStopKeyword(const std::string& value) {
    return isPriorityKeyword(value) ||
           isCategoryKeyword(value) ||
           isRemindKeyword(value);
}

std::string twoDigits(const std::string& value) {
    if (value.size() == 1) {
        return "0" + value;
    }

    return value;
}

bool isValidDateParts(
    const std::string& year,
    const std::string& month,
    const std::string& day,
    const std::string& hour,
    const std::string& minute
) {
    if (
        year.size() != 4 ||
        month.empty() ||
        day.empty() ||
        hour.empty() ||
        minute.empty()
    ) {
        return false;
    }

    try {
        int y = std::stoi(year);
        int mo = std::stoi(month);
        int d = std::stoi(day);
        int h = std::stoi(hour);
        int mi = std::stoi(minute);

        if (y < 1900 || y > 3000) {
            return false;
        }

        if (mo < 1 || mo > 12) {
            return false;
        }

        if (d < 1 || d > 31) {
            return false;
        }

        if (h < 0 || h > 23) {
            return false;
        }

        if (mi < 0 || mi > 59) {
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

std::string buildDateTime(
    const std::string& year,
    const std::string& month,
    const std::string& day,
    const std::string& hour,
    const std::string& minute
) {
    if (!isValidDateParts(year, month, day, hour, minute)) {
        return "";
    }

    return year + "-" +
           twoDigits(month) + "-" +
           twoDigits(day) + "_" +
           twoDigits(hour) + ":" +
           twoDigits(minute);
}

std::string formatDateTimeFromDigits(const std::string& digits) {
    if (digits.size() == 12) {
        return buildDateTime(
            digits.substr(0, 4),
            digits.substr(4, 2),
            digits.substr(6, 2),
            digits.substr(8, 2),
            digits.substr(10, 2)
        );
    }

    if (digits.size() == 11) {
        return buildDateTime(
            digits.substr(0, 4),
            digits.substr(4, 2),
            digits.substr(6, 2),
            digits.substr(8, 1),
            digits.substr(9, 2)
        );
    }

    return "";
}

bool parseDateTimeFromTokenWindow(
    const std::vector<std::string>& words,
    int begin,
    std::string& output,
    int& consumedUntil
) {
    consumedUntil = begin;
    output.clear();

    if (begin >= static_cast<int>(words.size())) {
        return false;
    }

    std::string digits;

    for (int i = begin; i < static_cast<int>(words.size()); ++i) {
        if (isStopKeyword(words[i])) {
            break;
        }

        const std::string partDigits = onlyDigits(words[i]);

        if (partDigits.empty()) {
            break;
        }

        digits += partDigits;
        consumedUntil = i + 1;

        if (digits.size() == 12 || digits.size() == 11) {
            output = formatDateTimeFromDigits(digits);

            if (!output.empty()) {
                return true;
            }
        }

        if (digits.size() > 12) {
            break;
        }
    }

    return false;
}

bool parseDateTimeAfterKeyword(
    const std::vector<std::string>& words,
    int keywordPosition,
    std::string& output,
    int& consumedUntil
) {
    return parseDateTimeFromTokenWindow(words, keywordPosition + 1, output, consumedUntil);
}

} // namespace
