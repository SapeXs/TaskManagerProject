#include "tasks/recurring_task.h"

#include "core/task_name.h"

RecurringTask::RecurringTask(int32_t id, std::string title, std::string description,
                             TaskPriority priority, TagContainer tags,
                             std::chrono::system_clock::time_point deadline,
                             int64_t repeat_interval_seconds)
    : TaskWithProgress<TimeLeftProgress>(id, std::move(title), std::move(description), priority,
                                         std::move(tags), deadline),
      repeat_interval_seconds_(repeat_interval_seconds) {}

std::string_view RecurringTask::GetTypeName() const {
  return task_names::kNameRecurringTask;
}

std::chrono::system_clock::time_point RecurringTask::GetDeadline() const noexcept {
  return GetProgress().GetDeadline();
}

void RecurringTask::SetDeadline(std::chrono::system_clock::time_point deadline) noexcept {
  GetProgress().SetDeadline(deadline);
}

int64_t RecurringTask::GetSecondsLeft() const noexcept {
  return GetProgress().GetSecondsLeft();
}

int64_t RecurringTask::GetRepeatIntervalSeconds() const noexcept {
  return repeat_interval_seconds_;
}

void RecurringTask::SetSecondsLeft(int64_t seconds_left) noexcept {
  GetProgress().SetSecondsLeft(seconds_left);
}

void RecurringTask::ResetToNextOccurrence() noexcept {
  GetProgress().SetDeadline(std::chrono::system_clock::now() +
                            std::chrono::seconds(repeat_interval_seconds_));
}

void RecurringTask::SetRepeatIntervalSeconds(int64_t repeat_interval_seconds) noexcept {
  repeat_interval_seconds_ = repeat_interval_seconds;
}

std::vector<std::string> RecurringTask::GetStorageFields() const {
  return {std::to_string(std::chrono::system_clock::to_time_t(GetDeadline())),
          std::to_string(GetRepeatIntervalSeconds())};
}
