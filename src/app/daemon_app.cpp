#include "app/daemon_app.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include "app/command_handler.h"
#include "commands/command_parser.h"

DaemonApp::DaemonApp(std::chrono::seconds autosave_interval, std::filesystem::path socket_server)
    : autosave_interval_(autosave_interval), storage_("tasks.txt"), socket_server_(socket_server) {}

void DaemonApp::Run() {
  CommandParser parser;
  CommandHandler handler(task_manager_, storage_, task_mutex_, next_id_);

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

  autosave_thread_ = std::jthread([this](std::stop_token stop_token) { AutosaveLoop(stop_token); });

  std::cout << "TaskManager daemon started\n";

  while (running_) {
    std::string message = socket_server_.WaitMessage();

    if (!running_) {
      break;
    }

    if (message.empty()) {
      continue;
    }

    Command command = parser.Parse(message);
    std::string response = handler.Handle(command);

    socket_server_.SendResponse(response);

    std::cout << "Received: " << message << '\n';
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
