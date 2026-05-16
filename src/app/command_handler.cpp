#include "app/command_handler.h"

#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "storage/task_constructor.h"
#include "tasks/tasks_lib.h"

#include "format/task_formatter.h"

namespace {

std::string JoinArgs(std::span<const std::string> args) {
  std::string result;

  for (std::size_t i = 0; i < args.size(); ++i) {
    if (i > 0) {
      result += ' ';
    }

    result += args[i];
  }

  return result;
}

bool ParsePriorityArg(const std::string& value, TaskPriority& priority) {
  if (value == "low") {
    priority = TaskPriority::kLowPriority;
    return true;
  }

  if (value == "medium") {
    priority = TaskPriority::kMediumPriority;
    return true;
  }

  if (value == "high") {
    priority = TaskPriority::kHighPriority;
    return true;
  }

  if (value == "critical") {
    priority = TaskPriority::kCriticalPriority;
    return true;
  }

  return false;
}

bool ParseStateArg(const std::string& value, TaskState& state) {
  if (value == "not_started") {
    state = TaskState::kNotStarted;
    return true;
  }

  if (value == "in_progress") {
    state = TaskState::kInProgress;
    return true;
  }

  if (value == "done") {
    state = TaskState::kDone;
    return true;
  }

  if (value == "overdue") {
    state = TaskState::kOverdue;
    return true;
  }

  if (value == "scheduled") {
    state = TaskState::kScheduled;
    return true;
  }

  return false;
}

bool ParseTaskType(const std::string& value, AddTaskType& type) {
  if (value == "reminder") {
    type = AddTaskType::kReminder;
    return true;
  }

  if (value == "recurring") {
    type = AddTaskType::kRecurring;
    return true;
  }

  if (value == "bounded" || value == "bounded-recurring") {
    type = AddTaskType::kBoundedRecurring;
    return true;
  }

  if (value == "savings") {
    type = AddTaskType::kSavings;
    return true;
  }

  if (value == "stepped") {
    type = AddTaskType::kSteppedDeadline;
    return true;
  }

  if (value == "final") {
    type = AddTaskType::kFinalDeadline;
    return true;
  }

  return false;
}

}  // namespace

CommandHandler::CommandHandler(TaskManager& task_manager, TaskStorage& storage,
                               std::mutex& task_mutex, int32_t& next_id)
    : task_manager_(task_manager), storage_(storage), task_mutex_(task_mutex), next_id_(next_id) {}

std::string CommandHandler::Handle(const Command& command) {
  switch (command.GetType()) {
    case CommandType::kAdd:
      return HandleAdd(command);
    case CommandType::kList:
      return HandleList();
    case CommandType::kFind:
      return HandleFind(command);
    case CommandType::kRemove:
      return HandleRemove(command);
    case CommandType::kFilter:
      return HandleFilter(command);
    case CommandType::kSave:
      return HandleSave();
    case CommandType::kHelp:
      return HandleHelp();
    case CommandType::kClear:
      return HandleClear();
    default:
      return "Unknown command";
  }
}

std::string CommandHandler::HandleAdd(const Command& command) {
  if (command.GetArgs().empty()) {
    return "Usage: add [type] <title> [options]";
  }

  AddTaskOptions options;
  std::vector<std::string> title_parts;

  auto args = command.GetArgs();
  std::size_t start = 0;

  AddTaskType parsed_type;
  if (ParseTaskType(args[0], parsed_type)) {
    options.type = parsed_type;
    start = 1;
  }

  for (std::size_t i = start; i < args.size(); ++i) {
    if (args[i] == "--priority") {
      if (i + 1 >= args.size()) {
        return "Missing priority value";
      }

      if (!ParsePriorityArg(args[i + 1], options.priority)) {
        return "Invalid priority. Available: low, medium, high, critical";
      }

      ++i;
      continue;
    }

    if (args[i] == "--tag") {
      if (i + 1 >= args.size()) {
        return "Missing tag value";
      }

      options.tags.insert(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--seconds") {
      if (i + 1 >= args.size()) {
        return "Missing value for --seconds";
      }

      options.seconds_left = std::stoll(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--interval") {
      if (i + 1 >= args.size()) {
        return "Missing value for --interval";
      }

      options.repeat_interval_seconds = std::stoll(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--repeats") {
      if (i + 1 >= args.size()) {
        return "Missing value for --repeats";
      }

      options.repeats_left = std::stoi(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--current") {
      if (i + 1 >= args.size()) {
        return "Missing value for --current";
      }

      options.current_value = std::stoll(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--target") {
      if (i + 1 >= args.size()) {
        return "Missing value for --target";
      }

      options.target_value = std::stoll(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--step") {
      if (i + 1 >= args.size()) {
        return "Missing step text";
      }

      options.step_texts.push_back(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--current-step") {
      if (i + 1 >= args.size()) {
        return "Missing value for --current-step";
      }

      options.current_step = static_cast<std::size_t>(std::stoull(args[i + 1]));
      ++i;
      continue;
    }

    if (args[i] == "--final") {
      if (i + 1 >= args.size()) {
        return "Missing final deadline text";
      }

      options.final_deadline_text = args[i + 1];
      ++i;
      continue;
    }

    title_parts.push_back(args[i]);
  }

  if (title_parts.empty()) {
    return "Missing title";
  }

  options.title = JoinArgs(title_parts);

  std::string error;
  std::unique_ptr<TaskBase> task = CreateTaskFromAddOptions(next_id_, std::move(options), error);

  if (task == nullptr) {
    return error;
  }

  ++next_id_;

  {
    std::lock_guard lock(task_mutex_);
    task_manager_.AddTask(std::move(task));
  }

  return "Task added";
}

std::string CommandHandler::HandleList() {
  std::lock_guard lock(task_mutex_);
  auto tasks = task_manager_.GetAllTasks();
  return TaskFormatter::FormatTaskList(tasks);
}

std::string CommandHandler::HandleFind(const Command& command) {
  if (command.GetArgs().empty()) {
    return "Missing task id";
  }

  int32_t id = 0;

  try {
    id = std::stoi(command.GetArgs()[0]);
  } catch (const std::exception&) {
    return "Invalid task id";
  }

  std::lock_guard lock(task_mutex_);

  const TaskBase* task = task_manager_.FindTaskById(id);

  if (task == nullptr) {
    return "Task not found";
  }

  return TaskFormatter::FormatTask(*task);
}

std::string CommandHandler::HandleFilter(const Command& command) {
  if (command.GetArgs().size() < 2) {
    return "Usage: filter <priority|state|tag> <value>";
  }

  const std::string& field = command.GetArgs()[0];
  const std::string& value = command.GetArgs()[1];

  std::lock_guard lock(task_mutex_);

  if (field == "priority") {
    TaskPriority priority;

    if (!ParsePriorityArg(value, priority)) {
      return "Invalid priority. Available: low, medium, high, critical";
    }

    auto tasks = task_manager_.FilterByPriority(priority);
    return TaskFormatter::FormatTaskList(tasks);
  }

  if (field == "state") {
    TaskState state;

    if (!ParseStateArg(value, state)) {
      return "Invalid state. Available: not_started, in_progress, done, overdue, scheduled";
    }

    auto tasks = task_manager_.FilterByState(state);
    return TaskFormatter::FormatTaskList(tasks);
  }

  if (field == "tag") {
    auto tasks = task_manager_.FilterByTag(value);
    return TaskFormatter::FormatTaskList(tasks);
  }

  return "Unknown filter field. Available: priority, state, tag";
}

std::string CommandHandler::HandleRemove(const Command& command) {
  if (command.GetArgs().empty()) {
    return "Missing task id";
  }

  int32_t id = 0;

  try {
    id = std::stoi(command.GetArgs()[0]);
  } catch (const std::exception&) {
    return "Invalid task id";
  }

  {
    std::lock_guard lock(task_mutex_);

    if (task_manager_.FindTaskById(id) == nullptr) {
      return "Task not found";
    }

    task_manager_.RemoveTask(id);
  }

  return "Task removed";
}

std::string CommandHandler::HandleSave() {
  {
    std::lock_guard lock(task_mutex_);
    storage_.Save(task_manager_);
  }

  return "Saved";
}

std::string CommandHandler::HandleHelp() {
  return TaskFormatter::FormatHelp();
}

std::string CommandHandler::HandleClear() {
  {
    std::lock_guard lock(task_mutex_);
    task_manager_.Clear();
    storage_.Save(task_manager_);
    next_id_ = 1;
  }

  return "All tasks cleared";
}
