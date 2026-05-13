#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>

#include "manager/task_manager.h"
#include "storage/task_storage.h"

class DaemonApp {
public:
    explicit DaemonApp(std::chrono::seconds autosave_interval);

    void Run();
    void Stop();

private:
    void AutosaveLoop(std::stop_token stop_token);

    std::chrono::seconds autosave_interval_;

    std::atomic_bool running_ = false;

    std::mutex task_mutex_;

    TaskManager task_manager_;
    TaskStorage storage_;

    std::jthread autosave_thread_;
};
