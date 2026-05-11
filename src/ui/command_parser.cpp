#include "ui/command_parser.h"
#include <sstream>
#include <unordered_map>

std::vector<std::string> CommandParser::Split_(const std::string& input) const {
  std::vector<std::string> tokens;
  std::istringstream stream(input);
  std::string token;
  while (stream >> token) {
    tokens.push_back(std::move(token));
  }
  return tokens;
}

CommandType CommandParser::ParseCommandType_(const std::string& command_name) const {
  static const std::unordered_map<std::string, CommandType> kCommandMap = {
      {"add", CommandType::kAdd},     {"list", CommandType::kList},
      {"find", CommandType::kFind},   {"remove", CommandType::kRemove},
      {"filter", CommandType::kFilter}, {"help", CommandType::kHelp},
      {"exit", CommandType::kExit}
  };
  auto it = kCommandMap.find(command_name);
  return (it != kCommandMap.end()) ? it->second : CommandType::kInvalid;
}

Command CommandParser::Parse(const std::string& input) const {
  std::vector<std::string> tokens = Split_(input);
  if (tokens.empty()) {
    return Command(CommandType::kInvalid, {});
  }
  return Command(ParseCommandType_(tokens[0]),
                 std::vector<std::string>(tokens.begin() + 1, tokens.end()));
}
