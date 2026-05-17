#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <thread>

#include "ipc/unix_socket_server.h"
#include "manager/task_manager.h"
#include "storage/task_storage.h"

class DaemonApp {
 public:
  explicit DaemonApp(std::chrono::seconds autosave_interval, std::filesystem::path socket_server);

  void Run();
  void Stop();

  void AutosaveLoop(std::stop_token stop_token);

 private:
  std::chrono::seconds autosave_interval_;

  std::atomic_bool running_ = false;

  std::mutex task_mutex_;

  TaskManager task_manager_;
  TaskStorage storage_;

  std::jthread autosave_thread_;

  UnixSocketServer socket_server_;

  int32_t next_id_ = 1;
};
