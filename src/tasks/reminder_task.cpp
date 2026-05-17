#include "tasks/reminder_task.h"

#include "core/task_name.h"

ReminderTask::ReminderTask(int32_t id, std::string title, std::string description,
                           TaskPriority priority, TagContainer tags,
                           std::chrono::system_clock::time_point deadline)
    : TaskWithProgress<TimeLeftProgress>(id, std::move(title), std::move(description), priority,
                                         std::move(tags), deadline) {}

std::string_view ReminderTask::GetTypeName() const {
  return task_names::kNameReminderTask;
}

std::chrono::system_clock::time_point ReminderTask::GetDeadline() const noexcept {
  return GetProgress().GetDeadline();
}

void ReminderTask::SetDeadline(std::chrono::system_clock::time_point deadline) noexcept {
  GetProgress().SetDeadline(deadline);
}

void ReminderTask::SetSecondsLeft(int64_t seconds_left) noexcept {
  GetProgress().SetSecondsLeft(seconds_left);
}

int64_t ReminderTask::GetSecondsLeft() const noexcept {
  return GetProgress().GetSecondsLeft();
}

std::vector<std::string> ReminderTask::GetStorageFields() const {
  return {std::to_string(std::chrono::system_clock::to_time_t(GetDeadline()))};
}
