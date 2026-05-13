#pragma once

#include <filesystem>

#include "manager/task_manager.h"

class TaskStorage {
public:
    explicit TaskStorage(std::filesystem::path path);

    void Save(const TaskManager& manager);
    void Load(TaskManager& manager);

private:
    std::filesystem::path path_;
};
