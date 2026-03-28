#pragma once

#include <cstdint>
#include <string>

#include "../tasks/recurring_task.h"

class BoundedRecurringTask : public RecurringTask {
 public:
  BoundedRecurringTask(int32_t id, std::string title, std::string description,
                       TaskPriority priority, TagContainer tags,
                       int64_t seconds_left, int64_t repeat_interval_seconds,
                       int32_t repeats_left);

  std::string GetTypeName() const override;

  int32_t GetRepeatsLeft() const noexcept;
  bool CanRepeat() const noexcept;
  void CompleteOccurrence() noexcept;

 private:
  int32_t repeats_left_;
};
