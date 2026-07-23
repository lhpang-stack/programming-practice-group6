#include "UserManager.h"

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
    if (username.empty() || password.empty()) {
        return false;
    }

    loadUsers();

    if (userExists(username)) {
        return false;
    }

    users[username] = password;
    return saveUsers();
}

bool UserManager::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        currentUser.clear();
        return false;
    }

    loadUsers();

    auto it = users.find(username);

    if (it == users.end()) {
        currentUser.clear();
        return false;
    }

    if (it->second != password) {
        currentUser.clear();
        return false;
    }

    currentUser = username;
    return true;
}

void UserManager::logout() {
    currentUser.clear();
}

bool UserManager::userExists(const std::string& username) const {
    return users.find(username) != users.end();
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
            if (!username.empty() && !password.empty()) {
                users[username] = password;
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
