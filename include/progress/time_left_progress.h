#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

namespace {
constexpr int64_t kSecInMin = 60;
constexpr int64_t kMinInHour = 60;
constexpr int64_t kHourInDay = 24;
}  // namespace

class TimeLeftProgress {
 public:
  explicit TimeLeftProgress(int64_t seconds_left) : seconds_left_(seconds_left) {}

  int64_t GetSecondsLeft() const noexcept { return seconds_left_; }
  void SetSecondsLeft(int64_t seconds_left) noexcept { seconds_left_ = seconds_left; }

  std::string ToString() const {
    if (seconds_left_ < 0) return "event passed";
    if (seconds_left_ < kSecInMin) return std::to_string(seconds_left_) + " sec left";

    int64_t minutes = seconds_left_ / kSecInMin;
    if (minutes < kMinInHour) return std::to_string(minutes) + " min left";

    int64_t hours = minutes / kMinInHour;
    if (hours < kHourInDay) return std::to_string(hours) + " hours left";

    int64_t days = hours / kHourInDay;
    return std::to_string(days) + " days left";
  }

  TaskState GetState() const noexcept {
    return seconds_left_ < 0 ? TaskState::kOverdue : TaskState::kScheduled;
  }

 private:
  int64_t seconds_left_;
};
