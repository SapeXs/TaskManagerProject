#include "core/task_with_progress.h"

using TagContainer = TaskBase::TagContainer;

template <ProgressLike ProgressType>
TaskWithProgress<ProgressType>::TaskWithProgress(int32_t id, std::string title,
                                                 std::string description,
                                                 TaskPriority priority,
                                                 TagContainer tags)
    : TaskBase(id, std::move(title), std::move(description), priority,
               std::move(tags)){};

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
