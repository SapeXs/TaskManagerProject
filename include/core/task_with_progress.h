#pragma once

#include <concepts>
#include <cstdint>
#include <string>

#include "task_base.h"
#include "task_priority.h"
#include "task_state.h"

template <class ProgressType>
concept ProgressLike = requires(const ProgressType &progress) {
  { progress.ToString() } -> std::same_as<std::string>;
  { progress.GetState() } -> std::same_as<TaskState>;
};

template <ProgressLike ProgressType> class TaskWithProgress : public TaskBase {
public:
  using TagContainer = TaskBase::TagContainer;

  TaskWithProgress(int32_t id, std::string title, std::string description,
                   TaskPriority priority, TagContainer tags,
                   int64_t seconds_left);

  TaskState GetState() const;
  std::string GetProgressString() const;

protected:
  ProgressType& GetProgress();

private:
  ProgressType progress_;
};
