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
  add reminder <title> [--at datetime | --date date | --time duration] [--priority value] [--tag value]
      Create a simple reminder task.
      Examples:
        taskctl add reminder "Read book" --time 30m --priority high --tag study
        taskctl add reminder "Read book" --at "2026-05-20 18:30"
        taskctl add reminder "Submit report" --date "2026-05-25"

  add recurring <title> [--at datetime | --time duration] --interval duration [--priority value] [--tag value]
      Create a task that repeats after every interval.
      Examples:
        taskctl add recurring "Water plants" --time 1h --interval 1d --tag home
        taskctl add recurring "Water plants" --at "2026-05-18 10:00" --interval 1d

  add bounded <title> [--at datetime | --time duration] --interval duration --repeats n [--priority value] [--tag value]
      Create a recurring task with limited number of repeats.
      Examples:
        taskctl add bounded "Gym" --time 2h --interval 1w --repeats 10 --tag health
        taskctl add bounded "Gym" --at "2026-05-19 19:00" --interval 1w --repeats 10

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

General task editing:
  set-title <id> <new title>
      Change task title.
      Example:
        taskctl set-title 3 "Read C++ book"

  set-description <id> <new description>
      Change task description.
      Example:
        taskctl set-description 3 "Important university task"

  set-priority <id> <low|medium|high|critical>
      Change task priority.
      Example:
        taskctl set-priority 3 high

  add-tag <id> <tag>
      Add tag to task.
      Example:
        taskctl add-tag 3 study

  remove-tag <id> <tag>
      Remove tag from task.
      Example:
        taskctl remove-tag 3 study

Progress and type-specific editing:
  set-deadline <id> <datetime>
      Set exact calendar deadline for a task.
      Example:
        taskctl set-deadline 3 "2026-06-01 23:59"

  set-date <id> <date>
      Set date deadline (time will be 23:59:59).
      Example:
        taskctl set-date 3 "2026-06-01"

  set-time <id> <duration>
      Change time left for reminder or recurring task.
      Example:
        taskctl set-time 3 2h

  set-interval <id> <duration>
      Change repeat interval for recurring or bounded recurring task.
      Example:
        taskctl set-interval 4 1w

  set-repeats <id> <count>
      Change repeats count for bounded recurring task.
      Example:
        taskctl set-repeats 4 10

  add-value <id> <value>
      Add value to savings task progress.
      Example:
        taskctl add-value 5 5000

  advance <id>
      Move stepped or final deadline task to the next step.
      Example:
        taskctl advance 6

  overdue <id>
      Mark stepped or final deadline task as overdue.
      Example:
        taskctl overdue 6

  reset <id>
      Reset recurring or bounded recurring task to next occurrence.
      Example:
        taskctl reset 4

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
  Date/time examples:
    2026-05-20
    2026-05-20 18:30
    2026-05-20 18:30:00

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

    You can modify tags later:
      taskctl add-tag 3 university
      taskctl remove-tag 3 study

  Titles and values with spaces:
    Use quotes:
      taskctl add reminder "Read big book" --tag "very important"
      taskctl set-title 3 "Read advanced C++ book"
      taskctl set-deadline 3 "2026-06-01 23:59"

  Default behavior:
    If task type is omitted, reminder is used.
    If priority is omitted, medium is used.

  Notes:
    set-deadline, set-date, set-time work only for reminder, recurring and bounded recurring tasks.
    set-interval and reset work only for recurring and bounded recurring tasks.
    set-repeats works only for bounded recurring tasks.
    add-value works only for savings tasks.
    advance and overdue work only for stepped and final deadline tasks.
)";
}
