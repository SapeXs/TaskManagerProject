#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

class PercentageProgress {
 public:
  PercentageProgress(int64_t current_value, int64_t target_value)
      : current_value_(current_value), target_value_(target_value) {
  }

  int64_t GetCurrentValue() const noexcept {
    return current_value_;
  }

  int64_t GetTargetValue() const noexcept {
    return target_value_;
  }

  void Add(int64_t value) noexcept {
    current_value_ += value;
    if (current_value_ < 0) {
      current_value_ = 0;
    }
  }

  int32_t GetPercent() const noexcept {
    if (target_value_ <= 0 || current_value_ <= 0) {
      return 0;
    }
    if (current_value_ >= target_value_) {
      return 100;
    }
    return static_cast<int32_t>(current_value_ * 100 / target_value_);
  }

  std::string ToString() const {
    return std::to_string(GetPercent()) + "%";
  }

  TaskState GetState() const noexcept {
    int32_t percent = GetPercent();
    if (percent == 0) {
      return TaskState::kNotStarted;
    }
    if (percent >= 100) {
      return TaskState::kDone;
    }
    return TaskState::kInProgress;
  }

 private:
  int64_t current_value_;
  int64_t target_value_;
};
