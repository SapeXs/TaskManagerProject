#pragma once

#include <cstdint>
#include <string>

#include "../core/task_priority.h"
#include "../core/task_with_progress.h"
#include "../progress/time_left_progress.h"

class RecurringTask : public TaskWithProgress<TimeLeftProgress> {
 public:
  using TagContainer = TaskBase::TagContainer;

  RecurringTask(int32_t id, std::string title, std::string description,
                TaskPriority priority, TagContainer tags,
                int64_t seconds_left, int64_t repeat_interval_seconds);

  std::string GetTypeName() const override;

  int64_t GetRepeatIntervalSeconds() const noexcept;
  void SetSecondsLeft(int64_t seconds_left) noexcept;
  void ResetToNextOccurrence() noexcept;

 private:
  int64_t repeat_interval_seconds_;
};