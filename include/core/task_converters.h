#pragma once

#include <cstdint>
#include <string_view>

#include "core/task_priority.h"
#include "core/task_state.h"

std::string_view TaskPriorityToString(TaskPriority priority);
std::string_view TaskStateToString(TaskState state);

bool ParseTaskPriority(std::string_view value, TaskPriority& priority);
bool ParseTaskState(std::string_view value, TaskState& state);

int16_t TaskPriorityToStorageValue(TaskPriority priority);
TaskPriority TaskPriorityFromStorageValue(int16_t value);
