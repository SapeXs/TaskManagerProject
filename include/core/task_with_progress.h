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
                   ProgressType progress)
      : TaskBase(id, std::move(title), std::move(description), priority,
                 std::move(tags)),
        progress_(std::move(progress)) {}

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
  const ProgressType &GetProgress() const noexcept { return progress_; }

  void SetProgress(ProgressType progress) { progress_ = std::move(progress); }

private:
  ProgressType progress_;
};
