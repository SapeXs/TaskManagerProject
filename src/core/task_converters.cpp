#include "core/task_converters.h"

std::string_view TaskPriorityToString(TaskPriority priority) {
  switch (priority) {
    case TaskPriority::kLowPriority:
      return "low";
    case TaskPriority::kMediumPriority:
      return "medium";
    case TaskPriority::kHighPriority:
      return "high";
    case TaskPriority::kCriticalPriority:
      return "critical";
    default:
      return "unknown";
  }
}

std::string_view TaskStateToString(TaskState state) {
  switch (state) {
    case TaskState::kNotStarted:
      return "not_started";
    case TaskState::kInProgress:
      return "in_progress";
    case TaskState::kDone:
      return "done";
    case TaskState::kOverdue:
      return "overdue";
    case TaskState::kScheduled:
      return "scheduled";
    default:
      return "unknown";
  }
}

bool ParseTaskPriority(std::string_view value, TaskPriority& priority) {
  if (value == "low") {
    priority = TaskPriority::kLowPriority;
    return true;
  }

  if (value == "medium") {
    priority = TaskPriority::kMediumPriority;
    return true;
  }

  if (value == "high") {
    priority = TaskPriority::kHighPriority;
    return true;
  }

  if (value == "critical") {
    priority = TaskPriority::kCriticalPriority;
    return true;
  }

  return false;
}

bool ParseTaskState(std::string_view value, TaskState& state) {
  if (value == "not_started") {
    state = TaskState::kNotStarted;
    return true;
  }

  if (value == "in_progress") {
    state = TaskState::kInProgress;
    return true;
  }

  if (value == "done") {
    state = TaskState::kDone;
    return true;
  }

  if (value == "overdue") {
    state = TaskState::kOverdue;
    return true;
  }

  if (value == "scheduled") {
    state = TaskState::kScheduled;
    return true;
  }

  return false;
}

int16_t TaskPriorityToStorageValue(TaskPriority priority) {
  switch (priority) {
    case TaskPriority::kLowPriority:
      return 0;
    case TaskPriority::kMediumPriority:
      return 1;
    case TaskPriority::kHighPriority:
      return 2;
    case TaskPriority::kCriticalPriority:
      return 3;
    default:
      return 1;
  }
}

TaskPriority TaskPriorityFromStorageValue(int16_t value) {
  switch (value) {
    case 0:
      return TaskPriority::kLowPriority;
    case 1:
      return TaskPriority::kMediumPriority;
    case 2:
      return TaskPriority::kHighPriority;
    case 3:
      return TaskPriority::kCriticalPriority;
    default:
      return TaskPriority::kMediumPriority;
  }
}
