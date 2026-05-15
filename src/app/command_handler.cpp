#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>

#include "tasks/reminder_task.h"
#include "app/command_handler.h"

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
    case CommandType::kRemove:
      return HandleRemove(command);
    case CommandType::kSave:
      return HandleSave();
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
  std::string response;

  {
    std::lock_guard lock(task_mutex_);
    auto tasks = task_manager_.GetAllTasks();

    for (const TaskBase* task : tasks) {
      response += std::to_string(task->GetId());
      response += " | ";
      response += task->GetTitle();
      response += '\n';
    }
  }

  if (response.empty()) {
    response = "No tasks";
  }

  return response;
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
