#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

namespace {
constexpr int32_t kMinPercent = 0;
constexpr int32_t kMaxPercent = 100;
constexpr std::string kSignPercent = "%";
}  // namespace

class PercentageProgress {
 public:
  PercentageProgress(int64_t current_value, int64_t target_value)
      : current_value_(current_value), target_value_(target_value) {}

  int64_t GetCurrentValue() const noexcept {
    return current_value_;
  }

  int64_t GetTargetValue() const noexcept {
    return target_value_;
  }

  void Add(int64_t value) noexcept {
    current_value_ += value;
    if (current_value_ < kMinPercent) {
      current_value_ = kMinPercent;
    }
  }

  int32_t GetPercent() const noexcept {
    if (target_value_ <= kMinPercent || current_value_ <= kMinPercent) {
      return kMinPercent;
    }
    if (current_value_ >= target_value_) {
      return kMaxPercent;
    }
    return static_cast<int32_t>(current_value_ * kMaxPercent / target_value_);
  }

  std::string ToString() const {
    return std::to_string(GetPercent()) + kSignPercent;
  }

  TaskState GetState() const noexcept {
    int32_t percent = GetPercent();
    if (percent == kMinPercent) {
      return TaskState::kNotStarted;
    }
    if (percent >= kMaxPercent) {
      return TaskState::kDone;
    }
    return TaskState::kInProgress;
  }

 private:
  int64_t current_value_;
  int64_t target_value_;
};
