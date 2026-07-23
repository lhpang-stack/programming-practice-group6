#ifndef VOICE_TASK_PARSER_H
#define VOICE_TASK_PARSER_H

#include <string>

struct VoiceTaskInfo {
    std::string name;
    std::string startTime;
    std::string priority;
    std::string category;
    std::string remindTime;
};

class VoiceTaskParser {
public:
    static bool parse(const std::string& text, VoiceTaskInfo& taskInfo);
};

#endif
