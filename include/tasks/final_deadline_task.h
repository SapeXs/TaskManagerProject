#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../tasks/stepped_deadline_task.h"

class FinalDeadlineTask : public SteppedDeadlineTask {
 public:
  FinalDeadlineTask(int32_t id, std::string title, std::string description,
                    TaskPriority priority, TagContainer tags,
                    std::vector<std::string> step_texts,
                    std::string final_deadline_text,
                    std::size_t current_step = 0);

  std::string GetTypeName() const override;

  const std::string& GetFinalDeadlineText() const noexcept;
  void MarkFinalDeadlinePassed();

 private:
  std::string final_deadline_text_;
};
