#pragma once

#include <span>
#include <string>

#include "core/task_base.h"

class TaskFormatter {
 public:
  static std::string FormatTask(const TaskBase& task);
  static std::string FormatTaskList(std::span<const TaskBase* const> tasks);
  static std::string FormatTaskList(std::span<TaskBase* const> tasks);
  static std::string FormatTags(std::span<const std::string> tags);
  static std::string FormatHelp();
};
