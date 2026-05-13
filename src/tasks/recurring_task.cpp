#include "tasks/recurring_task.h"
#include "core/task_name.h"

RecurringTask::RecurringTask(int32_t id, std::string title,
                             std::string description, TaskPriority priority,
                             TagContainer tags, int64_t seconds_left,
                             int64_t repeat_interval_seconds)
    : TaskWithProgress<TimeLeftProgress>(id, std::move(title),
                                         std::move(description), priority,
                                         std::move(tags), seconds_left),
      repeat_interval_seconds_(repeat_interval_seconds) {}

std::string_view RecurringTask::GetTypeName() const { return kNameRecurringTask; }

int64_t RecurringTask::GetRepeatIntervalSeconds() const noexcept {
  return repeat_interval_seconds_;
}

void RecurringTask::SetSecondsLeft(int64_t seconds_left) noexcept {
  GetProgress().SetSecondsLeft(seconds_left);
};

void RecurringTask::ResetToNextOccurrence() noexcept {
  GetProgress().SetSecondsLeft(repeat_interval_seconds_);
}
