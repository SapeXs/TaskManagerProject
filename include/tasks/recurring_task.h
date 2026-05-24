#pragma once

#include <chrono>
#include <cstdint>
#include <string>

#include "core/task_priority.h"
#include "core/task_with_progress.h"
#include "progress/time_left_progress.h"

class RecurringTask : public TaskWithProgress<TimeLeftProgress> {
 public:
  using TagContainer = TaskBase::TagContainer;

  RecurringTask(int32_t id, std::string title, std::string description, TaskPriority priority,
                TagContainer tags, std::chrono::system_clock::time_point deadline,
                int64_t repeat_interval_seconds);

  std::string_view GetTypeName() const override;

  std::chrono::system_clock::time_point GetDeadline() const noexcept;
  void SetDeadline(std::chrono::system_clock::time_point deadline) noexcept;

  int64_t GetSecondsLeft() const noexcept;
  int64_t GetRepeatIntervalSeconds() const noexcept;
  void SetSecondsLeft(int64_t seconds_left) noexcept;
  void SetRepeatIntervalSeconds(int64_t repeat_interval_seconds) noexcept;

  virtual void ResetToNextOccurrence() noexcept;

  std::vector<std::string> GetStorageFields() const override;

 private:
  int64_t repeat_interval_seconds_;
};
