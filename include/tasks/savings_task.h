#pragma once

#include <cstdint>
#include <string>

#include "core/task_priority.h"
#include "core/task_with_progress.h"
#include "progress/percentage_progress.h"

class SavingsTask : public TaskWithProgress<PercentageProgress> {
public:
  using TagContainer = TaskBase::TagContainer;

  SavingsTask(int32_t id, std::string title, std::string description,
              TaskPriority priority, TagContainer tags, int64_t current_value,
              int64_t target_value);

  std::string_view GetTypeName() const override;

  int64_t GetCurrentValue() const noexcept;
  int64_t GetTargetValue() const noexcept;

  void AddValue(int64_t value) noexcept;
};
