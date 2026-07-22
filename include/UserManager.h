#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <map>
#include <string>

class UserManager {
private:
    std::map<std::string, std::string> users;
    std::string currentUser;
    std::string userFilePath;

public:
    UserManager();
    explicit UserManager(const std::string& userFilePath);

    bool registerUser(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);
    void logout();

    bool userExists(const std::string& username) const;
    bool isLoggedIn() const;
    std::string getCurrentUser() const;

    bool loadUsers();
    bool saveUsers() const;
};

#endif