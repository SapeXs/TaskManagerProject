#pragma once

#include <string>
#include <vector>

#include "ui/command.h"
#include "ui/command_type.h"

class CommandParser {
 public:
  Command Parse(const std::string& input) const;

 private:
  std::vector<std::string> Split_(const std::string& input) const;
  CommandType ParseCommandType_(const std::string& command_name) const;
};
