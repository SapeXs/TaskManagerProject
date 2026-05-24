#pragma once

#include <concepts>
#include <string>

#include "core/task_state.h"

template <class T>
concept ProgressLike = requires(const T& progress) {
  { progress.ToString() } -> std::same_as<std::string>;
  { progress.GetState() } -> std::same_as<TaskState>;
};
