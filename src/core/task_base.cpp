#include "core/task_base.h"

using TagContainer = std::unordered_set<std::string>;

TaskBase::TaskBase(int32_t id, std::string title, std::string description,
                   TaskPriority priority, TagContainer tags)
    : id_(id), title_(std::move(title)), description_(std::move(description)),
      priority_(priority), tags_(std::move(tags)) {}

int32_t TaskBase::GetId() const noexcept { return id_; }

const std::string &TaskBase::GetTitle() const noexcept { return title_; }

const std::string &TaskBase::GetDescription() const noexcept {
  return description_;
}

TaskPriority TaskBase::GetPriority() const noexcept { return priority_; }

const TagContainer &TaskBase::GetTags() const noexcept { return tags_; }

bool TaskBase::HasTag(const std::string &tag) const {
  if (tags_.find(tag) != tags_.end()) {
    return true;
  }
  return false;
}

void TaskBase::AddTag(const std::string &tag) { tags_.insert(tag); }

void TaskBase::RemoveTag(const std::string &tag) { tags_.erase(tag); }

void TaskBase::SetTitle(std::string title) { title_ = std::move(title); }

void TaskBase::SetDescription(std::string description) {
  description_ = std::move(description);
}

void TaskBase::SetPriority(TaskPriority priority) { priority_ = priority; }

void TaskBase::SetTags(TagContainer tags) { tags_ = std::move(tags); }

void TaskBase::SetTags(std::string tag) {
  tags_.clear();
  tags_.insert(std::move(tag));
}
