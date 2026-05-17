#include "tasks/bounded_recurring_task.h"
#include "core/task_name.h"

BoundedRecurringTask::BoundedRecurringTask(int32_t id, std::string title, std::string description,
                                           TaskPriority priority, TagContainer tags,
                                           std::chrono::system_clock::time_point deadline,
                                           int64_t repeat_interval_seconds,
                                           int32_t repeats_left)
    : RecurringTask(id, std::move(title), std::move(description), priority, std::move(tags),
                    deadline, repeat_interval_seconds),
      repeats_left_(repeats_left) {}

std::string_view BoundedRecurringTask::GetTypeName() const {
  return task_names::kNameBoundedRecurringTask;
}

int32_t BoundedRecurringTask::GetRepeatsLeft() const noexcept {
  return repeats_left_;
}

void BoundedRecurringTask::SetRepeatsLeft(int32_t repeats) {
  repeats_left_ = repeats;
}

void BoundedRecurringTask::ReduceRepeatsLeft() {
  --repeats_left_;
}

bool BoundedRecurringTask::CanRepeat() const noexcept {
  return repeats_left_ > 0;
}

void BoundedRecurringTask::CompleteOccurrence() noexcept {
  repeats_left_ = 0;
}

void BoundedRecurringTask::ResetToNextOccurrence() noexcept {
  if (CanRepeat()) {
    ReduceRepeatsLeft();
    RecurringTask::ResetToNextOccurrence();
  }
}

std::vector<std::string> BoundedRecurringTask::GetStorageFields() const {
  return {std::to_string(std::chrono::system_clock::to_time_t(GetDeadline())),
          std::to_string(GetRepeatIntervalSeconds()),
          std::to_string(GetRepeatsLeft())};
}
