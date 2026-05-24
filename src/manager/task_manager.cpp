#include "manager/task_manager.h"

#include <algorithm>
#include <set>

void TaskManager::AddTask(TaskPtr task) {
  if (task == nullptr) {
    return;
  }

  const int32_t id = task->GetId();

  if (task_index_.contains(id)) {
    return;
  }

  TaskBase* raw_task = task.get();

  tasks_.push_back(std::move(task));
  task_index_[id] = raw_task;
  task_views_.push_back(raw_task);
  const_task_views_.push_back(raw_task);

  last_filtered_tasks_.clear();
}

void TaskManager::RemoveTask(int32_t id) {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return;
  }

  task_index_.erase(it);

  tasks_.erase(
      std::remove_if(tasks_.begin(), tasks_.end(),
                     [id](const TaskPtr& task) { return task != nullptr && task->GetId() == id; }),
      tasks_.end());

  task_views_.erase(
      std::remove_if(task_views_.begin(), task_views_.end(),
                     [id](const TaskBase* task) { return task != nullptr && task->GetId() == id; }),
      task_views_.end());

  const_task_views_.erase(
      std::remove_if(const_task_views_.begin(), const_task_views_.end(),
                     [id](const TaskBase* task) { return task != nullptr && task->GetId() == id; }),
      const_task_views_.end());

  last_filtered_tasks_.clear();
}

TaskBase* TaskManager::FindTaskById(int32_t id) noexcept {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return nullptr;
  }

  return it->second;
}

const TaskBase* TaskManager::FindTaskById(int32_t id) const noexcept {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return nullptr;
  }

  return it->second;
}

std::span<TaskBase* const> TaskManager::GetAllTasks() noexcept {
  return std::span<TaskBase* const>(task_views_);
}

std::span<const TaskBase* const> TaskManager::GetAllTasks() const noexcept {
  return std::span<const TaskBase* const>(const_task_views_);
}

std::span<TaskBase* const> TaskManager::GetLastFilteredTasks() noexcept {
  return std::span<TaskBase* const>(last_filtered_tasks_);
}

std::span<TaskBase* const> TaskManager::FilterByState(TaskState state) noexcept {
  return Filter([&state](const TaskBase& task) { return task.GetState() == state; });
}

std::span<TaskBase* const> TaskManager::FilterByPriority(TaskPriority priority) noexcept {
  return Filter([&priority](const TaskBase& task) { return task.GetPriority() == priority; });
}

std::span<TaskBase* const> TaskManager::FilterByTag(const std::string& tag) noexcept {
  return Filter([&tag](const TaskBase& task) { return task.HasTag(tag); });
}

std::size_t TaskManager::Size() const noexcept {
  return tasks_.size();
}

bool TaskManager::Empty() const noexcept {
  return tasks_.empty();
}

void TaskManager::Clear() noexcept {
  tasks_.clear();
  task_index_.clear();
  task_views_.clear();
  const_task_views_.clear();
  last_filtered_tasks_.clear();
}

std::vector<std::string> TaskManager::GetAllTags() const noexcept {
  std::set<std::string> unique_tags;

  for (const auto& task : tasks_) {
    if (task == nullptr) {
      continue;
    }

    for (const std::string& tag : task->GetTags()) {
      unique_tags.insert(tag);
    }
  }

  return std::vector<std::string>(unique_tags.begin(), unique_tags.end());
}
