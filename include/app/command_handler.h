#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>

#include "commands/command.h"
#include "manager/task_manager.h"
#include "storage/task_storage.h"

class CommandHandler {
 public:
  CommandHandler(TaskManager& task_manager, TaskStorage& storage, std::mutex& task_mutex,
                 int32_t& next_id, std::unordered_set<int32_t>& notified_tasks);

  std::string Handle(const Command& command);

 private:
  std::string HandleAdd(const Command& command);
  std::string HandleList();
  std::string HandleFind(const Command& command);
  std::string HandleRemove(const Command& command);
  std::string HandleFilter(const Command& command);
  std::string HandleSave();
  std::string HandleClear();
  std::string HandleHelp();
  std::string HandleSetTitle(const Command& command);
  std::string HandleSetDescription(const Command& command);
  std::string HandleSetPriority(const Command& command);
  std::string HandleAddTag(const Command& command);
  std::string HandleRemoveTag(const Command& command);
  std::string HandleSetTime(const Command& command);
  std::string HandleSetInterval(const Command& command);
  std::string HandleSetRepeats(const Command& command);
  std::string HandleAddValue(const Command& command);
  std::string HandleAdvance(const Command& command);
  std::string HandleOverdue(const Command& command);
  std::string HandleReset(const Command& command);

  std::string HandleSetDeadline(const Command& command);
  std::string HandleSetDate(const Command& command);
  std::string HandleSetRemindBefore(const Command& command);

  void SaveTasksLocked();
  void ForgetNotificationLocked(int32_t task_id);
  void ClearNotificationsLocked();

  TaskManager& task_manager_;
  TaskStorage& storage_;
  std::mutex& task_mutex_;
  int32_t& next_id_;
  std::unordered_set<int32_t>& notified_tasks_;
};
