#include "format/task_formatter.h"

#include <string>

#include "core/task_converters.h"

namespace {

std::string TagsToString(const TaskBase::TagContainer& tags) {
  if (tags.empty()) {
    return "-";
  }

  std::string result;
  bool first = true;

  for (const std::string& tag : tags) {
    if (!first) {
      result += ", ";
    }

    first = false;
    result += tag;
  }

  return result;
}

}  // namespace

std::string TaskFormatter::FormatTask(const TaskBase& task) {
  std::string result;

  result += std::to_string(task.GetId());
  result += " | ";
  result += std::string(task.GetTypeName());
  result += " | ";
  result += task.GetTitle();
  result += " | priority: ";
  result += TaskPriorityToString(task.GetPriority());
  result += " | state: ";
  result += TaskStateToString(task.GetState());
  result += " | progress: ";
  result += task.GetProgressString();
  result += " | tags: ";
  result += TagsToString(task.GetTags());

  return result;
}

std::string TaskFormatter::FormatTaskList(std::span<const TaskBase* const> tasks) {
  if (tasks.empty()) {
    return "No tasks";
  }

  std::string result;

  for (const TaskBase* task : tasks) {
    if (task == nullptr) {
      continue;
    }

    result += FormatTask(*task);
    result += '\n';
  }

  if (!result.empty() && result.back() == '\n') {
    result.pop_back();
  }

  return result.empty() ? "No tasks" : result;
}

std::string TaskFormatter::FormatHelp() {
  return "Available commands:\n"
         "  add reminder <title> [--seconds n] [--priority value] [--tag value]\n"
         "  add recurring <title> --seconds n --interval n [--priority value] [--tag value]\n"
         "  add bounded <title> --seconds n --interval n --repeats n [--priority value] [--tag "
         "value]\n"
         "  add savings <title> --current n --target n [--priority value] [--tag value]\n"
         "  add stepped <title> --step text [--step text ...] [--current-step n]\n"
         "  add final <title> --step text [--step text ...] --final text [--current-step n]\n"
         "  list             - show all tasks\n"
         "  find <id>        - find task by id\n"
         "  filter priority <value>        - filter by priority: low, medium, high, critical\n"
         "  filter state <value>           - filter by state: not_started, in_progress, done, "
         "overdue, scheduled\n"
         "  filter tag <value>             - filter by tag\n"
         "  remove <id>      - remove task by id\n"
         "  clear                                       - remove all tasks\n"
         "  save             - save tasks to storage\n"
         "  help             - show this help";
}
