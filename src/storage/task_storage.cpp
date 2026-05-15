#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "storage/task_storage.h"
#include "tasks/tasks_lib.h"
#include "core/task_name.h"

namespace {

std::string Escape(const std::string& value) {
  std::string result;
  for (char ch : value) {
    if (ch == '\\' || ch == '|') {
      result += '\\';
    }
    result += ch;
  }
  return result;
}

std::vector<std::string> SplitEscaped(const std::string& line) {
  std::vector<std::string> result;
  std::string current;
  bool escaped = false;

  for (char ch : line) {
    if (escaped) {
      current += ch;
      escaped = false;
    } else if (ch == '\\') {
      escaped = true;
    } else if (ch == '|') {
      result.push_back(current);
      current.clear();
    } else {
      current += ch;
    }
  }

  result.push_back(current);
  return result;
}

std::string TagsToString(const TaskBase::TagContainer& tags) {
  std::string result;
  bool first = true;

  for (const std::string& tag : tags) {
    if (!first) {
      result += ',';
    }
    first = false;
    result += tag;
  }

  return result;
}

TaskBase::TagContainer ParseTags(const std::string& value) {
  TaskBase::TagContainer tags;
  std::stringstream stream(value);
  std::string tag;

  while (std::getline(stream, tag, ',')) {
    if (!tag.empty()) {
      tags.insert(tag);
    }
  }

  return tags;
}

TaskPriority ParsePriority(const std::string& value) {
  int priority = std::stoi(value);

  switch (priority) {
    case 0:
      return TaskPriority::kLowPriority;
    case 1:
      return TaskPriority::kMediumPriority;
    case 2:
      return TaskPriority::kHighPriority;
    case 3:
      return TaskPriority::kCriticalPriority;
    default:
      return TaskPriority::kMediumPriority;
  }
}

std::unique_ptr<TaskBase> CreateTaskFromFields(const std::vector<std::string>& parts) {
  if (parts.size() < 6) {
    return nullptr;
  }

  const std::string& type = parts[0];
  int32_t id = std::stoi(parts[1]);
  std::string title = parts[2];
  std::string description = parts[3];
  TaskPriority priority = ParsePriority(parts[4]);
  TaskBase::TagContainer tags = ParseTags(parts[5]);

  if (type == kNameReminderTask && parts.size() == 7) {
    return std::make_unique<ReminderTask>(id, std::move(title), std::move(description), priority,
                                          std::move(tags), std::stoll(parts[6]));
  }

  if (type == kNameRecurringTask && parts.size() == 8) {
    return std::make_unique<RecurringTask>(id, std::move(title), std::move(description), priority,
                                           std::move(tags), std::stoll(parts[6]),
                                           std::stoll(parts[7]));
  }

  if (type == kNameBoundedRecurringTask && parts.size() == 9) {
    return std::make_unique<BoundedRecurringTask>(
        id, std::move(title), std::move(description), priority, std::move(tags),
        std::stoll(parts[6]), std::stoll(parts[7]), std::stoi(parts[8]));
  }

  if (type == kNameSavingTask && parts.size() == 8) {
    return std::make_unique<SavingsTask>(id, std::move(title), std::move(description), priority,
                                         std::move(tags), std::stoll(parts[6]),
                                         std::stoll(parts[7]));
  }

  if (type == kNameSteppedDeadlineTask && parts.size() >= 8) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(parts[6]));
    std::vector<std::string> steps(parts.begin() + 7, parts.end());

    return std::make_unique<SteppedDeadlineTask>(id, std::move(title), std::move(description),
                                                 priority, std::move(tags), std::move(steps),
                                                 current_step);
  }

  if (type == kNameFinalDeadlineTask && parts.size() >= 9) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(parts[6]));
    std::vector<std::string> steps(parts.begin() + 7, parts.end() - 1);
    std::string final_deadline_text = parts.back();

    return std::make_unique<FinalDeadlineTask>(id, std::move(title), std::move(description),
                                               priority, std::move(tags), std::move(steps),
                                               std::move(final_deadline_text), current_step);
  }

  return nullptr;
}

}

TaskStorage::TaskStorage(std::filesystem::path path) : path_(std::move(path)) {}

void TaskStorage::Save(const TaskManager& manager) {
  std::ofstream file(path_);

  if (!file.is_open()) {
    std::cerr << "Failed to open storage file\n";
    return;
  }

  for (const TaskBase* task : manager.GetAllTasks()) {
    file << Escape(std::string(task->GetTypeName())) << '|'
         << task->GetId() << '|'
         << Escape(task->GetTitle()) << '|'
         << Escape(task->GetDescription()) << '|'
         << static_cast<int>(task->GetPriority()) << '|'
         << Escape(TagsToString(task->GetTags()));

    for (const std::string& field : task->GetStorageFields()) {
      file << '|' << Escape(field);
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
      std::vector<std::string> parts = SplitEscaped(line);
      std::unique_ptr<TaskBase> task = CreateTaskFromFields(parts);

      if (task != nullptr) {
        manager.AddTask(std::move(task));
      }
    } catch (...) {
      std::cerr << "Failed to parse task line: " << line << '\n';
    }
  }
}
