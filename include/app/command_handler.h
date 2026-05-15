#pragma once

#include <cstdint>
#include <mutex>
#include <string>

#include "commands/command.h"
#include "manager/task_manager.h"
#include "storage/task_storage.h"

class CommandHandler {
 public:
  CommandHandler(TaskManager& task_manager, TaskStorage& storage, std::mutex& task_mutex,
                 int32_t& next_id);

  std::string Handle(const Command& command);

 private:
  std::string HandleAdd(const Command& command);
  std::string HandleList();
  std::string HandleFind(const Command& command);
  std::string HandleRemove(const Command& command);
  std::string HandleFilter(const Command& command);
  std::string HandleSave();
  std::string HandleHelp();

  TaskManager& task_manager_;
  TaskStorage& storage_;
  std::mutex& task_mutex_;
  int32_t& next_id_;
};
