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
  return R"(TaskManager CLI

Usage:
  taskctl <command> [arguments]

Task creation:
  add reminder <title> [--time duration] [--priority value] [--tag value]
      Create a simple reminder task.
      Example:
        taskctl add reminder "Read book" --time 30m --priority high --tag study

  add recurring <title> --time duration --interval duration [--priority value] [--tag value]
      Create a task that repeats after every interval.
      Example:
        taskctl add recurring "Water plants" --time 1h --interval 1d --tag home

  add bounded <title> --time duration --interval duration --repeats n [--priority value] [--tag value]
      Create a recurring task with limited number of repeats.
      Example:
        taskctl add bounded "Gym" --time 2h --interval 1w --repeats 10 --tag health

  add savings <title> --current n --target n [--priority value] [--tag value]
      Create a savings/progress task.
      Example:
        taskctl add savings "Buy laptop" --current 10000 --target 70000 --priority critical

  add stepped <title> --step text [--step text ...] [--current-step n]
      Create a task with several text steps.
      Example:
        taskctl add stepped "Course project" --step "write code" --step "prepare defense"

  add final <title> --step text [--step text ...] --final text [--current-step n]
      Create a stepped task with a final deadline note.
      Example:
        taskctl add final "Course work" --step "write" --step "check" --final "deadline soon"

Task viewing:
  list
      Show all tasks.

  find <id>
      Show one task by id.
      Example:
        taskctl find 3

Filtering:
  filter priority <low|medium|high|critical>
      Show tasks with selected priority.
      Example:
        taskctl filter priority high

  filter state <not_started|in_progress|done|overdue|scheduled>
      Show tasks with selected state.
      Example:
        taskctl filter state scheduled

  filter tag <value>
      Show tasks with selected tag.
      Example:
        taskctl filter tag study

Managing:
  remove <id>
      Remove task by id.
      Example:
        taskctl remove 3

  clear
      Remove all tasks and save empty storage.

  save
      Save tasks manually.

  help
      Show this help message.

Hints:
  Duration examples:
    30s  = 30 seconds
    10m  = 10 minutes
    2h   = 2 hours
    1d   = 1 day
    1w   = 1 week
    3mo  = 3 months, approximately 30 days each
    1y   = 1 year, approximately 365 days
    1h30m or "1h 30m" are also supported

  Priority values:
    low, medium, high, critical

  Tags:
    You can pass several tags:
      taskctl add reminder "Read book" --tag study --tag university

  Titles and values with spaces:
    Use quotes:
      taskctl add reminder "Read big book" --tag "very important"

  Default behavior:
    If task type is omitted, reminder is used.
    If priority is omitted, medium is used.
)";
}
