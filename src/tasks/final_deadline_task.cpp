#include "tasks/final_deadline_task.h"
#include "core/task_name.h"

FinalDeadlineTask::FinalDeadlineTask(int32_t id, std::string title, 
    std::string description, TaskPriority priority,
    TagContainer tags, std::vector<std::string> step_texts,
    std::string final_deadline_text, std::size_t current_step)
    : SteppedDeadlineTask(
          id,
          std::move(title),
          std::move(description),
          priority,
          std::move(tags),
          std::move(step_texts),
          current_step),
      final_deadline_text_(std::move(final_deadline_text)) {}

std::string_view FinalDeadlineTask::GetTypeName() const {
    return kNameFinalDeadlineTask;
}

const std::string& FinalDeadlineTask::GetFinalDeadlineText() const noexcept {
  return final_deadline_text_;
}

void FinalDeadlineTask::MarkFinalDeadlinePassed() {
  SetOverdue();
}

std::vector<std::string> FinalDeadlineTask::GetStorageFields() const {
  std::vector<std::string> fields = SteppedDeadlineTask::GetStorageFields();
  fields.push_back(final_deadline_text_);
  return fields;
}
