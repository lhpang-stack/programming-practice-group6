#include "SoundReminder.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

namespace {

std::string findReminderSoundFile() {
    const std::string candidates[] = {
        "assets/reminder.wav",
        "../assets/reminder.wav",
        "../../assets/reminder.wav",
        "./reminder.wav"
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    return "";
}

#ifdef _WIN32
void playFallbackBeep() {
    MessageBeep(MB_ICONEXCLAMATION);
}
#else
void playFallbackBeep() {
    std::cout << '\a' << std::flush;
}
#endif

} // namespace

void playReminderSound() {
    std::string soundFile = findReminderSoundFile();

#ifdef _WIN32
    if (!soundFile.empty()) {
        BOOL ok = PlaySoundA(
            soundFile.c_str(),
            NULL,
            SND_FILENAME | SND_ASYNC
        );

        if (ok) {
            return;
        }
    }

    playFallbackBeep();

#elif defined(__APPLE__)
    if (!soundFile.empty()) {
        std::string command = "afplay \"" + soundFile + "\" >/dev/null 2>&1 &";
        int result = std::system(command.c_str());

        if (result == 0) {
            return;
        }
    }

    playFallbackBeep();

#else
    if (!soundFile.empty()) {
        std::string command =
            "(paplay \"" + soundFile + "\" >/dev/null 2>&1 || "
            "aplay \"" + soundFile + "\" >/dev/null 2>&1) &";

        int result = std::system(command.c_str());

        if (result == 0) {
            return;
        }
    }

    playFallbackBeep();
#endif
}