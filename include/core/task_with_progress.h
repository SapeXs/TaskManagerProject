#pragma once

#include <concepts>
#include <cstdint>
#include <string>

#include "progress/progress_concepts.h"
#include "task_base.h"
#include "task_priority.h"
#include "task_state.h"

template <ProgressLike ProgressType> class TaskWithProgress : public TaskBase {
public:
  using TagContainer = TaskBase::TagContainer;

  TaskWithProgress(int32_t id, std::string title, std::string description,
                   TaskPriority priority, TagContainer tags);

  TaskState GetState() const override;
  std::string GetProgressString() const override;

protected:
  ProgressType& GetProgress();

private:
  ProgressType progress_;
};
