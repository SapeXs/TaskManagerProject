#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

#include "core/task_priority.h"
#include "core/task_state.h"

class TaskBase {
 public:
  using TagContainer = std::unordered_set<std::string>;

  TaskBase(int32_t id, std::string title, std::string description,
           TaskPriority priority, TagContainer tags);

  virtual ~TaskBase() = default;

  virtual std::string_view GetTypeName() const = 0;
  virtual TaskState GetState() const = 0;
  virtual std::string GetProgressString() const = 0;
  virtual std::vector<std::string> GetStorageFields() const = 0;

  int32_t GetId() const noexcept;
  const std::string& GetTitle() const noexcept;
  const std::string& GetDescription() const noexcept;
  TaskPriority GetPriority() const noexcept;
  const TagContainer &GetTags() const noexcept;

  bool HasTag(const std::string &tag) const;
  void AddTag(const std::string &tag);
  void RemoveTag(const std::string &tag);

 protected:
  void SetTitle(std::string title);
  void SetDescription(std::string description);
  void SetPriority(TaskPriority priority);
  void SetTags(TagContainer tags);
  void SetTags(std::string tag);

 private:
  int32_t id_;
  std::string title_;
  std::string description_;
  TaskPriority priority_;
  TagContainer tags_;
};
