#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "commands/command.h"

class CommandParser {
 public:
  Command Parse(std::string_view input) const;

 private:
  std::vector<std::string> Split_(std::string_view input) const;
  CommandType ParseCommandType_(std::string_view command_name) const;
};
