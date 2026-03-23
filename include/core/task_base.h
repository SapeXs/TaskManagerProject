#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>

#include "task_priority.h"
#include "task_state.h"

class TaskBase {
public:
  using TagContainer = std::unordered_set<std::string>;

  TaskBase(int32_t id, std::string title, std::string description,
           TaskPriority priority, TagContainer tags);

  /*virtual ~TaskBase() = default;
  TaskBase(const TaskBase&) = default;
  TaskBase(TaskBase&&) noexcept = default;
  TaskBase& operator=(const TaskBase&) = default;
  TaskBase& operator=(TaskBase&&) noexcept = default;*/

  virtual std::string GetTypeName() const = 0;
  virtual TaskState GetState() const = 0;
  virtual std::string GetProgressString() const = 0;
  virtual void Print(std::ostream &out) const = 0;

  int32_t GetId() const noexcept;
  const std::string &GetTitle() const noexcept;
  const std::string &GetDescription() const noexcept;
  TaskPriority GetPriority() const noexcept;
  const TagContainer &GetTags() const noexcept;

  bool HasTag(std::string_view tag) const;
  void AddTag(std::string tag);
  void RemoveTag(std::string_view tag);

protected:
  void SetTitle(std::string title);
  void SetDescription(std::string description);
  void SetPriority(TaskPriority priority);
  void SetTags(TagContainer tags);

private:
  int32_t id_;
  std::string title_;
  std::string description_;
  TaskPriority priority_;
  TagContainer tags_;
};
