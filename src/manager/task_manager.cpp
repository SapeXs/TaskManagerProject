#include "manager/task_manager.h"

#include <algorithm>

template <class Predicate>
std::vector<TaskBase *> TaskManager::Filter(Predicate predicate) noexcept {
  std::vector<TaskBase *> result;
  result.reserve(tasks_.size());

  for (const auto &task : tasks_) {
    if (task != nullptr && predicate(*task)) {
      result.emplace_back(task.get());
    }
  }

  return result;
}

void TaskManager::AddTask(TaskPtr task) {
  if (task == nullptr) {
    return;
  }

  const int32_t id = task->GetId();

  if (task_index_.contains(id)) {
    return;
  }

  TaskBase *raw_task = task.get();
  tasks_.push_back(std::move(task));
  task_index_[id] = raw_task;
}

void TaskManager::RemoveTask(int32_t id) {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return;
  }

  task_index_.erase(it);

  tasks_.erase(std::remove_if(tasks_.begin(), tasks_.end(),
                              [id](const TaskPtr &task) {
                                return task != nullptr && task->GetId() == id;
                              }),
               tasks_.end());

  return;
}

TaskBase *TaskManager::FindTaskById(int32_t id) noexcept {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return nullptr;
  }

  return it->second;
}

const TaskBase *TaskManager::FindTaskById(int32_t id) const noexcept {
  auto it = task_index_.find(id);
  if (it == task_index_.end()) {
    return nullptr;
  }

  return it->second;
}

std::vector<TaskBase *> TaskManager::GetAllTasks() noexcept {
  std::vector<TaskBase *> result;
  result.reserve(tasks_.size());

  for (const auto &task : tasks_) {
    if (task != nullptr) {
      result.emplace_back(task.get());
    }
  }

  return result;
}

std::vector<const TaskBase *> TaskManager::GetAllTasks() const noexcept {
  std::vector<const TaskBase *> result;
  result.reserve(tasks_.size());

  for (const auto &task : tasks_) {
    if (task != nullptr) {
      result.emplace_back(task.get());
    }
  }

  return result;
}

std::vector<TaskBase *> TaskManager::FilterByState(TaskState state) noexcept {
  return Filter(
      [&state](const TaskBase &task) { return task.GetState() == state; });
}

std::vector<TaskBase *>
TaskManager::FilterByPriority(TaskPriority priority) noexcept {
  return Filter([&priority](const TaskBase &task) {
    return task.GetPriority() == priority;
  });
}

std::vector<TaskBase *>
TaskManager::FilterByTag(const std::string &tag) noexcept {
  return Filter([&tag](const TaskBase &task) { return task.HasTag(tag); });
}

std::size_t TaskManager::Size() const noexcept { return tasks_.size(); }

bool TaskManager::Empty() const noexcept { return tasks_.empty(); }

void TaskManager::Clear() noexcept {
  tasks_.clear();
  task_index_.clear();
}
