#pragma once

#include <cstdint>
#include <string>

#include "tasks/recurring_task.h"

class BoundedRecurringTask : public RecurringTask {
 public:
  BoundedRecurringTask(int32_t id, std::string title, std::string description,
                       TaskPriority priority, TagContainer tags, int64_t seconds_left,
                       int64_t repeat_interval_seconds, int32_t repeats_left);

  std::string_view GetTypeName() const;

  int32_t GetRepeatsLeft() const noexcept;
  void SetRepeatsLeft(int32_t repeats);
  void ReduceRepeatsLeft();
  bool CanRepeat() const noexcept;
  void CompleteOccurrence() noexcept;

  std::vector<std::string> GetStorageFields() const override;

 private:
  int32_t repeats_left_;
};
