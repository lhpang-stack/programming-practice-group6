#include <iostream>
#include <string>
#include <vector>

void printHelp() {
    std::cout << "myschedule - A command line schedule manager\n\n";

    std::cout << "Usage:\n";
    std::cout << "  myschedule help\n";
    std::cout << "  (More commands will be implemented soon)\n\n";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    if (argc == 1) {
        printHelp();
        return 0;
    }

    std::string command = args[1];

    if (command == "help" || command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    return 1;
}#include "UserManager.h"

#include <iostream>
#include <string>
#include <vector>

void printHelp() {
    std::cout << "myschedule - A command line schedule manager\n\n";

    std::cout << "Usage:\n";
    std::cout << "  myschedule help\n";
    std::cout << "  myschedule register <username> <password>\n";
    std::cout << "  myschedule login <username> <password>\n\n";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);

    if (argc == 1) {
        printHelp();
        return 0;
    }

    std::string command = args[1];
    UserManager userManager;

    if (command == "help" || command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    if (command == "register") {
        if (argc != 4) {
            std::cerr << "Usage: myschedule register <username> <password>\n";
            return 1;
        }

        return userManager.registerUser(args[2], args[3]) ? 0 : 1;
    }

    if (command == "login") {
        if (argc != 4) {
            std::cerr << "Usage: myschedule login <username> <password>\n";
            return 1;
        }

        return userManager.loginUser(args[2], args[3]) ? 0 : 1;
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    return 1;
}
