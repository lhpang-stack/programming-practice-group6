#include "Reminder.h"
#include "TaskManager.h"
#include "UserManager.h"
#include "VoiceTaskParser.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace {

std::string quoteCommandArgument(const std::string& arg) {
#ifdef _WIN32
    std::string result = "\"";

    for (char c : arg) {
        if (c == '"') {
            result += "\\\"";
        } else {
            result += c;
        }
    }

    result += "\"";
    return result;
#else
    std::string result = "'";

    for (char c : arg) {
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }

    result += "'";
    return result;
#endif
}

std::string trimRightNewlines(std::string text) {
    while (
        !text.empty() &&
        (
            text.back() == '\n' ||
            text.back() == '\r' ||
            text.back() == ' ' ||
            text.back() == '\t'
        )
    ) {
        text.pop_back();
    }

    return text;
}

} // namespace

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
    std::cout << "  myschedule <username> <password> searchtask <keyword>\n";
    std::cout << "  myschedule <username> <password> voiceadd\n";
    std::cout << "  myschedule <username> <password> voiceadd --manual\n";
    std::cout << "  myschedule <username> <password> voiceadd --file <wav_path>\n";
    std::cout << "  myschedule <username> <password> voiceadd <wav_path>\n";
    std::cout << "  myschedule <username> <password> voiceadd --mic\n";
    std::cout << "  myschedule <username> <password> run\n\n";

    std::cout << "Editable fields:\n";
    std::cout << "  name, startTime, priority, category, remindTime, status\n\n";

    std::cout << "Time format:\n";
    std::cout << "  YYYY-MM-DD_HH:MM\n\n";

    std::cout << "Voice text format:\n";
    std::cout << "  task <name> at <YYYY-MM-DD_HH:MM> priority <high|medium|low> category <category> remind <YYYY-MM-DD_HH:MM>\n";
    std::cout << "  task <name> at <YYYY-MM-DD HH:MM> priority <high|medium|low> category <category> remind <YYYY-MM-DD HH:MM>\n\n";

    std::cout << "Examples:\n";
    std::cout << "  myschedule register user1 123456\n";
    std::cout << "  myschedule login user1 123456\n";
    std::cout << "  myschedule user1 123456 addtask WriteReport 2026-07-15_20:30 high study 2026-07-15_20:20\n";
    std::cout << "  myschedule user1 123456 showtask all\n";
    std::cout << "  myschedule user1 123456 showtask pending\n";
    std::cout << "  myschedule user1 123456 showtask done\n";
    std::cout << "  myschedule user1 123456 donetask 1\n";
    std::cout << "  myschedule user1 123456 edittask 1 priority low\n";
    std::cout << "  myschedule user1 123456 searchtask report\n";
    std::cout << "  myschedule user1 123456 voiceadd\n";
    std::cout << "  myschedule user1 123456 voiceadd --manual\n";
    std::cout << "  myschedule user1 123456 voiceadd --file assets/voice_task.wav\n";
    std::cout << "  myschedule user1 123456 voiceadd assets/voice_task.wav\n";
    std::cout << "  myschedule user1 123456 voiceadd --mic\n";
    std::cout << "  myschedule user1 123456 run\n\n";

    std::cout << "Run mode commands:\n";
    std::cout << "  addtask <name> <startTime> [priority] [category] [remindTime]\n";
    std::cout << "  showtask [all|day|month|pending|done] [date]\n";
    std::cout << "  deltask <id>\n";
    std::cout << "  donetask <id>\n";
    std::cout << "  edittask <id> <field> <value>\n";
    std::cout << "  searchtask <keyword>\n";
    std::cout << "  voiceadd\n";
    std::cout << "  voiceadd --manual\n";
    std::cout << "  voiceadd --file <wav_path>\n";
    std::cout << "  voiceadd <wav_path>\n";
    std::cout << "  voiceadd --mic\n";
    std::cout << "  help\n";
    std::cout << "  exit\n\n";

    std::cout << "Notes:\n";
    std::cout << "  - Passwords are stored as hash values, not plain text.\n";
    std::cout << "  - Task data is saved in local files under the data directory.\n";
    std::cout << "  - In run mode, a background thread checks reminders periodically.\n";
    std::cout << "  - voiceadd calls scripts/voice_input.py to get recognized text.\n";
    std::cout << "  - voiceadd <wav_path> is equivalent to voiceadd --file <wav_path>.\n";
}

std::vector<std::string> splitCommandLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string runVoiceInputScript(const std::vector<std::string>& voiceArgs) {
#ifdef _WIN32
    std::string command = "python scripts/voice_input.py";
#else
    std::string command = "python3 scripts/voice_input.py";
#endif

    for (const auto& arg : voiceArgs) {
        command += " ";
        command += quoteCommandArgument(arg);
    }

    std::array<char, 256> buffer {};
    std::string result;

    FILE* pipe = POPEN(command.c_str(), "r");

    if (!pipe) {
        std::cerr << "Error: failed to start voice input script.\n";
        std::cerr << "Command: " << command << "\n";
        return "";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }

    int closeCode = PCLOSE(pipe);

    result = trimRightNewlines(result);

    if (closeCode != 0 && result.empty()) {
        std::cerr << "Error: voice input script exited with code: " << closeCode << "\n";
        return "";
    }

    return result;
}

std::vector<std::string> normalizeVoiceArgs(const std::vector<std::string>& rawArgs) {
    std::vector<std::string> voiceArgs = rawArgs;

    if (voiceArgs.empty()) {
        voiceArgs.push_back("--manual");
        return voiceArgs;
    }

    if (voiceArgs.size() == 1) {
        const std::string onlyArg = voiceArgs[0];

        if (onlyArg == "--manual" || onlyArg == "--mic") {
            return voiceArgs;
        }

        if (onlyArg == "--file") {
            return voiceArgs;
        }

        voiceArgs.insert(voiceArgs.begin(), "--file");
        return voiceArgs;
    }

    return voiceArgs;
}

bool validateVoiceArgs(const std::vector<std::string>& voiceArgs) {
    if (voiceArgs.empty()) {
        return false;
    }

    if (voiceArgs.size() == 1) {
        if (voiceArgs[0] == "--manual" || voiceArgs[0] == "--mic") {
            return true;
        }

        if (voiceArgs[0] == "--file") {
            std::cerr << "Error: --file requires a wav file path.\n";
            return false;
        }

        std::cerr << "Error: invalid voiceadd option: " << voiceArgs[0] << "\n";
        return false;
    }

    if (voiceArgs.size() == 2 && voiceArgs[0] == "--file") {
        return true;
    }

    std::cerr << "Usage:\n";
    std::cerr << "  voiceadd\n";
    std::cerr << "  voiceadd --manual\n";
    std::cerr << "  voiceadd --file <wav_path>\n";
    std::cerr << "  voiceadd <wav_path>\n";
    std::cerr << "  voiceadd --mic\n";
    return false;
}

int handleVoiceAdd(TaskManager& taskManager, const std::vector<std::string>& rawVoiceArgs) {
    std::vector<std::string> voiceArgs = normalizeVoiceArgs(rawVoiceArgs);

    if (!validateVoiceArgs(voiceArgs)) {
        return 1;
    }

    std::cout << "Starting voice task input...\n";
    std::cout << "Expected recognized text format:\n";
    std::cout << "  task <name> at <YYYY-MM-DD_HH:MM> priority <high|medium|low> category <category> remind <YYYY-MM-DD_HH:MM>\n\n";

    const std::string recognizedText = runVoiceInputScript(voiceArgs);

    if (recognizedText.empty()) {
        std::cerr << "Error: failed to get recognized voice text.\n";
        std::cerr << "Please check scripts/voice_input.py, Python environment, audio file path, or microphone device.\n";
        return 1;
    }

    std::cout << "Voice recognized text: " << recognizedText << "\n";

    VoiceTaskInfo taskInfo;

    if (!VoiceTaskParser::parse(recognizedText, taskInfo)) {
        std::cerr << "Error: failed to parse recognized text.\n";
        std::cerr << "Please use format:\n";
        std::cerr << "  task StudyCpp at 2026-07-14_21:00 priority high category study remind 2026-07-14_20:50\n";
        return 1;
    }

    std::cout << "Parsed task:\n";
    std::cout << "  name: " << taskInfo.name << "\n";
    std::cout << "  startTime: " << taskInfo.startTime << "\n";
    std::cout << "  priority: " << taskInfo.priority << "\n";
    std::cout << "  category: " << taskInfo.category << "\n";
    std::cout << "  remindTime: " << taskInfo.remindTime << "\n";

    if (!taskManager.addTask(
            taskInfo.name,
            taskInfo.startTime,
            taskInfo.priority,
            taskInfo.category,
            taskInfo.remindTime
        )) {
        std::cerr << "Error: failed to add task from voice input.\n";
        return 1;
    }

    std::cout << "Task added successfully from voice input.\n";
    return 0;
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

    if (userCommand == "voiceadd") {
        std::vector<std::string> rawVoiceArgs;

        for (int i = offset + 1; i < static_cast<int>(args.size()); ++i) {
            rawVoiceArgs.push_back(args[i]);
        }

        return handleVoiceAdd(taskManager, rawVoiceArgs);
    }

    if (userCommand == "help") {
        printHelp();
        return 0;
    }

    std::cerr << "Unknown command: " << userCommand << "\n";
    return 1;
}

void runShell(TaskManager& taskManager) {
    Reminder reminder(taskManager);
    reminder.start();

    std::cout << "Entering myschedule run mode.\n";
    std::cout << "Type 'help' for commands, 'exit' to quit.\n";

    std::string line;

    while (true) {
        std::cout << "myschedule> " << std::flush;

        if (!std::getline(std::cin, line)) {
            break;
        }

        std::vector<std::string> tokens = splitCommandLine(line);

        if (tokens.empty()) {
            continue;
        }

        if (tokens[0] == "exit" || tokens[0] == "quit") {
            break;
        }

        handleUserCommand(taskManager, tokens, 0);
    }

    reminder.stop();
    std::cout << "Exit run mode.\n";
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
        const std::string userCommand = args[3];

        if (!userManager.loginUser(username, password)) {
            return 1;
        }

        TaskManager taskManager(username);

        if (userCommand == "run") {
            if (argc != 4) {
                std::cerr << "Usage: myschedule <username> <password> run\n";
                return 1;
            }

            runShell(taskManager);
            return 0;
        }

        return handleUserCommand(taskManager, args, 3);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printHelp();
    return 1;
}
