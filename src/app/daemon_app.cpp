#include "app/daemon_app.h"
#include "ui/command_parser.h"
#include "tasks/reminder_task.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <thread>

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

DaemonApp::DaemonApp(std::chrono::seconds autosave_interval, std::filesystem::path socket_server)
    : autosave_interval_(autosave_interval),
      storage_("tasks.txt"), socket_server_(socket_server) {}

void DaemonApp::Run() {
  CommandParser parser;

  if (!socket_server_.Start()) {
    std::cerr << "Failed to start socket server\n";
    return;
  }

  running_ = true;

  {
    std::lock_guard lock(task_mutex_);
    storage_.Load(task_manager_);
    next_id_ = 1;
    for (const TaskBase* task : task_manager_.GetAllTasks()) {
      next_id_ = std::max(next_id_, task->GetId() + 1);
    }
  }

  autosave_thread_ = std::jthread(
    [this](std::stop_token stop_token) {
      AutosaveLoop(stop_token);
    }
  );

  std::cout << "TaskManager daemon started\n";

  while (running_) {
    std::string message = socket_server_.WaitMessage();

    if (message.empty()) {
      continue;
    }

    Command command = parser.Parse(message);

    switch (command.GetType()) {
      case CommandType::kAdd: {
        if (command.GetArgs().empty()) {
          socket_server_.SendResponse("Missing title");
          break;
        }

        std::string title = JoinArgs(command.GetArgs());

        auto task = std::make_unique<ReminderTask>(
            next_id_++, std::move(title), "", TaskPriority::kMediumPriority,
            TaskBase::TagContainer{}, 0);

        {
          std::lock_guard lock(task_mutex_);
          task_manager_.AddTask(std::move(task));
        }

        socket_server_.SendResponse("Task added");
        break;
      }

      case CommandType::kList: {
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

        socket_server_.SendResponse(response);
        break;
      }

      case CommandType::kRemove: {
        if (command.GetArgs().empty()) {
          socket_server_.SendResponse("Missing task id");
          break;
        }

        int32_t id = 0;

        try {
          id = std::stoi(command.GetArgs()[0]);
        } catch (...) {
          socket_server_.SendResponse("Invalid task id");
          break;
        }

        {
          std::lock_guard lock(task_mutex_);

          if (task_manager_.FindTaskById(id) == nullptr) {
            socket_server_.SendResponse("Task not found");
            break;
          }

          task_manager_.RemoveTask(id);
        }

        socket_server_.SendResponse("Task removed");
        break;
      }

      case CommandType::kSave: {
        {
          std::lock_guard lock(task_mutex_);
          storage_.Save(task_manager_);
        }

        socket_server_.SendResponse("Saved");
        break;
      }

      default:
        socket_server_.SendResponse("Unknown command");
        break;
    }

    if (!message.empty()) {
      std::cout << "Received: " << message << '\n';
    }
  }

  autosave_thread_.request_stop();
  autosave_thread_.join();

  {
    std::lock_guard lock(task_mutex_);
    storage_.Save(task_manager_);
  }

  std::cout << "TaskManager daemon stopped\n";
}

void DaemonApp::Stop() {
  running_ = false;

  socket_server_.Shutdown();
}

void DaemonApp::AutosaveLoop(std::stop_token stop_token) {
  while (!stop_token.stop_requested()) {
    std::this_thread::sleep_for(autosave_interval_);

    {
      std::lock_guard lock(task_mutex_);

      std::cout << "Autosave...\n";

      storage_.Save(task_manager_);
    }
  }
}
