#include "core/task_with_progress.h"

using TagContainer = TaskBase::TagContainer;

template <ProgressLike ProgressType>
template <class... ProgressArgs>
TaskWithProgress<ProgressType>::TaskWithProgress(
    int32_t id, std::string title, std::string description,
    TaskPriority priority, TagContainer tags, ProgressArgs &&...progress_args)
    : TaskBase(id, std::move(title), std::move(description), priority,
               std::move(tags)),
      progress_(std::forward<ProgressArgs>(progress_args)...){};

template <ProgressLike ProgressType>
TaskState TaskWithProgress<ProgressType>::GetState() const {
  return progress_.GetState();
}

template <ProgressLike ProgressType>
std::string TaskWithProgress<ProgressType>::GetProgressString() const {
  return progress_.ToString();
}

template <ProgressLike ProgressType>
ProgressType& TaskWithProgress<ProgressType>::GetProgress() {
  return progress_;
}
