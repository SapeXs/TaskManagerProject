#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <utility>

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
                   int64_t seconds_left)
      : TaskBase(id, std::move(title), std::move(description), priority,
                 std::move(tags)),
        progress_(seconds_left) {}

  /*~TaskWithProgress() override = default;
  TaskWithProgress(const TaskWithProgress&) = default;
  TaskWithProgress(TaskWithProgress&&) noexcept = default;
  TaskWithProgress& operator=(const TaskWithProgress&) = default;
  TaskWithProgress& operator=(TaskWithProgress&&) noexcept = default;*/

  TaskState GetState() const override { return progress_.GetState(); }

  std::string GetProgressString() const override {
    return progress_.ToString();
  }

protected:
  ProgressType& GetProgress() { return progress_; }

private:
  ProgressType progress_;
};
