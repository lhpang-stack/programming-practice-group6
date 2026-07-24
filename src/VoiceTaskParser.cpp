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

} // namespace

