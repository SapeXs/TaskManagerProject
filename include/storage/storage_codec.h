#pragma once

#include <string>
#include <vector>

#include "core/task_base.h"
#include "core/task_priority.h"

std::string EscapeStorageField(const std::string& value);
std::vector<std::string> SplitStorageLine(const std::string& value);

std::string TagsToStorageString(const TaskBase::TagContainer& tags);
TaskBase::TagContainer ParseStorageTags(const std::string& value);

TaskPriority ParseTaskPriority(const std::string& value);
