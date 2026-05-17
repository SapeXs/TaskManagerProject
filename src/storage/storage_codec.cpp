#include "storage/storage_codec.h"

#include <sstream>

#include "core/task_converters.h"

std::string EscapeStorageField(const std::string& value) {
  std::string result;

  for (char ch : value) {
    if (ch == '\\' || ch == '|') {
      result += '\\';
    }
    result += ch;
  }

  return result;
}

std::vector<std::string> SplitStorageLine(const std::string& line) {
  std::vector<std::string> result;
  std::string current;
  bool escaped = false;

  for (char ch : line) {
    if (escaped) {
      current += ch;
      escaped = false;
    } else if (ch == '\\') {
      escaped = true;
    } else if (ch == '|') {
      result.push_back(current);
      current.clear();
    } else {
      current += ch;
    }
  }

  result.push_back(current);
  return result;
}

std::string TagsToStorageString(const TaskBase::TagContainer& tags) {
  std::string result;
  bool first = true;

  for (const std::string& tag : tags) {
    if (!first) {
      result += ',';
    }

    first = false;
    result += tag;
  }

  return result;
}

TaskBase::TagContainer ParseStorageTags(const std::string& value) {
  TaskBase::TagContainer tags;
  std::stringstream stream(value);
  std::string tag;

  while (std::getline(stream, tag, ',')) {
    if (!tag.empty()) {
      tags.insert(tag);
    }
  }

  return tags;
}

TaskPriority ParseTaskPriority(const std::string& value) {
  return TaskPriorityFromStorageValue(std::stoi(value));
}
