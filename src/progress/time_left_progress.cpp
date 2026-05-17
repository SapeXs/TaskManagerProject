#include "progress/time_left_progress.h"

#include "core/date_time_utils.h"

#include <string_view>

namespace {
constexpr int64_t kSecondsInMinute = 60;
constexpr int64_t kSecondsInHour = 3600;
constexpr int64_t kSecondsInDay = 86400;

constexpr std::string_view kPrefix = "deadline: ";
constexpr std::string_view kSeparator = ", ";
constexpr std::string_view kOverdue = "overdue";
constexpr std::string_view kLeft = "left: ";
constexpr std::string_view kDaysSuffix = "d ";
constexpr std::string_view kHoursSuffix = "h ";
constexpr std::string_view kMinutesSuffix = "m ";
constexpr std::string_view kSecondsSuffix = "s";
}  // namespace

TimeLeftProgress::TimeLeftProgress(int64_t seconds_left) {
  SetSecondsLeft(seconds_left);
}

TimeLeftProgress::TimeLeftProgress(std::chrono::system_clock::time_point deadline)
    : deadline_(deadline) {}

int64_t TimeLeftProgress::GetSecondsLeft() const noexcept {
  if (IsExpired()) {
    return 0;
  }
  return SecondsUntil(deadline_).count();
}

std::chrono::system_clock::time_point TimeLeftProgress::GetDeadline() const noexcept {
  return deadline_;
}

void TimeLeftProgress::SetSecondsLeft(int64_t seconds_left) noexcept {
  deadline_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds_left);
}

void TimeLeftProgress::SetDeadline(std::chrono::system_clock::time_point deadline) noexcept {
  deadline_ = deadline;
}

bool TimeLeftProgress::IsExpired() const noexcept {
  return ::IsExpired(deadline_);
}

TaskState TimeLeftProgress::GetState() const {
  if (IsExpired()) {
    return TaskState::kOverdue;
  }
  return TaskState::kScheduled;
}

std::string TimeLeftProgress::ToString() const {
  std::string result(kPrefix);
  result += FormatDateTime(deadline_);
  result += kSeparator;

  if (IsExpired()) {
    result += kOverdue;
    return result;
  }

  int64_t secs = GetSecondsLeft();
  int64_t days = secs / kSecondsInDay;
  int64_t hours = (secs % kSecondsInDay) / kSecondsInHour;
  int64_t mins = (secs % kSecondsInHour) / kSecondsInMinute;

  result += kLeft;

  std::string time_str;
  if (days > 0) {
    time_str += std::to_string(days) + std::string(kDaysSuffix);
  }
  if (hours > 0) {
    time_str += std::to_string(hours) + std::string(kHoursSuffix);
  }
  if (mins > 0 && days == 0) {
    time_str += std::to_string(mins) + std::string(kMinutesSuffix);
  }
  if (days == 0 && hours == 0 && mins == 0) {
    time_str += std::to_string(secs) + std::string(kSecondsSuffix);
  }

  while (!time_str.empty() && time_str.back() == ' ') {
    time_str.pop_back();
  }

  result += time_str;
  return result;
}