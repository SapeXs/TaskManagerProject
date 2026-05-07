#include "tasks/reminder_task.h"
#include "core/task_name.h"

ReminderTask::ReminderTask(int32_t id, std::string title,
                           std::string description, TaskPriority priority,
                           TagContainer tags, int64_t seconds_left)
    : TaskWithProgress<TimeLeftProgress>(id, std::move(title),
                                         std::move(description), priority,
                                         std::move(tags), seconds_left) {}

std::string ReminderTask::GetTypeName() const { return kNameReminderTask; }

void ReminderTask::SetSecondsLeft(int64_t seconds_left) noexcept {
  GetProgress().SetSecondsLeft(seconds_left);
}
