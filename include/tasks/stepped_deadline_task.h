#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/task_priority.h"
#include "core/task_with_progress.h"
#include "progress/text_progress.h"
#include "core/task_state.h"

class SteppedDeadlineTask : public TaskWithProgress<TextProgress> {
 public:
  using TagContainer = TaskBase::TagContainer;

  SteppedDeadlineTask(int32_t id, std::string title, std::string description,
                      TaskPriority priority, TagContainer tags,
                      std::vector<std::string> step_texts,
                      std::size_t current_step = 0);

  std::string GetTypeName() const override;

  const std::vector<std::string>& GetStepTexts() const noexcept;
  std::size_t GetCurrentStep() const noexcept;

  void AdvanceStep();
  void SetOverdue();

 protected:
  void UpdateProgressText(TaskState state);

 private:
  std::vector<std::string> step_texts_;
  std::size_t current_step_;
};
