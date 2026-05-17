#pragma once

enum class CommandType {
  kAdd,
  kList,
  kFind,
  kRemove,
  kFilter,
  kTags,
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

  kSetDeadline,
  kSetDate,

  kInvalid,
};
