#pragma once

#include <string>
#include <utility>

#include "core/task_state.h"

class TextProgress {
public:
  TextProgress(std::string text, TaskState state)
      : text_(std::move(text)), state_(state) {}

  const std::string& GetText() const noexcept { return text_; }

  void Set(std::string text, TaskState state) {
    text_ = std::move(text);
    state_ = state;
  }

  std::string ToString() const { return text_; }

  TaskState GetState() const noexcept { return state_; }

private:
  std::string text_;
  TaskState state_;
};
