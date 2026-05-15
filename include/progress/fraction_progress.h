#pragma once

#include <cstdint>
#include <string>

#include "core/task_state.h"

namespace {
constexpr int32_t kMin = 0;
constexpr std::string kSignSlash = "/";
} // namespace

class FractionProgress {
public:
  FractionProgress(int32_t done, int32_t total) : done_(done), total_(total) {}

  int32_t GetDone() const noexcept { return done_; }

  int32_t GetTotal() const noexcept { return total_; }

  void IncrementDone(int32_t value = 1) noexcept {
    done_ += value;
    if (done_ < kMin) {
      done_ = kMin;
    }
    if (done_ > total_) {
      done_ = total_;
    }
  }

  std::string ToString() const {
    return std::to_string(done_) + kSignSlash + std::to_string(total_);
  }

  TaskState GetState() const noexcept {
    if (total_ == kMin || done_ == kMin) {
      return TaskState::kNotStarted;
    }
    if (done_ >= total_) {
      return TaskState::kDone;
    }
    return TaskState::kInProgress;
  }

private:
  int32_t done_;
  int32_t total_;
};
