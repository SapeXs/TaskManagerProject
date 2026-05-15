#include "format/task_formatter.h"

#include <string>

namespace {

std::string StateToString(TaskState state) {
  switch (state) {
    case TaskState::kNotStarted:
      return "not_started";
    case TaskState::kInProgress:
      return "in_progress";
    case TaskState::kDone:
      return "done";
    case TaskState::kOverdue:
      return "overdue";
    case TaskState::kScheduled:
      return "scheduled";
    default:
      return "unknown";
  }
}

std::string PriorityToString(TaskPriority priority) {
  switch (priority) {
    case TaskPriority::kLowPriority:
      return "low";
    case TaskPriority::kMediumPriority:
      return "medium";
    case TaskPriority::kHighPriority:
      return "high";
    case TaskPriority::kCriticalPriority:
      return "critical";
    default:
      return "unknown";
  }
}

}

std::string TaskFormatter::FormatTask(const TaskBase& task) {
  std::string result;

  result += std::to_string(task.GetId());
  result += " | ";
  result += std::string(task.GetTypeName());
  result += " | ";
  result += task.GetTitle();
  result += " | priority: ";
  result += PriorityToString(task.GetPriority());
  result += " | state: ";
  result += StateToString(task.GetState());
  result += " | progress: ";
  result += task.GetProgressString();

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
         "  add <title>      - add reminder task\n"
         "  list             - show all tasks\n"
         "  find <id>        - find task by id\n"
         "  filter priority <value>        - filter by priority: low, medium, high, critical\n"
         "  filter state <value>           - filter by state: not_started, in_progress, done, overdue, scheduled\n"
         "  filter tag <value>             - filter by tag\n"
         "  remove <id>      - remove task by id\n"
         "  save             - save tasks to storage\n"
         "  help             - show this help";
}
