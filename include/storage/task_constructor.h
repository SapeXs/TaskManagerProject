#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/task_base.h"

std::unique_ptr<TaskBase> CreateTaskFromStorageFields(const std::vector<std::string>& fields);
