#include "tasks/bounded_recurring_task.h"
#include "core/task_name.h"

BoundedRecurringTask::BoundedRecurringTask(int32_t id, std::string title, std::string description,
                       TaskPriority priority, TagContainer tags,
                       int64_t seconds_left, int64_t repeat_interval_seconds,
                       int32_t repeats_left) : RecurringTask(id, title, description, priority, tags, seconds_left, repeat_interval_seconds), repeats_left_(repeats_left) {}

std::string BoundedRecurringTask::GetTypeName() const {
    return kNameBoundedRecurringTask;
}

int32_t BoundedRecurringTask::GetRepeatsLeft() const {
    return repeats_left_;
}

void BoundedRecurringTask::SetRepeatsLeft(int32_t repeats) {
    repeats_left_ = repeats;
}

void BoundedRecurringTask::ReduceRepeatsLeft() {
    --repeats_left_;
}

bool BoundedRecurringTask::CanRepeat() const {
    return repeats_left_ > 0;
}

void BoundedRecurringTask::CompleteOccurrence() {
    repeats_left_ = 0;
}
