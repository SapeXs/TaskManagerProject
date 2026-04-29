#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

class TimeLeftProgress {
 public:
  explicit TimeLeftProgress(int64_t seconds_left)
      : seconds_left_(seconds_left) {
  }

  int64_t GetSecondsLeft() const noexcept {
    return seconds_left_;
  }

  void SetSecondsLeft(int64_t seconds_left) noexcept {
    seconds_left_ = seconds_left;
  }

  std::string ToString() const {
    if (seconds_left_ < 0) {
      return "event passed";
    }

    if (seconds_left_ < 60) {
      return std::to_string(seconds_left_) + " sec left";
    }

    int64_t minutes = seconds_left_ / 60;
    if (minutes < 60) {
      return std::to_string(minutes) + " min left";
    }

    int64_t hours = minutes / 60;
    if (hours < 24) {
      return std::to_string(hours) + " hours left";
    }

    int64_t days = hours / 24;
    return std::to_string(days) + " days left";
  }

  TaskState GetState() const noexcept {
    if (seconds_left_ < 0) {
      return TaskState::kOverdue;
    }
    return TaskState::kScheduled;
  }

 private:
  int64_t seconds_left_;
};
