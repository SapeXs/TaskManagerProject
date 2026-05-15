#include "commands/command_type.h"
#include <string_view>

std::string_view ToString(CommandType type) {
    switch (type) {
        case CommandType::kAdd:
            return "add";
        case CommandType::kList:
            return "list";
        case CommandType::kFind:
            return "find";
        case CommandType::kRemove:
            return "remove";
        case CommandType::kFilter:  
            return "filter";
        case CommandType::kHelp:
            return "help";
        case CommandType::kExit:
            return "exit";
        case CommandType::kSave:
            return "save";
        default:
            return "invalid";
    }
}
