#include "storage/task_storage.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>

#include "core/task_converters.h"
#include "storage/storage_codec.h"
#include "storage/task_constructor.h"

TaskStorage::TaskStorage(std::filesystem::path path) : path_(std::move(path)) {}

void TaskStorage::Save(const TaskManager& manager) {
  std::ofstream file(path_);

  if (!file.is_open()) {
    std::cerr << "Failed to open storage file\n";
    return;
  }

  for (const TaskBase* task : manager.GetAllTasks()) {
    if (task == nullptr) {
      continue;
    }

    file << EscapeStorageField(task->GetTypeName()) << '|' << task->GetId() << '|'
         << EscapeStorageField(task->GetTitle()) << '|'
         << EscapeStorageField(task->GetDescription()) << '|'
         << TaskPriorityToStorageValue(task->GetPriority()) << '|'
         << EscapeStorageField(TagsToStorageString(task->GetTags()));

    for (const std::string& field : task->GetStorageFields()) {
      file << '|' << EscapeStorageField(field);
    }

    file << '\n';
  }
}

void TaskStorage::Load(TaskManager& manager) {
  std::ifstream file(path_);

  if (!file.is_open()) {
    return;
  }

  manager.Clear();

  std::string line;

  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    try {
      std::vector<std::string> fields = SplitStorageLine(line);
      std::unique_ptr<TaskBase> task = CreateTaskFromStorageFields(fields);

      if (task != nullptr) {
        manager.AddTask(std::move(task));
      }
    } catch (...) {
      std::cerr << "Failed to parse task line: " << line << '\n';
    }
  }
}
