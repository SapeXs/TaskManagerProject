#include "tasks/reminder_task.h"

#include "core/task_name.h"

ReminderTask::ReminderTask(int32_t id, std::string title, std::string description,
                           TaskPriority priority, TagContainer tags, int64_t seconds_left)
    : TaskWithProgress<TimeLeftProgress>(id, std::move(title), std::move(description), priority,
                                         std::move(tags), seconds_left) {}

std::string_view ReminderTask::GetTypeName() const {
  return task_names::kNameReminderTask;
}

void ReminderTask::SetSecondsLeft(int64_t seconds_left) noexcept {
  GetProgress().SetSecondsLeft(seconds_left);
}

int64_t ReminderTask::GetSecondsLeft() const noexcept {
  return GetProgress().GetSecondsLeft();
}

std::vector<std::string> ReminderTask::GetStorageFields() const {
  return {std::to_string(GetSecondsLeft())};
}
