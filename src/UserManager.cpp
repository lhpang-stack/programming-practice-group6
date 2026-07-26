#include "UserManager.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

UserManager::UserManager(const std::string& userFilePath)
    : userFilePath(userFilePath) {
    Utils::ensureDataDirectory();
}

bool UserManager::userExists(const std::string& username) const {
    std::ifstream file(userFilePath);

    if (!file.is_open()) {
        return false;
    }

    std::string storedUsername;
    std::string storedHash;

    while (file >> storedUsername >> storedHash) {
        if (storedUsername == username) {
            return true;
        }
    }

    return false;
}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cerr << "Error: username and password cannot be empty.\n";
        return false;
    }

    if (userExists(username)) {
        std::cerr << "Error: user already exists: " << username << "\n";
        return false;
    }

    std::ofstream file(userFilePath, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error: cannot open user file: " << userFilePath << "\n";
        return false;
    }

    file << username << " " << Utils::hashPassword(password) << "\n";

    std::cout << "User registered successfully: " << username << "\n";
    return true;
}

bool UserManager::loginUser(const std::string& username, const std::string& password) const {
    std::ifstream file(userFilePath);

    if (!file.is_open()) {
        std::cerr << "Error: user file not found. Please register first.\n";
        return false;
    }

    std::string storedUsername;
    std::string storedHash;
    const std::string inputHash = Utils::hashPassword(password);

    while (file >> storedUsername >> storedHash) {
        if (storedUsername == username && storedHash == inputHash) {
            std::cout << "Login successful. Welcome, " << username << "!\n";
            return true;
        }
    }

    std::cerr << "Error: invalid username or password.\n";
    return false;
}
