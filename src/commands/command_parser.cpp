#include "commands/command_parser.h"

#include <sstream>
#include <string>
#include <unordered_map>

namespace {
const std::unordered_map<std::string, CommandType> kCommandMap = {
    {"add", CommandType::kAdd},       {"list", CommandType::kList},
    {"find", CommandType::kFind},     {"remove", CommandType::kRemove},
    {"filter", CommandType::kFilter}, {"help", CommandType::kHelp},
    {"exit", CommandType::kExit},     {"save", CommandType::kSave}};
}

std::vector<std::string> CommandParser::Split_(const std::string& input) const {
  std::vector<std::string> tokens;
  tokens.reserve(10);

  std::istringstream stream(input);
  std::string token;
  while (stream >> token) {
    tokens.push_back(std::move(token));
  }
  return tokens;
}

CommandType CommandParser::ParseCommandType_(const std::string& command_name) const {
  auto it = kCommandMap.find(command_name);
  return (it != kCommandMap.end()) ? it->second : CommandType::kInvalid;
}

Command CommandParser::Parse(const std::string& input) const {
  std::vector<std::string> tokens = Split_(input);

  if (tokens.empty()) {
    return Command(CommandType::kInvalid, {});
  }

  std::vector<std::string> args;

  if (tokens.size() > 1) {
    args.assign(std::next(tokens.begin()), tokens.end());
  }

  return Command(ParseCommandType_(tokens[0]), std::move(args));
}
