#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "commands/command_type.h"

class Command {
 public:
  Command(CommandType type, std::vector<std::string> args = {});

  CommandType GetType() const noexcept;
  std::span<const std::string> GetArgs() const noexcept;
  bool IsValid() const noexcept;

 private:
  CommandType type_;
  std::vector<std::string> args_;
};

std::string_view ToString(CommandType type);
