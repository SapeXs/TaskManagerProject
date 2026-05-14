#include "app/daemon_app.h"

#include <chrono>
#include <iostream>
#include <thread>

DaemonApp::DaemonApp(std::chrono::seconds autosave_interval, std::filesystem::path socket_server)
    : autosave_interval_(autosave_interval),
      storage_("tasks.txt"), socket_server_(socket_server) {}

void DaemonApp::Run() {
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
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string_view message = socket_server_.WaitMessage();

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
