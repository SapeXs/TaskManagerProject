#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/task_base.h"
#include "core/task_priority.h"
#include "core/task_state.h"

class TaskManager {
public:
  using TaskPtr = std::unique_ptr<TaskBase>;
  using TaskList = std::vector<TaskPtr>;

  void AddTask(TaskPtr task);
  void RemoveTask(int32_t id);

  TaskBase* FindTaskById(int32_t id) noexcept;
  const TaskBase* FindTaskById(int32_t id) const noexcept;

  std::vector<TaskBase*> GetAllTasks() noexcept;
  std::vector<const TaskBase*> GetAllTasks() const noexcept;
  std::span<TaskBase* const> GetLastFilteredTasks() noexcept;

  std::span<TaskBase* const> FilterByState(TaskState state) noexcept;
  std::span<TaskBase* const> FilterByPriority(TaskPriority priority) noexcept;
  std::span<TaskBase* const> FilterByTag(const std::string& tag) noexcept;

  template <class Predicate>
  std::span<TaskBase* const> Filter(Predicate predicate) noexcept;

  std::size_t Size() const noexcept;
  bool Empty() const noexcept;
  void Clear() noexcept;

private:
  TaskList tasks_;
  std::unordered_map<int32_t, TaskBase*> task_index_;
  std::vector<TaskBase*> last_filtered_tasks_;
};

template <class Predicate>
std::span<TaskBase* const> TaskManager::Filter(Predicate predicate) noexcept {
  last_filtered_tasks_.clear();
  last_filtered_tasks_.reserve(tasks_.size());

  for (const auto& task : tasks_) {
    if (task != nullptr && predicate(*task)) {
      last_filtered_tasks_.emplace_back(task.get());
    }
  }

  return std::span<TaskBase* const>(last_filtered_tasks_);
}
