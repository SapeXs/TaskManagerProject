#pragma once

#include <chrono>
#include <cstdint>
#include <string>

#include "core/task_state.h"

class TimeLeftProgress {
 public:
  explicit TimeLeftProgress(int64_t seconds_left);
  explicit TimeLeftProgress(std::chrono::system_clock::time_point deadline);

  int64_t GetSecondsLeft() const noexcept;
  std::chrono::system_clock::time_point GetDeadline() const noexcept;

  void SetSecondsLeft(int64_t seconds_left) noexcept;
  void SetDeadline(std::chrono::system_clock::time_point deadline) noexcept;

  bool IsExpired() const noexcept;

  TaskState GetState() const;
  std::string ToString() const;

 private:
  std::chrono::system_clock::time_point deadline_;
};