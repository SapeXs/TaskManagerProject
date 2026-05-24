#include "app/command_handler.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "core/date_time_utils.h"
#include "core/duration_parser.h"
#include "core/task_converters.h"
#include "storage/task_constructor.h"
#include "tasks/tasks_lib.h"

#include "format/task_formatter.h"

namespace {

std::string JoinArgs(std::span<const std::string> args) {
  std::size_t result_size = 0;

  for (const std::string& arg : args) {
    result_size += arg.size();
  }

  if (!args.empty()) {
    result_size += args.size() - 1;
  }

  std::string result;
  result.reserve(result_size);

  for (std::size_t i = 0; i < args.size(); ++i) {
    if (i > 0) {
      result += ' ';
    }

    result += args[i];
  }

  return result;
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

bool ReadInt64(std::span<const std::string> args, std::size_t& index, std::string_view option,
               int64_t& value, std::string& error) {
  if (index + 1 >= args.size()) {
    error = "Missing value for " + std::string(option);
    return false;
  }
  try {
    std::size_t parsed_chars = 0;
    value = std::stoll(args[index + 1], &parsed_chars);
    if (parsed_chars != args[index + 1].size()) {
      error = "Invalid integer value for " + std::string(option);
      return false;
    }
  } catch (const std::exception&) {
    error = "Invalid integer value for " + std::string(option);
    return false;
  }
  ++index;
  return true;
}

bool ReadInt32(std::span<const std::string> args, std::size_t& index, std::string_view option,
               int32_t& value, std::string& error) {
  int64_t parsed_value = 0;
  if (!ReadInt64(args, index, option, parsed_value, error))
    return false;
  if (parsed_value < INT32_MIN || parsed_value > INT32_MAX) {
    error = "Value for " + std::string(option) + " is out of int32 range";
    return false;
  }
  value = static_cast<int32_t>(parsed_value);
  return true;
}

bool ReadSize(std::span<const std::string> args, std::size_t& index, std::string_view option,
              std::size_t& value, std::string& error) {
  int64_t parsed_value = 0;
  if (!ReadInt64(args, index, option, parsed_value, error))
    return false;
  if (parsed_value < 0) {
    error = "Value for " + std::string(option) + " must be non-negative";
    return false;
  }
  value = static_cast<std::size_t>(parsed_value);
  return true;
}

bool ReadTaskId(std::span<const std::string> args, int32_t& id, std::string& error) {
  if (args.empty()) {
    error = "Missing task id";
    return false;
  }
  try {
    std::size_t parsed_chars = 0;
    id = std::stoi(args[0], &parsed_chars);
    if (parsed_chars != args[0].size()) {
      error = "Invalid task id";
      return false;
    }
  } catch (const std::exception&) {
    error = "Invalid task id";
    return false;
  }
  return true;
}

bool ReadInt64Value(const std::string& text, int64_t& value, std::string& error) {
  try {
    std::size_t parsed_chars = 0;
    value = std::stoll(text, &parsed_chars);
    if (parsed_chars != text.size()) {
      error = "Invalid integer value";
      return false;
    }
  } catch (const std::exception&) {
    error = "Invalid integer value";
    return false;
  }
  return true;
}

bool ReadInt32Value(const std::string& text, int32_t& value, std::string& error) {
  int64_t parsed_value = 0;
  if (!ReadInt64Value(text, parsed_value, error))
    return false;
  if (parsed_value < INT32_MIN || parsed_value > INT32_MAX) {
    error = "Integer value is out of int32 range";
    return false;
  }
  value = static_cast<int32_t>(parsed_value);
  return true;
}

}  // namespace

CommandHandler::CommandHandler(TaskManager& task_manager, TaskStorage& storage,
                               std::mutex& task_mutex, int32_t& next_id,
                               std::unordered_set<int32_t>& notified_tasks)
    : task_manager_(task_manager),
      storage_(storage),
      task_mutex_(task_mutex),
      next_id_(next_id),
      notified_tasks_(notified_tasks) {}

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
    case CommandType::kTags:
      return HandleTags();
    case CommandType::kSave:
      return HandleSave();
    case CommandType::kHelp:
      return HandleHelp();
    case CommandType::kExit:
      return HandleShutdown();
    case CommandType::kClear:
      return HandleClear();
    case CommandType::kSetTitle:
      return HandleSetTitle(command);
    case CommandType::kSetDescription:
      return HandleSetDescription(command);
    case CommandType::kSetPriority:
      return HandleSetPriority(command);
    case CommandType::kAddTag:
      return HandleAddTag(command);
    case CommandType::kRemoveTag:
      return HandleRemoveTag(command);
    case CommandType::kSetTime:
      return HandleSetTime(command);
    case CommandType::kSetInterval:
      return HandleSetInterval(command);
    case CommandType::kSetRepeats:
      return HandleSetRepeats(command);
    case CommandType::kAddValue:
      return HandleAddValue(command);
    case CommandType::kAdvance:
      return HandleAdvance(command);
    case CommandType::kOverdue:
      return HandleOverdue(command);
    case CommandType::kReset:
      return HandleReset(command);
    case CommandType::kSetDeadline:
      return HandleSetDeadline(command);
    case CommandType::kSetDate:
      return HandleSetDate(command);
    default:
      return "Unknown command";
  }
}

std::string CommandHandler::HandleAdd(const Command& command) {
  if (command.GetArgs().empty()) {
    return "Usage: add [type] <title> [options]";
  }

  std::string error;
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
      if (i + 1 >= args.size())
        return "Missing priority value";
      if (!ParseTaskPriority(args[i + 1], options.priority)) {
        return "Invalid priority. Available: low, medium, high, critical";
      }
      ++i;
      continue;
    }

    if (args[i] == "--tag") {
      if (i + 1 >= args.size())
        return "Missing tag value";
      options.tags.insert(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--at" || args[i] == "--date") {
      if (i + 1 >= args.size())
        return "Missing value for " + args[i];
      if (options.deadline.has_value())
        return "Use only one time/date flag";

      auto parsed = ParseDateTime(args[i + 1]);
      if (!parsed)
        return "Invalid date/time format. Expected: YYYY-MM-DD or YYYY-MM-DD HH:MM";
      options.deadline = *parsed;
      ++i;
      continue;
    }

    if (args[i] == "--seconds" || args[i] == "--time") {
      if (i + 1 >= args.size())
        return "Missing value for " + args[i];
      if (options.deadline.has_value())
        return "Use only one time/date flag";

      int64_t value = 0;
      if (!TryParseDuration(args[i + 1], value, error))
        return error;

      options.deadline = std::chrono::system_clock::now() + std::chrono::seconds(value);
      ++i;
      continue;
    }

    if (args[i] == "--interval") {
      if (i + 1 >= args.size())
        return "Missing value for --interval";
      int64_t value = 0;
      if (!TryParseDuration(args[i + 1], value, error))
        return error;
      options.repeat_interval_seconds = value;
      ++i;
      continue;
    }

    if (args[i] == "--repeats") {
      int32_t value = 0;
      if (!ReadInt32(args, i, "--repeats", value, error))
        return error;
      options.repeats_left = value;
      continue;
    }

    if (args[i] == "--current") {
      int64_t value = 0;
      if (!ReadInt64(args, i, "--current", value, error))
        return error;
      options.current_value = value;
      continue;
    }

    if (args[i] == "--target") {
      int64_t value = 0;
      if (!ReadInt64(args, i, "--target", value, error))
        return error;
      options.target_value = value;
      continue;
    }

    if (args[i] == "--step") {
      if (i + 1 >= args.size())
        return "Missing step text";
      options.step_texts.push_back(args[i + 1]);
      ++i;
      continue;
    }

    if (args[i] == "--current-step") {
      std::size_t value = 0;
      if (!ReadSize(args, i, "--current-step", value, error))
        return error;
      options.current_step = value;
      continue;
    }

    if (args[i] == "--final") {
      if (i + 1 >= args.size())
        return "Missing final deadline text";
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

  std::unique_ptr<TaskBase> task = CreateTaskFromAddOptions(next_id_, std::move(options), error);

  if (task == nullptr) {
    return error;
  }

  ++next_id_;

  {
    std::lock_guard lock(task_mutex_);
    task_manager_.AddTask(std::move(task));
    SaveTasksLocked();
  }

  return "Task added";
}

std::string CommandHandler::HandleList() {
  std::lock_guard lock(task_mutex_);
  auto tasks = task_manager_.GetAllTasks();
  return TaskFormatter::FormatTaskList(tasks);
}

std::string CommandHandler::HandleTags() {
  std::lock_guard lock(task_mutex_);
  return TaskFormatter::FormatTags(task_manager_.GetAllTags());
}

std::string CommandHandler::HandleFind(const Command& command) {
  if (command.GetArgs().empty())
    return "Missing task id";
  int32_t id = 0;
  try {
    id = std::stoi(command.GetArgs()[0]);
  } catch (...) {
    return "Invalid task id";
  }
  std::lock_guard lock(task_mutex_);
  const TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";
  return TaskFormatter::FormatTask(*task);
}

std::string CommandHandler::HandleFilter(const Command& command) {
  if (command.GetArgs().size() < 2)
    return "Usage: filter <priority|state|tag> <value>";
  const std::string& field = command.GetArgs()[0];
  const std::string& value = command.GetArgs()[1];

  std::lock_guard lock(task_mutex_);

  if (field == "priority") {
    TaskPriority priority;
    if (!ParseTaskPriority(value, priority))
      return "Invalid priority. Available: low, medium, high, critical";
    return TaskFormatter::FormatTaskList(task_manager_.FilterByPriority(priority));
  }
  if (field == "state") {
    TaskState state;
    if (!ParseTaskState(value, state))
      return "Invalid state. Available: not_started, in_progress, done, overdue, scheduled";
    return TaskFormatter::FormatTaskList(task_manager_.FilterByState(state));
  }
  if (field == "tag") {
    return TaskFormatter::FormatTaskList(task_manager_.FilterByTag(value));
  }
  return "Unknown filter field. Available: priority, state, tag";
}

std::string CommandHandler::HandleRemove(const Command& command) {
  if (command.GetArgs().empty())
    return "Missing task id";
  int32_t id = 0;
  try {
    id = std::stoi(command.GetArgs()[0]);
  } catch (...) {
    return "Invalid task id";
  }
  {
    std::lock_guard lock(task_mutex_);
    if (task_manager_.FindTaskById(id) == nullptr)
      return "Task not found";
    task_manager_.RemoveTask(id);
    ForgetNotificationLocked(id);
    SaveTasksLocked();
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

std::string CommandHandler::HandleShutdown() {
  return "TaskManager daemon shutting down";
}

std::string CommandHandler::HandleClear() {
  {
    std::lock_guard lock(task_mutex_);
    task_manager_.Clear();
    next_id_ = 1;
    ClearNotificationsLocked();
    SaveTasksLocked();
  }

  return "All tasks cleared";
}

std::string CommandHandler::HandleSetTitle(const Command& command) {
  if (command.GetArgs().size() < 2)
    return "Usage: set-title <id> <new title>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::vector<std::string> title_parts(command.GetArgs().begin() + 1, command.GetArgs().end());
  std::string title = JoinArgs(title_parts);

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  task->SetTitle(std::move(title));
  SaveTasksLocked();
  return "Title updated";
}

std::string CommandHandler::HandleSetDescription(const Command& command) {
  if (command.GetArgs().size() < 2)
    return "Usage: set-description <id> <new description>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::vector<std::string> description_parts(command.GetArgs().begin() + 1,
                                             command.GetArgs().end());
  std::string description = JoinArgs(description_parts);

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  task->SetDescription(std::move(description));
  SaveTasksLocked();
  return "Description updated";
}

std::string CommandHandler::HandleSetPriority(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: set-priority <id> <low|medium|high|critical>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  TaskPriority priority;
  if (!ParseTaskPriority(command.GetArgs()[1], priority))
    return "Invalid priority";

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  task->SetPriority(priority);
  SaveTasksLocked();
  return "Priority updated";
}

std::string CommandHandler::HandleAddTag(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: add-tag <id> <tag>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  task->AddTag(command.GetArgs()[1]);
  SaveTasksLocked();
  return "Tag added";
}

std::string CommandHandler::HandleRemoveTag(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: remove-tag <id> <tag>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  task->RemoveTag(command.GetArgs()[1]);
  SaveTasksLocked();
  return "Tag removed";
}

std::string CommandHandler::HandleSetTime(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: set-time <id> <duration>";

  std::string error;
  int32_t id = 0;
  int64_t seconds = 0;

  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;
  if (!TryParseDuration(command.GetArgs()[1], seconds, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  if (auto* reminder = dynamic_cast<ReminderTask*>(task); reminder != nullptr) {
    reminder->SetSecondsLeft(seconds);
    ForgetNotificationLocked(id);
    SaveTasksLocked();
    return "Time updated";
  }

  if (auto* recurring = dynamic_cast<RecurringTask*>(task); recurring != nullptr) {
    recurring->SetSecondsLeft(seconds);
    ForgetNotificationLocked(id);
    SaveTasksLocked();
    return "Time updated";
  }

  return "This task type does not support time";
}

std::string CommandHandler::HandleSetInterval(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: set-interval <id> <duration>";

  std::string error;
  int32_t id = 0;
  int64_t interval = 0;

  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;
  if (!TryParseDuration(command.GetArgs()[1], interval, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* recurring = dynamic_cast<RecurringTask*>(task);
  if (recurring == nullptr)
    return "This task type does not support interval";

  recurring->SetRepeatIntervalSeconds(interval);
  ForgetNotificationLocked(id);
  SaveTasksLocked();
  return "Interval updated";
}

std::string CommandHandler::HandleSetRepeats(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: set-repeats <id> <count>";
  std::string error;
  int32_t id = 0;
  int32_t repeats = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;
  if (!ReadInt32Value(command.GetArgs()[1], repeats, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* bounded = dynamic_cast<BoundedRecurringTask*>(task);
  if (bounded == nullptr)
    return "This task type does not support repeats";

  bounded->SetRepeatsLeft(repeats);
  SaveTasksLocked();
  return "Repeats updated";
}

std::string CommandHandler::HandleAddValue(const Command& command) {
  if (command.GetArgs().size() != 2)
    return "Usage: add-value <id> <value>";
  std::string error;
  int32_t id = 0;
  int64_t value = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;
  if (!ReadInt64Value(command.GetArgs()[1], value, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* savings = dynamic_cast<SavingsTask*>(task);
  if (savings == nullptr)
    return "This task type does not support value progress";

  savings->AddValue(value);
  SaveTasksLocked();
  return "Value updated";
}

std::string CommandHandler::HandleAdvance(const Command& command) {
  if (command.GetArgs().size() != 1)
    return "Usage: advance <id>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* stepped = dynamic_cast<SteppedDeadlineTask*>(task);
  if (stepped == nullptr)
    return "This task type does not support steps";

  stepped->AdvanceStep();
  ForgetNotificationLocked(id);
  SaveTasksLocked();
  return "Step advanced";
}

std::string CommandHandler::HandleOverdue(const Command& command) {
  if (command.GetArgs().size() != 1)
    return "Usage: overdue <id>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* stepped = dynamic_cast<SteppedDeadlineTask*>(task);
  if (stepped == nullptr)
    return "This task type does not support overdue state";

  stepped->SetOverdue();
  SaveTasksLocked();
  return "Task marked overdue";
}

std::string CommandHandler::HandleReset(const Command& command) {
  if (command.GetArgs().size() != 1)
    return "Usage: reset <id>";
  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  auto* recurring = dynamic_cast<RecurringTask*>(task);
  if (recurring == nullptr)
    return "This task type does not support reset";

  recurring->ResetToNextOccurrence();
  ForgetNotificationLocked(id);
  SaveTasksLocked();
  return "Task reset to next occurrence";
}

std::string CommandHandler::HandleSetDeadline(const Command& command) {
  if (command.GetArgs().size() < 2) {
    return "Usage: set-deadline <id> \"YYYY-MM-DD HH:MM\"";
  }

  std::string error;
  int32_t id = 0;
  if (!ReadTaskId(command.GetArgs(), id, error))
    return error;

  auto parsed = ParseDateTime(command.GetArgs()[1]);
  if (!parsed)
    return "Invalid date/time format. Expected: YYYY-MM-DD or YYYY-MM-DD HH:MM";

  std::lock_guard lock(task_mutex_);
  TaskBase* task = task_manager_.FindTaskById(id);
  if (task == nullptr)
    return "Task not found";

  if (auto* reminder = dynamic_cast<ReminderTask*>(task); reminder != nullptr) {
    reminder->SetDeadline(*parsed);
  } else if (auto* recurring = dynamic_cast<RecurringTask*>(task); recurring != nullptr) {
    recurring->SetDeadline(*parsed);
  } else {
    return "This task type does not support deadline";
  }
  ForgetNotificationLocked(id);
  SaveTasksLocked();
  return "Deadline updated";
}

std::string CommandHandler::HandleSetDate(const Command& command) {
  return HandleSetDeadline(command);
}

void CommandHandler::SaveTasksLocked() {
  storage_.Save(task_manager_);
}

void CommandHandler::ForgetNotificationLocked(int32_t task_id) {
  notified_tasks_.erase(task_id);
}

void CommandHandler::ClearNotificationsLocked() {
  notified_tasks_.clear();
}
