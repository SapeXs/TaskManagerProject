#include "app/daemon_app.h"
#include "ui/command_parser.h"
#include "tasks/reminder_task.h"

#include <chrono>
#include <iostream>
#include <thread>

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

        static int32_t next_id = 1;

        auto task = std::make_unique<ReminderTask>(
            next_id++,
            command.GetArgs()[0],
            "",
            TaskPriority::kMediumPriority,
            TaskBase::TagContainer{},
            0);

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
