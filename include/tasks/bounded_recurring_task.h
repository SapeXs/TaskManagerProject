#pragma once

#include <chrono>
#include <cstdint>
#include <string>

#include "tasks/recurring_task.h"

class BoundedRecurringTask : public RecurringTask {
 public:
  BoundedRecurringTask(int32_t id, std::string title, std::string description,
                       TaskPriority priority, TagContainer tags,
                       std::chrono::system_clock::time_point deadline,
                       int64_t repeat_interval_seconds, int32_t repeats_left);

  std::string_view GetTypeName() const override;

  int32_t GetRepeatsLeft() const noexcept;
  void SetRepeatsLeft(int32_t repeats);
  void ReduceRepeatsLeft();
  bool CanRepeat() const noexcept;

  void CompleteOccurrence() noexcept;
  void ResetToNextOccurrence() noexcept override;

  std::vector<std::string> GetStorageFields() const override;

 private:
  int32_t repeats_left_;
};
