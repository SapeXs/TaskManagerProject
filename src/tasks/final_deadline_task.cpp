#include "tasks/final_deadline_task.h"

FinalDeadlineTask::FinalDeadlineTask(int32_t id, std::string title, std::string description,
                                     TaskPriority priority, TagContainer tags,
                                     int64_t deadline_timestamp)
    : DeadlineTask(id, std::move(title), std::move(description), priority, std::move(tags),
                   deadline_timestamp) {}

std::string FinalDeadlineTask::GetTypeName() const {
    return "FinalDeadlineTask";
}

int64_t FinalDeadlineTask::GetDeadlineTimestamp() const noexcept {
    return deadline_timestamp_;
}

void FinalDeadlineTask::SetDeadlineTimestamp(int64_t timestamp) {
    deadline_timestamp_ = timestamp;
}

bool FinalDeadlineTask::IsExpired() const noexcept {
    //хз как тут время делать 
    return false;
}