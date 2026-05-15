#include "tasks/stepped_deadline_task.h"
#include "core/task_name.h"

SteppedDeadlineTask::SteppedDeadlineTask(int32_t id, std::string title, std::string description,
                                         TaskPriority priority, TagContainer tags,
                                         std::vector<std::string> step_texts, size_t current_step)
    : TaskWithProgress<TextProgress>(
          id,
          std::move(title),
          std::move(description),
          priority,
          std::move(tags),
          step_texts.empty() ? "" : step_texts[current_step],
          TaskState::kInProgress), 
          step_texts_(std::move(step_texts)), current_step_(current_step) {}

std::string_view SteppedDeadlineTask::GetTypeName() const {
    return task_names::kNameSteppedDeadlineTask;
}

const std::vector<std::string>& SteppedDeadlineTask::GetStepTexts() const noexcept {
  return step_texts_;
}

size_t SteppedDeadlineTask::GetCurrentStep() const noexcept {
  return current_step_;
}

void SteppedDeadlineTask::AdvanceStep() {
  if (current_step_ + 1 < step_texts_.size()) {
    ++current_step_;
    UpdateProgressText(TaskState::kInProgress);
    return;
  }

  UpdateProgressText(TaskState::kDone);
}

void SteppedDeadlineTask::SetOverdue() {
  UpdateProgressText(TaskState::kOverdue);
}

void SteppedDeadlineTask::UpdateProgressText(
    TaskState state) {
  std::string text;

  if (!step_texts_.empty() &&
      current_step_ < step_texts_.size()) {
    text = step_texts_[current_step_];
  }

  GetProgress().Set(std::move(text), state);
}

std::vector<std::string> SteppedDeadlineTask::GetStorageFields() const {
  std::vector<std::string> fields;
  fields.push_back(std::to_string(GetCurrentStep()));

  for (const std::string& step : GetStepTexts()) {
    fields.push_back(step);
  }

  return fields;
}
