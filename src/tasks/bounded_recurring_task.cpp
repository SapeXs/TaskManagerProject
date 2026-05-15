#include "tasks/bounded_recurring_task.h"
#include "core/task_name.h"

BoundedRecurringTask::BoundedRecurringTask(
    int32_t id, std::string title, std::string description,
    TaskPriority priority, TagContainer tags, int64_t seconds_left,
    int64_t repeat_interval_seconds, int32_t repeats_left)
    : RecurringTask(id, std::move(title), std::move(description), priority,
                    std::move(tags), seconds_left, repeat_interval_seconds),
      repeats_left_(repeats_left) {}

std::string_view BoundedRecurringTask::GetTypeName() const {
  return kNameBoundedRecurringTask;
}

int32_t BoundedRecurringTask::GetRepeatsLeft() const noexcept {
  return repeats_left_;
}

void BoundedRecurringTask::SetRepeatsLeft(int32_t repeats) {
  repeats_left_ = repeats;
}

void BoundedRecurringTask::ReduceRepeatsLeft() { --repeats_left_; }

bool BoundedRecurringTask::CanRepeat() const noexcept {
  return repeats_left_ > 0;
}

void BoundedRecurringTask::CompleteOccurrence() noexcept { repeats_left_ = 0; }

std::vector<std::string> BoundedRecurringTask::GetStorageFields() const {
  return {std::to_string(GetSecondsLeft()), std::to_string(GetRepeatIntervalSeconds()),
          std::to_string(GetRepeatsLeft())};
}
