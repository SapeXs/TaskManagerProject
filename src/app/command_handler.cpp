#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>

#include "tasks/reminder_task.h"
#include "app/command_handler.h"
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

}

CommandHandler::CommandHandler(TaskManager& task_manager, TaskStorage& storage,
                               std::mutex& task_mutex, int32_t& next_id)
    : task_manager_(task_manager),
      storage_(storage),
      task_mutex_(task_mutex),
      next_id_(next_id) {}

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
    default:
      return "Unknown command";
  }
}

std::string CommandHandler::HandleAdd(const Command& command) {
  if (command.GetArgs().empty()) {
    return "Missing title";
  }

  std::string title = JoinArgs(command.GetArgs());

  auto task = std::make_unique<ReminderTask>(
      next_id_++, std::move(title), "", TaskPriority::kMediumPriority,
      TaskBase::TagContainer{}, 0);

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
