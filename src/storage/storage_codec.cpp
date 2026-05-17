#include "storage/storage_codec.h"

#include "core/task_converters.h"

std::string EscapeStorageField(std::string_view value) {
  std::string result;

  for (char ch : value) {
    if (ch == '\\' || ch == '|') {
      result += '\\';
    }
    result += ch;
  }

  return result;
}

std::vector<std::string> SplitStorageLine(std::string_view line) {
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

TaskBase::TagContainer ParseStorageTags(std::string_view value) {
  TaskBase::TagContainer tags;
  std::size_t start = 0;

  while (start <= value.size()) {
    std::size_t end = value.find(',', start);
    if (end == std::string_view::npos) {
      end = value.size();
    }

    std::string_view tag = value.substr(start, end - start);

    if (!tag.empty()) {
      tags.insert(std::string(tag));
    }

    start = end + 1;

    if (end == value.size()) {
      break;
    }
  }

  return tags;
}

TaskPriority ParseTaskPriority(std::string_view value) {
  return TaskPriorityFromStorageValue(std::stoi(std::string(value)));
}
