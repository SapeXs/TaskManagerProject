#include "storage/task_storage.h"

#include <fstream>
#include <iostream>

TaskStorage::TaskStorage(std::filesystem::path path)
    : path_(std::move(path)) {}

void TaskStorage::Save(const TaskManager&) {
  std::ofstream file(path_);

  if (!file.is_open()) {
    std::cerr << "Failed to open storage file\n";
    return;
  }

  file << "autosave\n";
}

void TaskStorage::Load(TaskManager&) {
  std::ifstream file(path_);

  if (!file.is_open()) {
    return;
  }

  std::cout << "Storage loaded\n";
}
