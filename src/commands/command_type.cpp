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
    case CommandType::kClear:
      return "clear";
    case CommandType::kSetTitle:
      return "set-title";
    case CommandType::kSetDescription:
      return "set-description";
    case CommandType::kSetPriority:
      return "set-priority";
    case CommandType::kAddTag:
      return "add-tag";
    case CommandType::kRemoveTag:
      return "remove-tag";
    case CommandType::kSetTime:
      return "set-time";
    case CommandType::kSetInterval:
      return "set-interval";
    case CommandType::kSetRepeats:
      return "set-repeats";
    case CommandType::kAddValue:
      return "add-value";
    case CommandType::kAdvance:
      return "advance";
    case CommandType::kOverdue:
      return "overdue";
    case CommandType::kReset:
      return "reset";
    default:
      return "invalid";
  }
}
