#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core/task_base.h"

enum class AddTaskType {
  kReminder,
  kRecurring,
  kBoundedRecurring,
  kSavings,
  kSteppedDeadline,
  kFinalDeadline,
};

struct AddTaskOptions {
  AddTaskType type = AddTaskType::kReminder;

  std::string title;
  std::string description;

  TaskPriority priority = TaskPriority::kMediumPriority;
  TaskBase::TagContainer tags;

  std::optional<std::chrono::system_clock::time_point> deadline;

  std::optional<int64_t> repeat_interval_seconds;
  std::optional<int32_t> repeats_left;

  std::optional<int64_t> current_value;
  std::optional<int64_t> target_value;

  std::vector<std::string> step_texts;
  std::optional<std::size_t> current_step;

  std::optional<std::string> final_deadline_text;
};

std::unique_ptr<TaskBase> CreateTaskFromStorageFields(const std::vector<std::string>& fields);

std::unique_ptr<TaskBase> CreateTaskFromAddOptions(int32_t id, AddTaskOptions options,
                                                   std::string& error);
