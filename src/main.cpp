#include "TaskManager.h"
#include "UserManager.h"

#include <iostream>
#include <string>
#include <vector>

void printHelp() {
    std::cout << "myschedule - A command line schedule manager\n\n";

    std::cout << "Usage:\n";
    std::cout << "  myschedule help\n";
    std::cout << "  myschedule register <username> <password>\n";
    std::cout << "  myschedule login <username> <password>\n";
    std::cout << "  myschedule <username> <password> addtask <name> <startTime> [priority] [category] [remindTime]\n";
    std::cout << "  myschedule <username> <password> showtask [all|day|month|pending|done] [date]\n";
    std::cout << "  myschedule <username> <password> deltask <id>\n";
    std::cout << "  myschedule <username> <password> donetask <id>\n";
    std::cout << "  myschedule <username> <password> edittask <id> <field> <value>\n";
    std::cout << "  myschedule <username> <password> searchtask <keyword>\n\n";
}

int handleUserCommand(TaskManager& taskManager, const std::vector<std::string>& args, int offset) {
    if (offset < 0 || offset >= static_cast<int>(args.size())) {
        std::cerr << "Error: missing command.\n";
        return 1;
    }

    const std::string userCommand = args[offset];

    if (userCommand == "addtask") {
        if (static_cast<int>(args.size()) < offset + 3 || static_cast<int>(args.size()) > offset + 6) {
            std::cerr << "Usage: addtask <name> <startTime> [priority] [category] [remindTime]\n";
            return 1;
        }

        std::string name = args[offset + 1];
        std::string startTime = args[offset + 2];
        std::string priority = (static_cast<int>(args.size()) >= offset + 4) ? args[offset + 3] : "medium";
        std::string category = (static_cast<int>(args.size()) >= offset + 5) ? args[offset + 4] : "life";
        std::string remindTime = (static_cast<int>(args.size()) >= offset + 6) ? args[offset + 5] : "";

        return taskManager.addTask(name, startTime, priority, category, remindTime) ? 0 : 1;
    }

    if (userCommand == "showtask") {
        if (static_cast<int>(args.size()) == offset + 1) {
            taskManager.showTasks("all", "");
            return 0;
        }

        if (static_cast<int>(args.size()) == offset + 2) {
            taskManager.showTasks(args[offset + 1], "");
            return 0;
        }

        if (static_cast<int>(args.size()) == offset + 3) {
            taskManager.showTasks(args[offset + 1], args[offset + 2]);
            return 0;
        }

        std::cerr << "Usage: showtask [all|day|month|pending|done] [date]\n";
        return 1;
    }

    if (userCommand == "deltask") {
        if (static_cast<int>(args.size()) != offset + 2) {
            std::cerr << "Usage: deltask <id>\n";
            return 1;
        }

        try {
            int id = std::stoi(args[offset + 1]);
            return taskManager.deleteTaskById(id) ? 0 : 1;
        } catch (...) {
            std::cerr << "Error: task id must be an integer.\n";
            return 1;
        }
    }

    if (userCommand == "donetask") {
        if (static_cast<int>(args.size()) != offset + 2) {
            std::cerr << "Usage: donetask <id>\n";
            return 1;
        }

        try {
            int id = std::stoi(args[offset + 1]);
            return taskManager.markTaskDone(id) ? 0 : 1;
        } catch (...) {
            std::cerr << "Error: task id must be an integer.\n";
            return 1;
        }
    }

    if (userCommand == "edittask") {
        if (static_cast<int>(args.size()) != offset + 4) {
            std::cerr << "Usage: edittask <id> <field> <value>\n";
            return 1;
        }

        try {
            int id = std::stoi(args[offset + 1]);
            std::string field = args[offset + 2];
            std::string value = args[offset + 3];

            return taskManager.editTask(id, field, value) ? 0 : 1;
        } catch (...) {
            std::cerr << "Error: task id must be an integer.\n";
            return 1;
        }
    }

    if (userCommand == "searchtask") {
        if (static_cast<int>(args.size()) != offset + 2) {
            std::cerr << "Usage: searchtask <keyword>\n";
            return 1;
        }

        taskManager.searchTasks(args[offset + 1]);
        return 0;
    }

    if (userCommand == "help") {
        printHelp();
        return 0;
    }

    std::cerr << "Unknown command: " << userCommand << "\n";
    return 1;
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

    if (argc >= 4) {
        const std::string username = args[1];
        const std::string password = args[2];

        if (!userManager.loginUser(username, password)) {
            return 1;
        }

        TaskManager taskManager(username);
        return handleUserCommand(taskManager, args, 3);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    return 1;
}
