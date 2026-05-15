#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <utility>

#include "progress/progress_concepts.h"
#include "task_base.h"
#include "task_priority.h"
#include "task_state.h"

template <ProgressLike ProgressType> class TaskWithProgress : public TaskBase {
public:
  using TagContainer = TaskBase::TagContainer;

  template <class... ProgressArgs>
  TaskWithProgress(int32_t id, std::string title, std::string description,
                   TaskPriority priority, TagContainer tags,
                   ProgressArgs &&...progress_args);

  TaskState GetState() const override;
  std::string GetProgressString() const override;

protected:
  ProgressType& GetProgress();

  const ProgressType& GetProgress() const;

 private:
  ProgressType progress_;
};

#include "core/task_with_progress.tpp"
