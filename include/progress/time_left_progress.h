#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

namespace {
constexpr int64_t kSecondsInMinute = 60;
constexpr int64_t kMinutesInHour = 60;
constexpr int64_t kHoursInDay = 24;
constexpr int64_t kZero = 0;

constexpr std::string kEventPassed = "event passed";
constexpr std::string kSecondsLeft = " sec left";
constexpr std::string kMinutesLeft = " min left";
constexpr std::string kHoursLeft = " hours left";
constexpr std::string kDaysLeft = " days left";
} // namespace

class TimeLeftProgress {
public:
  explicit TimeLeftProgress(int64_t seconds_left)
      : seconds_left_(seconds_left) {}

  int64_t GetSecondsLeft() const noexcept { return seconds_left_; }

  int64_t GetSecondsLeft() const noexcept { return seconds_left_; }
  void SetSecondsLeft(int64_t seconds_left) noexcept { seconds_left_ = seconds_left; }

  std::string ToString() const {
    if (seconds_left_ < kZero) {
      return kEventPassed;
    }

    if (seconds_left_ < kSecondsInMinute) {
      return std::to_string(seconds_left_) + kSecondsLeft;
    }

    int64_t minutes = seconds_left_ / kSecondsInMinute;
    if (minutes < kMinutesInHour) {
      return std::to_string(minutes) + kMinutesLeft;
    }

    int64_t hours = minutes / kMinutesInHour;
    if (hours < kHoursInDay) {
      return std::to_string(hours) + kHoursLeft;
    }

    int64_t days = hours / kHoursInDay;
    return std::to_string(days) + kDaysLeft;
  }

  TaskState GetState() const noexcept {
    if (seconds_left_ < kZero) {
      return TaskState::kOverdue;
    }
    return TaskState::kScheduled;
  }

private:
  int64_t seconds_left_;
};
