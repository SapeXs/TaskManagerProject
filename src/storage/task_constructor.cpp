#include "storage/task_constructor.h"

#include <utility>

#include "core/task_name.h"
#include "storage/storage_codec.h"
#include "tasks/tasks_lib.h"

std::unique_ptr<TaskBase> CreateTaskFromStorageFields(const std::vector<std::string>& fields) {
  if (fields.size() < 6) {
    return nullptr;
  }

  const std::string& type = fields[0];
  int32_t id = std::stoi(fields[1]);
  std::string title = fields[2];
  std::string description = fields[3];
  TaskPriority priority = ParseTaskPriority(fields[4]);
  TaskBase::TagContainer tags = ParseStorageTags(fields[5]);

  if (type == kNameReminderTask && fields.size() == 7) {
    return std::make_unique<ReminderTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::stoll(fields[6]));
  }

  if (type == kNameRecurringTask && fields.size() == 8) {
    return std::make_unique<RecurringTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::stoll(fields[6]), std::stoll(fields[7]));
  }

  if (type == kNameBoundedRecurringTask && fields.size() == 9) {
    return std::make_unique<BoundedRecurringTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::stoll(fields[6]), std::stoll(fields[7]), std::stoi(fields[8]));
  }

  if (type == kNameSavingTask && fields.size() == 8) {
    return std::make_unique<SavingsTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::stoll(fields[6]), std::stoll(fields[7]));
  }

  if (type == kNameSteppedDeadlineTask && fields.size() >= 8) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(fields[6]));
    std::vector<std::string> steps(fields.begin() + 7, fields.end());

    return std::make_unique<SteppedDeadlineTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::move(steps), current_step);
  }

  if (type == kNameFinalDeadlineTask && fields.size() >= 9) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(fields[6]));
    std::vector<std::string> steps(fields.begin() + 7, fields.end() - 1);
    std::string final_deadline_text = fields.back();

    return std::make_unique<FinalDeadlineTask>(id, std::move(title), std::move(description), priority, std::move(tags), std::move(steps), std::move(final_deadline_text), current_step);
  }

  return nullptr;
}
