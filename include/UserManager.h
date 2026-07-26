#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>

class UserManager {
public:
    explicit UserManager(const std::string& userFilePath = "data/users.txt");

    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password) const;
    bool userExists(const std::string& username) const;

private:
    std::string userFilePath;
};

#endif
