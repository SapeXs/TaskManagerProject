#include "core/task_name.h"
#include "tasks/savings_task.h"

SavingsTask::SavingsTask(int32_t id, std::string title, std::string description,
                         TaskPriority priority, TagContainer tags,
                         int64_t current_value, int64_t target_value)
    : TaskWithProgress<PercentageProgress>(id, std::move(title), std::move(description),
                                           priority, std::move(tags),
                                           current_value, target_value) {}

std::string_view SavingsTask::GetTypeName() const {
  return task_names::kNameSavingTask;
}
int64_t SavingsTask::GetCurrentValue() const noexcept {
    return GetProgress().GetCurrentValue();
}

int64_t SavingsTask::GetTargetValue() const noexcept {
    return GetProgress().GetTargetValue();
}

void SavingsTask::AddValue(int64_t value) noexcept {
    GetProgress().Add(value);
}

std::vector<std::string> SavingsTask::GetStorageFields() const {
  return {std::to_string(GetCurrentValue()), std::to_string(GetTargetValue())};
}
