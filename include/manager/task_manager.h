#pragma once

#include <cstdint>
#include <memory>
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

  std::vector<TaskBase*> FilterByState(TaskState state) noexcept;
  std::vector<TaskBase*> FilterByPriority(TaskPriority priority) noexcept;
  std::vector<TaskBase*> FilterByTag(const std::string& tag) noexcept;

  template <class Predicate>
  std::vector<TaskBase*> Filter(Predicate predicate) noexcept;

  std::size_t Size() const noexcept;
  bool Empty() const noexcept;
  void Clear() noexcept;

 private:
  TaskList tasks_;
  std::unordered_map<int32_t, TaskBase*> task_index_;
};
