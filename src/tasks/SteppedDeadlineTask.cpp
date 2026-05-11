#include "tasks/stepped_deadline_task.h"

SteppedDeadlineTask::SteppedDeadlineTask(int32_t id, std::string title, std::string description,
                                         TaskPriority priority, TagContainer tags,
                                         int64_t deadline_timestamp, int32_t steps_completed,
                                         int32_t total_steps)
    : DeadlineTask(id, std::move(title), std::move(description), priority, std::move(tags),
                   deadline_timestamp),
      steps_completed_(steps_completed),
      total_steps_(total_steps) {}

std::string SteppedDeadlineTask::GetTypeName() const {
    return "SteppedDeadlineTask";
}

int32_t SteppedDeadlineTask::GetStepsCompleted() const noexcept {
    return steps_completed_;
}

int32_t SteppedDeadlineTask::GetTotalSteps() const noexcept {
    return total_steps_;
}

void SteppedDeadlineTask::SetStepsCompleted(int32_t steps) {
    steps_completed_ = steps;
}

void SteppedDeadlineTask::SetTotalSteps(int32_t steps) {
    total_steps_ = steps;
}

void SteppedDeadlineTask::AdvanceStep() {
    ++steps_completed_;
}

bool SteppedDeadlineTask::IsFullyCompleted() const noexcept {
    return steps_completed_ >= total_steps_;
}