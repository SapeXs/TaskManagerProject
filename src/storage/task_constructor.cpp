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

  if (type == task_names::kNameReminderTask && fields.size() == 7) {
    return std::make_unique<ReminderTask>(id, std::move(title), std::move(description), priority,
                                          std::move(tags), std::stoll(fields[6]));
  }

  if (type == task_names::kNameRecurringTask && fields.size() == 8) {
    return std::make_unique<RecurringTask>(id, std::move(title), std::move(description), priority,
                                           std::move(tags), std::stoll(fields[6]),
                                           std::stoll(fields[7]));
  }

  if (type == task_names::kNameBoundedRecurringTask && fields.size() == 9) {
    return std::make_unique<BoundedRecurringTask>(id, std::move(title), std::move(description),
                                                  priority, std::move(tags), std::stoll(fields[6]),
                                                  std::stoll(fields[7]), std::stoi(fields[8]));
  }

  if (type == task_names::kNameSavingTask && fields.size() == 8) {
    return std::make_unique<SavingsTask>(id, std::move(title), std::move(description), priority,
                                         std::move(tags), std::stoll(fields[6]),
                                         std::stoll(fields[7]));
  }

  if (type == task_names::kNameSteppedDeadlineTask && fields.size() >= 8) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(fields[6]));
    std::vector<std::string> steps(fields.begin() + 7, fields.end());

    return std::make_unique<SteppedDeadlineTask>(id, std::move(title), std::move(description),
                                                 priority, std::move(tags), std::move(steps),
                                                 current_step);
  }

  if (type == task_names::kNameFinalDeadlineTask && fields.size() >= 9) {
    std::size_t current_step = static_cast<std::size_t>(std::stoull(fields[6]));
    std::vector<std::string> steps(fields.begin() + 7, fields.end() - 1);
    std::string final_deadline_text = fields.back();

    return std::make_unique<FinalDeadlineTask>(id, std::move(title), std::move(description),
                                               priority, std::move(tags), std::move(steps),
                                               std::move(final_deadline_text), current_step);
  }

  return nullptr;
}

std::unique_ptr<TaskBase> CreateTaskFromAddOptions(int32_t id, AddTaskOptions options,
                                                   std::string& error) {
  switch (options.type) {
    case AddTaskType::kReminder:
      return std::make_unique<ReminderTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), options.seconds_left.value_or(0));

    case AddTaskType::kRecurring:
      if (!options.seconds_left.has_value() || !options.repeat_interval_seconds.has_value()) {
        error = "Usage: add recurring <title> --seconds <value> --interval <value>";
        return nullptr;
      }

      return std::make_unique<RecurringTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), *options.seconds_left, *options.repeat_interval_seconds);

    case AddTaskType::kBoundedRecurring:
      if (!options.seconds_left.has_value() || !options.repeat_interval_seconds.has_value() ||
          !options.repeats_left.has_value()) {
        error = "Usage: add bounded <title> --seconds <value> --interval <value> --repeats <value>";
        return nullptr;
      }

      return std::make_unique<BoundedRecurringTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), *options.seconds_left, *options.repeat_interval_seconds,
          *options.repeats_left);

    case AddTaskType::kSavings:
      if (!options.current_value.has_value() || !options.target_value.has_value()) {
        error = "Usage: add savings <title> --current <value> --target <value>";
        return nullptr;
      }

      return std::make_unique<SavingsTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), *options.current_value, *options.target_value);

    case AddTaskType::kSteppedDeadline:
      if (options.step_texts.empty()) {
        error = "Usage: add stepped <title> --step <text> [--step <text> ...]";
        return nullptr;
      }

      return std::make_unique<SteppedDeadlineTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), std::move(options.step_texts), options.current_step.value_or(0));

    case AddTaskType::kFinalDeadline:
      if (options.step_texts.empty() || !options.final_deadline_text.has_value()) {
        error = "Usage: add final <title> --step <text> [--step <text> ...] --final <text>";
        return nullptr;
      }

      return std::make_unique<FinalDeadlineTask>(
          id, std::move(options.title), std::move(options.description), options.priority,
          std::move(options.tags), std::move(options.step_texts),
          std::move(*options.final_deadline_text), options.current_step.value_or(0));
  }

  error = "Unknown task type";
  return nullptr;
}
