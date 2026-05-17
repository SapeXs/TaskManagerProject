#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "core/task_base.h"
#include "core/task_priority.h"

std::string EscapeStorageField(std::string_view value);
std::vector<std::string> SplitStorageLine(std::string_view line);

std::string TagsToStorageString(const TaskBase::TagContainer& tags);
TaskBase::TagContainer ParseStorageTags(std::string_view value);

TaskPriority ParseTaskPriority(std::string_view value);
