#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "core/task_priority.h"
#include "core/task_with_progress.h"
#include "progress/time_left_progress.h"

class ReminderTask : public TaskWithProgress<TimeLeftProgress> {
 public:
  using TagContainer = TaskBase::TagContainer;

  ReminderTask(int32_t id, std::string title, std::string description, TaskPriority priority,
               TagContainer tags, std::chrono::system_clock::time_point deadline);

  std::string_view GetTypeName() const;

  std::chrono::system_clock::time_point GetDeadline() const noexcept;
  void SetDeadline(std::chrono::system_clock::time_point deadline) noexcept;

  int64_t GetSecondsLeft() const noexcept;
  void SetSecondsLeft(int64_t seconds_left) noexcept;

  std::vector<std::string> GetStorageFields() const override;
};
