#pragma once

enum class CommandType {
  kAdd,
  kList,
  kFind,
  kRemove,
  kFilter,
  kHelp,
  kExit,
  kSave,
  kClear,

  kSetTitle,
  kSetDescription,
  kSetPriority,
  kAddTag,
  kRemoveTag,
  kSetTime,
  kSetInterval,
  kSetRepeats,
  kAddValue,
  kAdvance,
  kOverdue,
  kReset,

  kInvalid,
};
