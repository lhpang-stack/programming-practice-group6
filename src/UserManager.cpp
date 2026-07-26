#include "UserManager.h"
#include "Utils.h"

#include <fstream>
#include <sstream>

UserManager::UserManager()
    : currentUser(""), userFilePath("data/users.txt") {
    loadUsers();
}

UserManager::UserManager(const std::string& userFilePath)
    : currentUser(""), userFilePath(userFilePath) {
    loadUsers();
}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    std::string cleanedUsername = Utils::trim(username);

    if (!Utils::isValidUsername(cleanedUsername) || !Utils::isValidPassword(password)) {
        return false;
    }

    loadUsers();

    if (userExists(cleanedUsername)) {
        return false;
    }

    users[cleanedUsername] = password;
    return saveUsers();
}

bool UserManager::login(const std::string& username, const std::string& password) {
    std::string cleanedUsername = Utils::trim(username);

    if (!Utils::isValidUsername(cleanedUsername) || password.empty()) {
        currentUser.clear();
        return false;
    }

    loadUsers();

    auto it = users.find(cleanedUsername);

    if (it == users.end()) {
        currentUser.clear();
        return false;
    }

    if (it->second != password) {
        currentUser.clear();
        return false;
    }

    currentUser = cleanedUsername;
    return true;
}

void UserManager::logout() {
    currentUser.clear();
}

bool UserManager::userExists(const std::string& username) const {
    std::string cleanedUsername = Utils::trim(username);

    if (cleanedUsername.empty()) {
        return false;
    }

    return users.find(cleanedUsername) != users.end();
}

bool UserManager::isLoggedIn() const {
    return !currentUser.empty();
}

std::string UserManager::getCurrentUser() const {
    return currentUser;
}

bool UserManager::loadUsers() {
    users.clear();

    std::ifstream file(userFilePath);

    if (!file.is_open()) {
        return true;
    }

    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string username;
        std::string password;

        if (std::getline(ss, username, ',') && std::getline(ss, password)) {
            std::string cleanedUsername = Utils::trim(username);

            if (Utils::isValidUsername(cleanedUsername) && !password.empty()) {
                users[cleanedUsername] = password;
            }
        }
    }

    file.close();
    return true;
}

bool UserManager::saveUsers() const {
    std::ofstream file(userFilePath);

    if (!file.is_open()) {
        return false;
    }

    for (const auto& pair : users) {
        file << pair.first << "," << pair.second << "\n";
    }

    file.close();
    return true;
}
