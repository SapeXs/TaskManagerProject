#include "ui/command_type.h"
#include <string>

std::string ToString(CommandType type) {
    switch (type) {
        case CommandType::kAdd:      return "add";
        case CommandType::kList:     return "list";
        case CommandType::kFind:     return "find";
        case CommandType::kRemove:   return "remove";
        case CommandType::kFilter:   return "filter";
        case CommandType::kHelp:     return "help";
        case CommandType::kExit:     return "exit";
        default:                     return "invalid";
    }
}
